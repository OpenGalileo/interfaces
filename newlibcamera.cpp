#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
#include <sys/mman.h>
#include <fstream>
#include <png.h>

#include <libcamera/libcamera.h>

using namespace libcamera;
using namespace std::chrono_literals;

static void requestComplete(Request *request);
static void writePNG(const std::string &filename, uint8_t *rgb, int width, int height);

#define WIDTH 4056
#define HEIGHT 3040

static std::shared_ptr<libcamera::Camera> camera;
int main() {
    std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();
    cm->start();
    for (auto const &camera : cm->cameras())
        std::cout << camera->id() << std::endl;
    
    auto cameras = cm->cameras();
    if (cameras.empty()) {
        std::cout << "No cameras were identified on the system.\n";
        cm->stop();
        return EXIT_FAILURE;
    }

    std::string cameraName = cameras[0]->id(); 

    camera = cm->get(cameraName);
    camera->acquire();

    std::unique_ptr<CameraConfiguration> config = camera->generateConfiguration( {StreamRole::Viewfinder}); // Should prob be still
    StreamConfiguration &streamConfig = config->at(0);
    std::cout << "Default viewfinder configuration is: " << streamConfig.toString() << std::endl;

    streamConfig.size.width = 4056;
    streamConfig.size.height = 3040;

    config->validate();
    std::cout << "Validated viewfinder configuration is: " << streamConfig.toString() << std::endl;

    camera->configure(config.get());

    FrameBufferAllocator *allocator = new FrameBufferAllocator(camera);

    for (StreamConfiguration &cfg : *config) {
        int ret = allocator->allocate(cfg.stream());
        if (ret < 0) {
            std::cerr << "Can't allocate buffers" << std::endl;
            return -ENOMEM;
        }

        size_t allocated = allocator->buffers(cfg.stream()).size();
        std::cout << "Allocated  " << allocated << " buffers for stream\n";
    }

    Stream *stream = streamConfig.stream();
    const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
    std::vector<std::unique_ptr<Request>> requests;

    for (unsigned int i = 0; i < buffers.size(); ++i) {
        std::unique_ptr<Request> request = camera->createRequest();
        if (!request) {
            std::cerr << "Can't create request\n";
            return -ENOMEM;
        }

        const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream, buffer.get());
        if (ret < 0) {
            std::cerr << "Can't set buffer for request\n";
            return ret;
        }

        requests.push_back(std::move(request));
    }

    camera->requestCompleted.connect(requestComplete);
    camera->start();
    for (std::unique_ptr<Request> &request : requests)
        camera->queueRequest(request.get());

    std::this_thread::sleep_for(3000ms);

    camera->stop();
    allocator->free(stream);
    delete allocator;
    camera->release();
    camera.reset();
    cm->stop();

    return 0;
}


static void requestComplete(Request *request) {
    if(request->status() == Request::RequestCancelled)
        return;
    
    const std::map<const Stream*, FrameBuffer*> &buffers = request->buffers();
    for (auto bufferPair : buffers) {
        FrameBuffer *buffer = bufferPair.second;
        const FrameMetadata &metadata = buffer->metadata();

        std::cout << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence << " bytesused: ";

        unsigned int nplane = 0;
        for (const FrameMetadata::Plane &plane : metadata.planes()) {
            //const uint8_t *data = plane.
            std::cout << plane.bytesused;
            if (++nplane < metadata.planes().size()) std::cout << "/";
        }
        std::cout << std::endl;
        // ---- Save raw frame data
        int index = 0;
        for (const FrameBuffer::Plane &plane : buffer->planes()) {
            void *memory = mmap(nullptr,
                                plane.length,
                                PROT_READ,
                                MAP_SHARED,
                                plane.fd.get(),
                                plane.offset);
            if (memory == MAP_FAILED) {
                std::cerr << "mmap failed" << std::endl;
                continue;
            }

            uint16_t *raw12 = (uint16_t *)memory;
            std::vector<uint8_t> rgb(WIDTH * HEIGHT * 3);

            auto getRaw = [&](int x, int y) {
                return raw12[y * WIDTH + x] >> 4;
            };

            for (int y = 0; y < HEIGHT - 1; ++y) {
                for (int x = 0; x < WIDTH - 1; ++ x) {
                    uint8_t R, G, B;

                    bool yOdd = y & 1;
                    bool xOdd = x & 1;

                    if (!yOdd && !xOdd) {          // B G
                        B = getRaw(x, y);
                        G = getRaw(x + 1, y);
                        R = getRaw(x + 1, y + 1);

                    } else if (!yOdd && xOdd) {    // G R
                        B = getRaw(x - 1, y);
                        G = getRaw(x, y);
                        R = getRaw(x, y + 1);

                    } else if (yOdd && !xOdd) {    // G B
                        B = getRaw(x, y - 1);
                        G = getRaw(x, y);
                        R = getRaw(x + 1, y);

                    } else {                       // R G
                        B = getRaw(x - 1, y - 1);
                        G = getRaw(x, y);
                        R = getRaw(x, y);
                    }

                    int idx = (y * WIDTH + x) * 3;

                    rgb[idx + 0] = R;
                    rgb[idx + 1] = G;
                    rgb[idx + 2] = B;
                }
            }

            writePNG("capture.png", rgb.data(), WIDTH, HEIGHT);
            std::cout << "saved capture.png\n";


            std::string filename = "frame_" + std::to_string(metadata.sequence)
                                 + "_plane" + std::to_string(index) + ".raw";
            std::ofstream file(filename, std::ios::binary);
            file.write(static_cast<const char *>(memory), metadata.planes()[index].bytesused);

            munmap(memory, plane.length);
            index++;
        }
    }


    request->reuse(Request::ReuseBuffers);
    camera->queueRequest(request);
}

static void writePNG(const std::string &filename, uint8_t *rgb, int width, int height) {
    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        std::cerr << "Failed to open PNG file\n";
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr,nullptr);
    png_infop info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png, info);

    for (int y = 0; y < height; ++y) {
        png_write_row(png, rgb + y * width * 3);
    }

    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

// static void requestComplete(Request *request) {
//     if(request->status() == Request::RequestCancelled)
//         return;
    
//     const std::map<const Stream*, FrameBuffer*> &buffers = request->buffers();
//     for (auto bufferPair : buffers) {
//         FrameBuffer *buffer = bufferPair.second;
//         const FrameMetadata &metadata = buffer->metadata();

//         std::cout << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence << " bytesused: ";

//         unsigned int nplane = 0;
//         for (const FrameMetadata::Plane &plane : metadata.planes()) {
//             //const uint8_t *data = plane.
//             std::cout << plane.bytesused;
//             if (++nplane < metadata.planes().size()) std::cout << "/";
//         }
//     }
//     request->reuse(Request::ReuseBuffers);
//     camera->queueRequest(request);
// }
