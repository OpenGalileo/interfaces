/*
This external interface code was written to be a driver for our star tracker
It can be used to configure and collect data for our device with settings for the IMU and the star tracker itself
We used this code throughout our testing ot ensure our full data pipeline could work from these simple commands
We also used this code to back the GUI we made for our demonstration
*/


#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>
#include <array>
#include <stddef.h>

typedef enum : uint8_t{
    IDLE_PI_OFF = 0x00, //Lowest power usage, no data is being collected, default mode on initilization
    IDLE_PI_ON = 0x01, //RPI is on but no data is being collected, used mostly for debugging
    IMU_MEASURE = 0x02, //RPI is off and imu is actively reporting measurements
    LOST_MEASURE = 0x03, //RPI on and collecting startracker data, IMU off 
    FUSION_MEASURE = 0x04, //Both RPI and IMU are on and collecting data 
    LOST_MEASURE_MOCK = 0x05 //Used for testing
} OP_MODE;

class StarTracker{
public: 

/*
Initialize star tracker
*/
StarTracker(); 

/*
Sets startracker operating mode:
    IDLE_PI_OFF = 0x00
    IDLE_PI_ON = 0x01
    IMU_MEASURE = 0x02
    LOST_MEASURE = 0x03
    FUSION_MEASURE = 0x04
    LOST_MEASURE_MOCK = 0x05
*/
void set_mode(uint8_t mode); 

/*
Function used to do all i2c reads from star tracker mcu to rpi flight computer
*/
bool read_i2c(uint8_t *buf, std::size_t size, uint8_t addr);

/*
Function used to do all i2c writes from rpi flight computer to startracker mcu
*/
bool write_i2c(uint8_t *buf, std::size_t size, uint8_t addr);

/*
Sets IMU update frequency
*/
void imu_frequency(float freq);

/*
Sets LOST update frequency
*/
void lost_frequency(float freq);

/*
Used for GUI testing to select a specific test image that has been previously captured
*/
void select_image(uint8_t image_num);

/*
Sends command to the star tracker mcu to read the specified imu data and return it
*/
uint8_t get_imu_temp();
std::array<float,3> get_imu_euler();
std::array<float,4> get_imu_quaternion(); //returns w,x,y,z
std::array<float,3> get_imu_accel();
std::array<float,3> get_imu_gyro();
std::array<float,3> get_imu_mag();
std::array<float,16> get_imu_all(); //Returns 4 quaterion, 3 euler, 3 accel, 3 mag, 3 gyro

 /*
 Functions to read data from the star tracking algorithm running on the RPi
 */
  uint8_t get_lost_valid();
  float get_lost_ra(); 
  float get_lost_dec(); 
  float get_lost_roll();
  float get_lost_i(); 
  float get_lost_j(); 
  float get_lost_k();
  float get_lost_real(); 
  std::array<float,7> get_lost_all(); //returns ra, dec, roll, i, j, k, real

};
#endif
