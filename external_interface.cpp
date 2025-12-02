/*
TODO: Decide if we want to only open and close the file once
TODO: Fix imu functions to read floats
*/


#include "external_interface.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>

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

std::array<float,4>  StarTracker::get_imu_quaternion(){
    uint8_t buf[8];
    bool ret = StarTracker::read_i2c(buf, 8, 0x04);
    if(!ret){
        std::cerr << "Failed quaternion read\n";
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }
    std::array<float,4> imu_quaternion = {0.0f, 0.0f, 0.0f, 0.0f};
    int16_t temp = buf[0] | (int16_t)buf[1]<<8;
    imu_quaternion[0] = (float)(temp/16384.0f);
    temp = buf[2] | (int16_t)buf[3]<<8;
    imu_quaternion[1] = (float)(temp/16384.0f);
    temp = buf[4] | (int16_t)buf[5]<<8;
    imu_quaternion[2] = (float)(temp/16384.0f);
    temp = buf[6] | (int16_t)buf[7]<<8;
    imu_quaternion[3] = (float)(temp/16384.0f);
    return imu_quaternion;
}

std::array<float,3>  StarTracker::get_imu_gyro(){
    uint8_t buf[6];
    bool ret = StarTracker::read_i2c(buf, 6, 0x1E);
    if(!ret){
        std::cerr << "Failed gyro read\n";
        return {0.0f, 0.0f, 0.0f};
    }
    std::array<float,3> imu_gyro = {0.0f, 0.0f, 0.0f};
    int16_t temp = buf[0] | (int16_t)buf[1]<<8;
    imu_gyro[0] = (float)(temp/16.0f);
    temp = buf[2] | (int16_t)buf[3]<<8;
    imu_gyro[1] = (float)(temp/16.0f);
    temp = buf[4] | (int16_t)buf[5]<<8;
    imu_gyro[2] = (float)(temp/16.0f);
    return imu_gyro;
}

std::array<float,3>  StarTracker::get_imu_mag(){
    uint8_t buf[6];
    bool ret = StarTracker::read_i2c(buf, 6, 0x18);
    if(!ret){
        std::cerr << "Failed mag read\n";
        return {0.0f, 0.0f, 0.0f};
    }
    std::array<float,3> imu_mag = {0.0f, 0.0f, 0.0f};
    int16_t temp = buf[0] | (int16_t)buf[1]<<8;
    imu_mag[0] = (float)(temp/16.0f);
    temp = buf[2] | (int16_t)buf[3]<<8;
    imu_mag[1] = (float)(temp/16.0f);
    temp = buf[4] | (int16_t)buf[5]<<8;
    imu_mag[2] = (float)(temp/16.0f);
    return imu_mag;
}

std::array<float,3>  StarTracker::get_imu_accel(){
    uint8_t buf[6];
    bool ret = StarTracker::read_i2c(buf, 6, 0x12);
    if(!ret){
        std::cerr << "Failed accel read\n";
        return {0.0f, 0.0f, 0.0f};
    }
    std::array<float,3> imu_accel = {0.0f, 0.0f, 0.0f};
    int16_t temp = buf[0] | (int16_t)buf[1]<<8;
    imu_accel[0] = (float)(temp/100.0f);
    temp = buf[2] | (int16_t)buf[3]<<8;
    imu_accel[1] = (float)(temp/100.0f);
    temp = buf[4] | (int16_t)buf[5]<<8;
    imu_accel[2] = (float)(temp/100.0f);
    return imu_accel;
}


std::array<float,16>  StarTracker::get_imu_all(){
    std::array<float,16> imu_all = {0.0f};
    auto q = StarTracker::get_imu_quaternion();
    std::copy(q.begin(), q.end(), imu_all.begin());
    auto e = StarTracker::get_imu_euler();
    std::copy(e.begin(), e.end(), imu_all.begin()+4);
    auto a = StarTracker::get_imu_accel();
    std::copy(a.begin(), a.end(), imu_all.begin()+7);
    auto m = StarTracker::get_imu_mag();
    std::copy(m.begin(), m.end(), imu_all.begin()+10);
    auto g = StarTracker::get_imu_gyro();
    std::copy(g.begin(), g.end(), imu_all.begin()+13);
    return imu_all;
}


float StarTracker::get_lost_ra(){
    uint8_t buf[2];
    bool ret = StarTracker::read_i2c(buf, 2, 0x30);
    if(!ret){
        std::cerr << "Failed lost ra read\n";
        return 0.0f;
    }
    float ra = ((float)(buf[0]|buf[1]<<8))/10000.0f; 
    return ra;
}


float StarTracker::get_lost_dec(){
    uint8_t buf[2];
    bool ret = StarTracker::read_i2c(buf, 2, 0x32);
    if(!ret){
        std::cerr << "Failed lost dec read\n";
        return 0.0f;
    }
    float dec = ((float)(buf[0]|buf[1]<<8))/10000.0f; 
    return dec;
}


float StarTracker::get_lost_roll(){
    uint8_t buf[2];
    bool ret = StarTracker::read_i2c(buf, 2, 0x34);
    if(!ret){
        std::cerr << "Failed lost roll read\n";
        return 0.0f;
    }
    float roll = ((float)(buf[0]|buf[1]<<8))/10000.0f; 
    return roll;
}


std::array<float,3>  StarTracker::get_lost_all(){
    std::array<float,3> lost_all = {0.0f};
    lost_all[0] = StarTracker::get_lost_ra();
    lost_all[1] = StarTracker::get_lost_dec();
    lost_all[2] = StarTracker::get_lost_roll();
    return lost_all; 
}

std::array<float, 3> StarTracker::get_lost_all_test(){
    uint8_t buf[13];

    bool ret = StarTracker::read_i2c(buf, sizeof(buf), 0x30);
    if (!ret) {
        std::cerr << "Failed lost roll read\n";
        return {};
    }

    std::array<float,3> lost_all = {0.0f, 0.0f, 0.0f};
    memcpy(&lost_all[0], &buf[1], sizeof(float));   // copy raw bytes into float
    memcpy(&lost_all[1], &buf[5], sizeof(float));  
    memcpy(&lost_all[2], &buf[9], sizeof(float));  

    return lost_all;
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
    int file = open(device, O_WRONLY); //may need to change this to rdwr
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


