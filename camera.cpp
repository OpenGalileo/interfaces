#include "camera.h"

    
StarCamera::StarCamera(int exposure, int iso, int res_row, int res_col, std::string outputFile):
      _exposure(exposure), _iso(iso), _res_width(res_row), _res_height(res_col), _outputFile(outputFile) {
        // Values set. Anything else?
      }
    //iso is light sensitivity, exposure is time, resolution is max image size we can use 
    
    //init camera, assign settings
    void StarCamera::init() {
      

    }

    //take picture, when SPI commands picture, this will be called to take image
    bool StarCamera::take_picture() {
      std::ostringstream cmd;
      cmd << "rpicam-still "
          << "-o " << _outputFile
          << " --shutter " << _exposure
          << " --immediate "
          << " --nopreview "
          << " --timeout " << _exposure + 100
          << " --gain " << _iso / 100
          << " --denoise off --awbgains 1,1";

          int ret = std::system(cmd.str().c_str());
          return ret == 0;
    }
    //see pictire, when SPI wants raw image data, this will export raw image data 
    void StarCamera::print_picture() {

    }

    void StarCamera::set_iso(int new_ISO) {
      _iso = new_ISO;
    }

    void StarCamera::set_exposure(int new_exposure) {
      _exposure = new_exposure;
    }
    //change image output name
    void StarCamera::set_outputFilename(std::string newFilename) {
      _outputFile = newFilename;
    }
    //send picture
