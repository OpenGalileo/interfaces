#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>
#include <array>
#include <stddef.h>

typedef enum : uint8_t{
    IDLE_PI_OFF, 
    IDLE_PI_ON,
    IMU_ONLY,
    PI_ONLY,
    FUSION
} OP_MODE;

class StarTracker{
public: 
  StarTracker(); //constructor to initialize startracker

  struct attitude_data{ //This is the information returned by the lost algorithm
    uint8_t attitude_known; //1 if lost found a match, 0 if not indicating the rest of the data is junk
    float attitude_ra; //Right ascension of the boresight vector
    float attitude_de; //declination of the boresight vector
    float attitude_roll; //roll about boresight access 
    float attitude_i; //quaternion i vector
    float attitude_j; //quaternion j vector
    float attitude_k; //quaternion k vector
    float attitude_real;//quaternion scaler
  };

  /*
  Sets mode to either IMU only (0), star tracker only (1), or combined data (2)
  Default is 2, combined data
  */
  void set_mode(uint8_t mode); 

  bool read_i2c(uint8_t *buf, std::size_t size, uint8_t addr);
  bool write_i2c(uint8_t *buf, std::size_t size, uint8_t addr);

  /*
  Acquires one packet from the current mode

  This is very low power and just takes one imu and or one camera measurement 
  */
  attitude_data single_data_point();

  /*
  Starts data collection at the specified rate in the current mode
  */
  void enable_stream(int frequency);

  /*
  More detailed enable stream function to be used only in mode 2 
  Allows the user to definie imu and camera frequency along with overall data frequency

  frequency --> Rate at which data is sent to the satellite
  imu_frequency --> Rate at which IMU data updates, must be >= frequency
  camera_frequency --> Rate at which images are taken and processed

  In our documentation we will create a table that relates these update frequencies to power consumption
  */
  void enable_stream(int frequency, int imu_frequency, int camera_frequency); 

  /*
  Reads a packet from the star tracker, should probably call this at a rate close to the frequency selected in enable stream
 */
  attitude_data read_stream(); 

  /*
  instructs the star tracker to stop taking measurements
  This is the sleep mode where no data is being measured but the devices is still on
  */
  void stop_stream();

 /*
  Tells the star tracker to not consume any power other than the bare minimum to maintain communications
  */
  void deep_sleep();


  //Would probably make more sense to return a data structure so the user can imu.gyro_x or somethign instead of indexing an array
  std::array<float,16> get_imu_all(); 
  std::array<float,3> get_imu_euler();
  std::array<float,4> get_imu_quaternion();
  std::array<float,3> get_imu_accel();
  std::array<float,3> get_imu_gyro();
  std::array<float,3> get_imu_mag();

  //LOST stuff
  float get_lost_ra(); 
  float get_lost_dec(); 
  float get_lost_roll();
  std::array<float,3> get_lost_all();
  std::array<float, 3> get_lost_all_test();


  /*
  Allows the satellite to access raw camera data if desired
  */
  uint8_t *get_raw_image(); 

  /*
  access to raw centroid data after image processing
  */
  uint8_t *get_centroids();

  /*
  access to raw star identificatino data
  */
  uint8_t *get_stars();


private:
  uint8_t mode = 2; 
  int frequency; 
  int imu_freq; 
  int camera_freq;
};

#endif


