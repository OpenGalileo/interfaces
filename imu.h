#ifndef IMU_H
#define IMU_H

// IMU pseudocode
// IMU has accelerometer and gyroscope to read velocity, acceleration, and orientation

class IMU {
private: 
   
public:
    void init(bool enable);
   //initialize any values we need upon startup, enable certain components if we want to choose, done with enable bool input.
    void CalibrateIMU();
    //call imu calibrate enable spin/any other setup the IMU needs

    void readQuaternion();
    //  read IMU output  --> this comes in the form of a 4 element vector, this is orientation for math purposes 

    void readEuler();
    //reads roll, pitch, and yaw

    // dont draw current
    void sleep();
    //dont pull imu to save power
    
    // wake to be idle/normal
    void wakeup();
    //start pulling imu
    
};

#endif
