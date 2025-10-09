#include "camera.h"

    
StarCamera::StarCamera(int exposure, int res_row, int res_col, int iso, int shutterTme, std::string outputFile):
      _exposure(exposure), _res_width(res_row), _res_height(res_col), _iso(iso), _shutterTime(shutterTme), _outputFile(outputFile) {
        // Values set. Anything else?
      }
    //iso is light sensitivity, exposure is time, resolution is max image size we can use 
    
    //init camera, assign settings
    void StarCamera::init() {
      

    }

    //take picture, when SPI commands picture, this will be called to take image
    bool StarCamera::take_picture() {
      std::ostringstream cmd;
      cmd << "libcamera-still "
          << "-o " << _outputFile
          << "--shutter " << _shutterTime;

          int ret = std::system(cmd.str().c_str());
          return ret == 0;
    }
    //see pictire, when SPI wants raw image data, this will export raw image data 
    void StarCamera::print_picture() {

    }
    //send picture