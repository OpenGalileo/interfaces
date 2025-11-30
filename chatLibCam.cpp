// still_capture.cpp
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <cstring>
#include <sys/mman.h>
#include <png.h>

#include <libcamera/libcamera.h>

using namespace libcamera;
using namespace std::chrono_literals;

// ----- Simple worker queue to save PNGs off the camera callback thread -----
struct SaveTask {
    std::string filename;
    std::vector<uint8_t> rgb;
    int width;
    int height;
};

class Saver {
public:
    Saver() : stop_{false} { worker_ = std::thread([this]{ this->run(); }); }
    ~Saver() {
        {
            std::unique_lock<std::mutex> lk(mutex_);
            stop_ = true;
            cv_.notify_all();
        }
        worker_.join();
    }
    void push(SaveTask &&t) {
        std::unique_lock<std::mutex> lk(mutex_);
        q_.push(std::move(t));
        cv_.notify_one();
    }

private:
    void run() {
        while (true) {
            SaveTask task;
            {
                std::unique_lock<std::mutex> lk(mutex_);
                cv_.wait(lk, [&]{ return stop_ || !q_.empty(); });
                if (stop_ && q_.empty()) return;
                task = std::move(q_.front());
                q_.pop();
            }
            writePNG(task.filename, task.rgb.data(), task.width, task.height);
            std::cout << "Saved " << task.filename << "\n";
        }
    }

    // PNG writer (blocking) used by worker thread
    static void writePNG(const std::string &filename, uint8_t *rgb, int width, int height) {
        FILE *fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            std::cerr << "Failed to open PNG file " << filename << "\n";
            return;
        }

        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png) { fclose(fp); return; }
        png_infop info = png_create_info_struct(png);
        if (!info) { png_destroy_write_struct(&png, nullptr); fclose(fp); return; }

        if (setjmp(png_jmpbuf(png))) {
            png_destroy_write_struct(&png, &info);
            fclose(fp);
            return;
        }

        png_init_io(png, fp);
        png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB,
                    PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        png_write_info(png, info);

        // rows
        for (int y = 0; y < height; ++y)
            png_write_row(png, rgb + y * width * 3);

        png_write_end(png, nullptr);
        png_destroy_write_struct(&png, &info);
        fclose(fp);
    }

    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<SaveTask> q_;
    bool stop_;
};

// ----- Global camera ptr needed by requestComplete to requeue - keep minimal -----
static std::shared_ptr<libcamera::Camera> g_camera;
static Saver g_saver;

// Forward
static void requestComplete(Request *request);

// ----- Helper: simple bilinear debayer for RGGB -----
// Input: raw16[y * stridePixels + x] -- 16-bit container, data possibly >8 bits
// Output: rgb vector (row-major, 3 bytes per pixel)
static void debayerRGGB_bilinear(const uint16_t *raw16, int stridePixels,
                                 int width, int height, std::vector<uint8_t> &rgb,
                                 int data_shift = 4 /* default >>4 to convert 12-bit->8-bit */) {
    auto get = [&](int x, int y) -> uint8_t {
        // clamp coords
        if (x < 0) x = 0;
        if (x >= width) x = width - 1;
        if (y < 0) y = 0;
        if (y >= height) y = height - 1;
        uint16_t v = raw16[y * stridePixels + x];
        if (data_shift >= 0) v = v >> data_shift; // bring to 8-bit range
        return static_cast<uint8_t>(v & 0xFF);
    };

    for (int y = 0; y < height; ++y) {
        bool yOdd = y & 1;
        for (int x = 0; x < width; ++x) {
            bool xOdd = x & 1;
            uint8_t R=0,G=0,B=0;

            // RGGB pattern:
            // (0,0) R  G
            // (1,0) G  B
            if (!yOdd && !xOdd) {          // R pixel
                R = get(x,y);
                // G averages left/right/up/down
                G = ( (int)get(x-1,y) + get(x+1,y) + get(x,y-1) + get(x,y+1) ) / 4;
                // B averages diagonal
                B = ( (int)get(x-1,y-1) + get(x+1,y-1) + get(x-1,y+1) + get(x+1,y+1) ) / 4;
            } else if (!yOdd && xOdd) {   // G (on R row)
                G = get(x,y);
                R = ( (int)get(x-1,y) + get(x+1,y) ) / 2;
                B = ( (int)get(x,y-1) + get(x,y+1) ) / 2;
            } else if (yOdd && !xOdd) {   // G (on B row)
                G = get(x,y);
                R = ( (int)get(x,y-1) + get(x,y+1) ) / 2;
                B = ( (int)get(x-1,y) + get(x+1,y) ) / 2;
            } else {                       // B pixel
                B = get(x,y);
                G = ( (int)get(x-1,y) + get(x+1,y) + get(x,y-1) + get(x,y+1) ) / 4;
                R = ( (int)get(x-1,y-1) + get(x+1,y-1) + get(x-1,y+1) + get(x+1,y+1) ) / 4;
            }

            int idx = (y * width + x) * 3;
            rgb[idx+0] = R;
            rgb[idx+1] = G;
            rgb[idx+2] = B;
        }
    }
}

// ----- Main -----
int main() {
    // Camera manager and discovery
    std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();
    if (cm->start()) {
        std::cerr << "Failed to start CameraManager\n";
        return EXIT_FAILURE;
    }

    // list cameras
    const auto &cams = cm->cameras();
    if (cams.empty()) {
        std::cerr << "No cameras found\n";
        cm->stop();
        return EXIT_FAILURE;
    }
    std::cout << "Detected cameras:\n";
    for (auto const &c : cams) std::cout << "  " << c->id() << "\n";

    // pick first camera
    std::string camId = cams[0]->id();
    g_camera = cm->get(camId);
    if (!g_camera) {
        std::cerr << "Failed to get camera " << camId << "\n";
        cm->stop();
        return EXIT_FAILURE;
    }

    if (g_camera->acquire()) {
        std::cerr << "Failed to acquire camera\n";
        cm->stop();
        return EXIT_FAILURE;
    }

    // Generate configuration for stills (full resolution)
    std::unique_ptr<CameraConfiguration> config = g_camera->generateConfiguration({ StreamRole::StillCapture });
    if (!config) {
        std::cerr << "Failed to generate configuration\n";
        g_camera->release();
        cm->stop();
        return EXIT_FAILURE;
    }

    StreamConfiguration &streamCfg = config->at(0);
    std::cout << "Initial stream config: " << streamCfg.toString() << "\n";

    // (Optional) Change desired size here if you want different still resolution
    // streamCfg.size.width = 4056;
    // streamCfg.size.height = 3040;

    // Validate and apply config
    if (config->validate() != CameraConfiguration::Status::Valid) {
        std::cerr << "Configuration validation failed or changed; using validated values.\n";
    }
    std::cout << "Validated stream config: " << streamCfg.toString() << "\n";

    // Get final values
    const int width = streamCfg.size.width;
    const int height = streamCfg.size.height;
    const PixelFormat pixelfmt = streamCfg.pixelFormat;
    std::cout << "Final size: " << width << "x" << height << " pixel format: " << pixelfmt.toString() << "\n";

    // Configure camera
    if (g_camera->configure(config.get())) {
        std::cerr << "Failed to configure camera\n";
        g_camera->release();
        cm->stop();
        return EXIT_FAILURE;
    }

    // Allocate buffers
    FrameBufferAllocator allocator(g_camera.get());
    for (auto &cfg : *config) {
        if (allocator.allocate(cfg.stream()) < 0) {
            std::cerr << "Failed to allocate buffers\n";
            g_camera->release();
            cm->stop();
            return EXIT_FAILURE;
        }
        std::cout << "Allocated " << allocator.buffers(cfg.stream()).size() << " buffers\n";
    }

    Stream *stream = streamCfg.stream();
    const auto &buffers = allocator.buffers(stream);
    std::vector<std::unique_ptr<Request>> requests;

    for (unsigned int i = 0; i < buffers.size(); ++i) {
        auto req = g_camera->createRequest();
        if (!req) {
            std::cerr << "Failed to create request\n";
            return EXIT_FAILURE;
        }
        if (req->addBuffer(stream, buffers[i].get()) < 0) {
            std::cerr << "Failed to add buffer to request\n";
            return EXIT_FAILURE;
        }
        requests.push_back(std::move(req));
    }

    // Connect callback
    g_camera->requestCompleted.connect(requestComplete);

    // Start camera
    if (g_camera->start()) {
        std::cerr << "Failed to start camera\n";
        return EXIT_FAILURE;
    }

    // Queue all requests (continuous capture). If you want only one capture, queue one.
    for (auto &r : requests)
        g_camera->queueRequest(r.get());

    // Let it run for a bit (or replace with wait-for-key etc.)
    std::this_thread::sleep_for(3000ms);

    // Stop
    g_camera->stop();

    // Free allocator resources
    allocator.free(stream);

    g_camera->release();
    g_camera.reset();
    cm->stop();

    // allow saver destructor to finish
    return 0;
}

// ----- Request completion callback -----
static void requestComplete(Request *request) {
    if (!request) return;
    if (request->status() == Request::RequestCancelled)
        return;

    const auto &buffers = request->buffers();
    for (const auto &bp : buffers) {
        FrameBuffer *buffer = bp.second;
        const FrameMetadata &metadata = buffer->metadata();
        std::cout << "seq: " << std::setw(6) << std::setfill('0') << metadata.sequence << "  ";

        // print plane bytesused
        unsigned int nplane = 0;
        for (const FrameMetadata::Plane &p : metadata.planes()) {
            std::cout << p.bytesused;
            if (++nplane < metadata.planes().size()) std::cout << "/";
        }
        std::cout << "\n";

        // iterate planes and process first plane as RAW Bayer
        int planeIndex = 0;
        for (const FrameBuffer::Plane &plane : buffer->planes()) {
            // mmap plane
            void *mem = mmap(nullptr, plane.length, PROT_READ, MAP_SHARED, plane.fd.get(), plane.offset);
            if (mem == MAP_FAILED) {
                std::cerr << "mmap failed for plane " << planeIndex << "\n";
                ++planeIndex;
                continue;
            }

            // Heuristic stride calculation:
            // If driver provided metadata.planes()[planeIndex].stride, use it.
            // Otherwise we fall back to plane.length / height.
            const FrameMetadata::Plane &metaPlane = metadata.planes()[planeIndex];
            int strideBytes = 0;
            if (metaPlane.length > 0 && metaPlane.bytesused > 0) {
                // best guess: bytes per line = bytesused / height (works if bytesused describes used area)
                strideBytes = metaPlane.bytesused / std::max(1, metadata.height ? metadata.height : 1);
            }
            if (strideBytes == 0) {
                // fallback: assume contiguous evenly split rows
                strideBytes = plane.length / std::max(1, metadata.height ? metadata.height : 1);
            }
            if (strideBytes == 0) {
                // last resort: assume width * 2
                strideBytes = metadata.width * 2;
            }

            int stridePixels = strideBytes / 2; // assume 2 bytes per pixel container
            std::cout << "plane " << planeIndex << " length=" << plane.length << " bytesused=" << metaPlane.bytesused
                      << " strideBytes=" << strideBytes << " stridePixels=" << stridePixels << "\n";

            // Treat memory as uint16_t array (common case where driver gives 16-bit containers).
            uint16_t *raw16 = reinterpret_cast<uint16_t *>(mem);

            // Prepare RGB buffer and debayer
            std::vector<uint8_t> rgb(metadata.width * metadata.height * 3);
            // data_shift assumes sensor puts 12-bit data MSB-aligned in 16-bit container (>>4 -> 8-bit)
            // If your sensor uses other packing, adjust this value (or implement packed 12-bit unpack).
            const int data_shift = 4;
            debayerRGGB_bilinear(raw16, stridePixels, metadata.width, metadata.height, rgb, data_shift);

            // Submit to saver worker
            {
                std::ostringstream oss;
                oss << "capture_" << std::setw(6) << std::setfill('0') << metadata.sequence << ".png";
                SaveTask task;
                task.filename = oss.str();
                task.rgb = std::move(rgb);
                task.width = metadata.width;
                task.height = metadata.height;
                g_saver.push(std::move(task));
            }

            // Also write raw plane to disk
            {
                std::ostringstream rn;
                rn << "frame_" << metadata.sequence << "_plane" << planeIndex << ".raw";
                std::ofstream f(rn.str(), std::ios::binary);
                if (f) f.write(reinterpret_cast<const char *>(mem), metaPlane.bytesused ? metaPlane.bytesused : plane.length);
            }

            munmap(mem, plane.length);
            ++planeIndex;
        }
    }

    // Reuse & requeue request for continuous capture. If you want only single-shot,
    // do not reuse/requeue.
    request->reuse(Request::ReuseBuffers);
    if (g_camera) g_camera->queueRequest(request);
}
