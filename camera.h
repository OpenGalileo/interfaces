#ifndef CAMERA_H
#define CAMERA_H

class Camera(){ //camera will be high def and a global shuttter
   
    private:     
    #define MAX_ROW 4056 
    //max row for high resolution camera 
    #define MAX_COL 3040
    //max col for high resolution camera
    int _exposure; //time in ms or us that it would take for picture
    int _res_row; 
    int _res_col;
    int _iso;
  
    //picture settings
    void image_settings();
    
    uint16_t array[res_col*res_row]; //this will depend on camera
    //planned camera widthx height and pixel count: 4056 x 3040
    //planned 12.3 megapixels
    
    public: 
    //constructor
    Camera(exposure, res_row, res_col, iso):
      _exposure(exposure), _res_row(res_row), _res_col(res_col), _iso(iso);
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
