#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <vector>
#include <sys/mman.h>

#include <libcamera/libcamera.h>

using namespace libcamera;

static std::shared_ptr<Camera> camera;

int main() {
    CameraManager cm;
    if (cm.start()) {
        std::cerr << "Failed to start CameraManager\n";
        return EXIT_FAILURE;
    }

    const auto &cams = cm.cameras();
    if (cams.empty()) {
        std::cerr << "No cameras found\n";
        cm.stop();
        return EXIT_FAILURE;
    }

    std::string camId = cams[0]->id();
    camera = cm.get(camId);
    if (!camera) {
        std::cerr << "Failed to get camera " << camId << "\n";
        cm.stop();
        return EXIT_FAILURE;
    }

    if (camera->acquire()) {
        std::cerr << "Failed to acquire camera\n";
        cm.stop();
        return EXIT_FAILURE;
    }

    // Still capture configuration
    std::unique_ptr<CameraConfiguration> config = camera->generateConfiguration({ StreamRole::StillCapture });
    if (!config) {
        std::cerr << "Failed to generate configuration\n";
        camera->release();
        cm.stop();
        return EXIT_FAILURE;
    }

    StreamConfiguration &streamCfg = config->at(0);
    if (config->validate() != CameraConfiguration::Status::Valid) {
        std::cerr << "Configuration changed on validation\n";
    }

    camera->configure(config.get());

    FrameBufferAllocator allocator(camera.get());
    if (allocator.allocate(streamCfg.stream()) < 0) {
        std::cerr << "Failed to allocate buffers\n";
        camera->release();
        cm.stop();
        return EXIT_FAILURE;
    }

    const auto &buffers = allocator.buffers(streamCfg.stream());
    if (buffers.empty()) {
        std::cerr << "No buffers allocated\n";
        camera->release();
        cm.stop();
        return EXIT_FAILURE;
    }

    auto request = camera->createRequest();
    if (!request) {
        std::cerr << "Failed to create request\n";
        camera->release();
        cm.stop();
        return EXIT_FAILURE;
    }

    if (request->addBuffer(streamCfg.stream(), buffers[0].get()) < 0) {
        std::cerr << "Failed to add buffer\n";
        camera->release();
        cm.stop();
        return EXIT_FAILURE;
    }

    bool done = false;

    camera->requestCompleted.connect([&](Request *r) {
        if (r->status() != Request::RequestCancelled) {
            FrameBuffer *buffer = r->buffers().begin()->second;
            const FrameMetadata &meta = buffer->metadata();

            void *mem = mmap(nullptr, buffer->planes()[0].length, PROT_READ, MAP_SHARED, buffer->planes()[0].fd.get(), buffer->planes()[0].offset);
            if (mem == MAP_FAILED) {
                std::cerr << "mmap failed\n";
                done = true;
                return;
            }

            std::ofstream file("capture.raw", std::ios::binary);
            file.write(reinterpret_cast<const char *>(mem), meta.planes()[0].bytesused);
            munmap(mem, buffer->planes()[0].length);

            std::cout << "Saved capture.raw (" << meta.planes()[0].bytesused << " bytes)\n";
        }
        done = true;
    });

    camera->start();
    camera->queueRequest(request.get());

    // Wait for capture
    while (!done)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    camera->stop();
    allocator.free(streamCfg.stream());
    camera->release();
    cm.stop();

    return 0;
}
