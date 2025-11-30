#include "camera.h"

    
StarCamera::StarCamera(camera_settings* settings):
      _exposure(settings->exposure), _gain(settings->gain), _res_width(settings->width), _res_height(settings->height), _outputFile(settings->filename) {
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
          << " --gain " << _gain
          << " --width " << _res_width
          << " --height " << _res_height;

          int ret = std::system(cmd.str().c_str());
          return ret == 0;
    }
    //see pictire, when SPI wants raw image data, this will export raw image data 
    void StarCamera::print_picture() {

    }

    void StarCamera::set_gain(int new_gain) {
      _gain = new_gain;
    }

    void StarCamera::set_exposure(int new_exposure) {
      _exposure = new_exposure;
    }
    //change image output name
    void StarCamera::set_outputFilename(std::string newFilename) {
      _outputFile = newFilename;
    }
    //send picture
