#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
// #include <stdio.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <errno.h>
// #include <sys/ioctl.h>
// #include <sys/mman.h>
// #include <string.h>
// #include <linux/videodev2.h>

#include <libcamera/libcamera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/request.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

class Camera { //camera will be high def and a global shuttter
   
    private:     
    #define MAX_WIDTH 4056 
    //max width for high resolution camera 
    #define MAX_HEIGHT 3040
    //max height for high resolution camera
    int _exposure; //time in ms or us that it would take for picture
    const static int _res_width; 
    const static int _res_height;
    int _iso;
  
    //picture settings
    void image_settings();
    
    uint16_t array[_res_height*_res_width]; //this will depend on camera
    //planned camera widthx height and pixel count: 4056 x 3040
    //planned 12.3 megapixels

    struct img_buffer {
      void *img_start;
      uint32_t length;
    };
    
    
    public: 
    //constructor
    Camera(int exposure, int res_width, int res_height, int iso);
    //iso is light sensitivity, exposure is time, resolution is max image size we can use 
    
    //init camera, assign settings
    void init();
    //take picture, when SPI commands picture, this will be called to take image
    void take_picture();
    //see pictire, when SPI wants raw image data, this will export raw image data 
    void print_picture();
    //send picture
};
#endif
