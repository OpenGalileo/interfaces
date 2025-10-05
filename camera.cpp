#include "camera.h"

    
Camera::Camera(int exposure, int res_row, int res_col, int iso):
      _exposure(exposure), _res_row(res_row), _res_col(res_col), _iso(iso) {
        // Values set. Anything else?
      }
    //iso is light sensitivity, exposure is time, resolution is max image size we can use 
    
    //init camera, assign settings
void Camera::init() {
  int fd = open("/dev/video0", O_RDWR);
  if (fd < 0) {
    perror("Failed to open /dev/video0");

  
    memset(&img_fmt, 0, sizeof(img_fmt));
    img_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    img_fmt.fmt.pix.width = _res_width;
    img_fmt.fmt.pix.height = _res_width;
  }
}
    //take picture, when SPI commands picture, this will be called to take image
    void take_picture();
    //see pictire, when SPI wants raw image data, this will export raw image data 
    void print_picture();
    //send picture