#ifndef CAMERA_H
#define CAMERA_H

class Camera(){
    private:     
    int _exposure;
    int _res_row;
    int _res_col;
    int _iso;
  
    //picture settings
    void image_settings();
    uint8_t array[res_col*res_row]; //this will depend on camera
  
    public: 
    //constructor
    Camera(exposure, res_row, res_col, iso):
      _exposure(exposure), _res_row(res_row), _res_col(res_col), _iso(iso);
    
    //init camera
    void init();
    //take picture'
    void take_picture();
    //see pictire
    void print_picture();
    //send picture
};
#endif
