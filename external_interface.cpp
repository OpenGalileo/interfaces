#include "external_interface.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

StarTracker::StarTracker(){

}

StarTracker::~StarTracker(){

}

std::array<float,3>  StarTracker::get_imu_euler(){
    const char* device = "/dev/i2c-1";
    int file = open(device, O_RDWR);
    if (file < 0) {
        std::cerr << "Failed to open I2C device\n";
        return nullptr;
    }
    if (ioctl(file, I2C_SLAVE, 0x18) < 0) {
        std::cerr << "Failed to set I2C address\n";
        close(file);
        return nullptr;
    }
    std::array<float,3> imu_euler = {0.0f, 0.0f, 0.0f};;
    uint8_t buf[6];
    read(file,buf, 6);
    close(file);
    imu_euler[0] = (float)((buf[0] + buf[1] <<8)/16);
    imu_euler[1] = (float)((buf[2] + buf[3] <<8)/16);
    imu_euler[2] = (float)((buf[4] + buf[5] <<8)/16);
    return imu_euler;
}
