#ifndef IMU_H
#define IMU_H

// IMU pseudocode
// IMU has accelerometer and gyroscope to read velocity, acceleration, and orientation

class IMU {

public:

    // Read velocity in m/s
    void readVelocity() {
        //  read data
        //  convert raw accel data to m/s^2
        //  calculate velocity
        //  return velocity
    }

    // Read acceleration in m/s^2
    void readAcceleration() {
        //  read regs
        //  combine MSB + LSB → signed int16 values
        //  convert raw LSB → m/s^2 using formula:
        //  accel_mps2 = raw_value * (range_in_g / 32768) * 9.81
        //  return accel values for X, Y, Z
    }

    void readXaxis() {
        //  read data regs
        //  convert raw data to deg/s
        //  calculate angle
        //  return angle
    }

    void readYaxis() {
        //  read gyro regs
        //  convert raw data to deg/s
        //  calculate angle
        //  return angle
    }

    void readZaxis() {
        //  read data regs
        //  convert raw data to deg/s
        //  calculate angle
    }

    // take full measurement of velocity, acceleration, and orientation
    void takeMeasurement() {
        //  trigger burst read
        //  read accel and gyro data
        //  convert raw data
        //  update velocity and orientation state
        //  store latest results internally
    }

    // dont draw current
    void sleepyTime() {

    }

    // wake to be idle/normal
    void WAKEUP() {

    }
};

#endif
