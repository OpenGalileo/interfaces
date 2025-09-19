#ifndef INTERFACE_H
#define INTERFACE_H

class StarTracker{
public: 
  StarTracker(); //constructor to initialize startracker

  struct attitude_data{ //Need to figure out what this will actually look like
    int attitude; 
  };

  /*
  Sets mode to either IMU only (0), star tracker only (1), or combined data (2)
  Default is 2, combined data
  */
  void set_mode(uint8_t mode); 

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

  /*
  Allows the satellite to access raw camera data if desired
  */
  uint8_t *get_raw_image(); 

private:
  uint8_t mode = 2; 
  int frequency; 
  int imu_freq; 
  int camera_freq;
}

#endif
