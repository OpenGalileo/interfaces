/*
TODO: Decide if we want to only open and close the file once
*/


#include "external_interface.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

StarTracker::StarTracker(){

}

//StarTracker::~StarTracker(){

//}
void StarTracker::set_mode(uint8_t mode){
    uint8_t buf[2]= {0x00, mode};
    //buf[1] = 0x00;
    bool ret = StarTracker::write_i2c(buf, 2, 0x00);
    //buf[1] = mode;
    //bool ret2 = StarTracker::write_i2c(buf, 1, 0x00);
    if(!ret){
        std::cerr << "Failed mode write\n";
        std::cerr << "Failed " << unsigned(mode) << "\n";
    }
    if(ret){
        std::cerr << "Set mode " << unsigned(mode) << "\n";
    }
}

std::array<float,3>  StarTracker::get_imu_euler(){
    uint8_t buf[6];
    bool ret = StarTracker::read_i2c(buf, 6, 0x0C);
    if(!ret){
        std::cerr << "Failed euler read\n";
        return {0.0f, 0.0f, 0.0f};
    }
    std::array<float,3> imu_euler = {0.0f, 0.0f, 0.0f};
        int16_t temp = buf[0] | (int16_t)buf[1]<<8;
    imu_euler[0] = (float)(temp/16.0f);
        temp = buf[2] | (int16_t)buf[3]<<8;
    imu_euler[1] = (float)(temp/16.0f);
        temp = buf[4] | (int16_t)buf[5]<<8;
    imu_euler[2] = (float)(temp/16.0f);
    return imu_euler;
}
bool StarTracker::read_i2c(uint8_t *buf, size_t size, uint8_t addr){
    const char* device = "/dev/i2c-1";
    int file = open(device,O_RDWR);
    if (file < 0) {
        std::cerr << "Failed to open I2C device\n";
        return false;
    }
    if (ioctl(file, I2C_SLAVE, 0x18) < 0) {
        std::cerr << "Failed to set I2C address\n";
        close(file);
        return false;
    }
    uint8_t buf_temp[1] = {addr};
    ssize_t reg_write = write(file, buf_temp,  1);
    std::cerr << "buf_temp = " << unsigned(buf_temp[0]) << "\n";
    if(reg_write <0){
        close(file);
        return false;
    }
    ssize_t byteRead = read(file,buf, size);
    if(byteRead < 0){
        close(file);
        return false;
    }
    close(file);
    return true;
}

bool StarTracker::write_i2c(uint8_t *buf, size_t size, uint8_t addr){
    const char* device = "/dev/i2c-1";
    int file = open(device, O_WRONLY);
    if (file < 0) {
        std::cerr << "Failed to open I2C device\n";
        return false;
    }
    if (ioctl(file, I2C_SLAVE, 0x18) < 0) {
        std::cerr << "Failed to set I2C address\n";
        close(file);
        return false;
    }
    //uint8_t buf_temp[2] = {addr, buf[0]};
   // ssize_t reg_write= write(file, buf_temp, 1);
   // std::cerr << "buf_temp = " << unsigned(buf_temp[0]) << "\n";
   // if(reg_write<0){
//      close(file);
//      return false;
 //   }
    ssize_t addrWrite = write(file, buf, 1);
    ssize_t byteWrite = write(file,&buf[1], size-1);
    if(byteWrite < 0 || addrWrite<0){
        close(file);
        return false;
    }
    close(file);
    return true;

}


