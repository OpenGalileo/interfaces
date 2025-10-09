#include "camera.h"

    
StarCamera::StarCamera(int exposure, int res_row, int res_col, int iso):
      _exposure(exposure), _res_row(res_row), _res_col(res_col), _iso(iso) {
        // Values set. Anything else?
      }
    //iso is light sensitivity, exposure is time, resolution is max image size we can use 
    
    //init camera, assign settings
void StarCamera::init() {
    cam_man.start(); // Initialize the camera from libcam
    if (cam_man.cameras().empty()) {
      std::cerr << "No cameras found\n";
    }

    camera = cam_man.cameras()[0];
    camera->acquire();

    config = camera->generateConfiguration({StreamRole::StillCapture});

    if (!config || config->empty()) {
      std::cerr << "Failed to get config\n";
    }

    config->at(0).pixelFormat = formats::RGB888;
    config->at(0).size = {MAX_WIDTH, MAX_HEIGHT};
    config->validate();
    camera->configure(config.get());

    allocator = std::make_unique<FrameBufferAllocator>(camera);

    stream = config->at(0).stream();

    if (allocator->allocate(stream) < 0) {
      std::cerr << "Buffer allocation failed\n";
    }

    const std::vector<std::unique_ptr<FrameBuffer>> &buffer = allocator->buffers(stream);

    fb = buffer[0].get();

    }

    //take picture, when SPI commands picture, this will be called to take image
    bool StarCamera::take_picture() {
      auto request = camera->createRequest();
      if (!request) {
        return false;
      }

      if (request->addBuffer(stream, fb) < 0) return false;
      if (camera->queueRequest(request) < 0) return false;

    }
    //see pictire, when SPI wants raw image data, this will export raw image data 
    void print_picture();
    //send picture