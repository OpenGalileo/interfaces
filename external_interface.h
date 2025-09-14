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
  */
  attitude_data single_data_point();

  /*
  Starts stream of current mode at specified freuquency 
  */
  void enable_stream(int frequency);

  attitude_data read_stream(); 
  void stop_stream();

private:
  uint8_t mode = 2; 
}

#endif
