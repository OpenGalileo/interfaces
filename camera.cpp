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
  }

    //take picture, when SPI commands picture, this will be called to take image
    void take_picture();
    //see pictire, when SPI wants raw image data, this will export raw image data 
    void print_picture();
    //send picture