


#include "external_interface.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>

StarTracker::StarTracker(){

}

void StarTracker::set_mode(uint8_t mode){
    uint8_t buf[2]= {0x00, mode};
    bool ret = StarTracker::write_i2c(buf, 2, 0x00);
    if(!ret){
        std::cerr << "Failed mode write\n";
        std::cerr << "Failed " << unsigned(mode) << "\n";
    }
    if(ret){
        std::cerr << "Set mode " << unsigned(mode) << "\n";
    }
}

void StarTracker::imu_frequency(float freq){ //frequency should be between 0.154 and 100 Hz
    uint8_t buf[5] = {0x50, 0, 0, 0, 0};
    std::memcpy(&buf[1], &freq, sizeof(float));
    bool ret = StarTracker::write_i2c(buf, 5, 0x50);
    if(!ret){
        std::cerr << "Failed imu freq write\n";
        std::cerr << "Failed " << freq << "\n";
    }
    if(ret){
        std::cerr << "Set imu freq " << freq << "\n";
    }
}

void StarTracker::lost_frequency(float freq){ //frequency should be between 0 and 0.5 Hz
    uint8_t buf[5] = {0x54, 0, 0, 0, 0};
    std::memcpy(&buf[1], &freq, sizeof(float));
    bool ret = StarTracker::write_i2c(buf, 5, 0x54);
    if(!ret){
        std::cerr << "Failed lost freq write\n";
        std::cerr << "Failed " << freq << "\n";
    }
    if(ret){
        std::cerr << "Set lost freq " << freq << "\n";
    }
} 


void StarTracker::select_image(uint8_t image_num){ //used for testing
    uint8_t buf[2] = {0x02, image_num};
    bool ret = StarTracker::write_i2c(buf, 2, 0x02);
    if(!ret){
        std::cerr << "Failed image select\n";
        std::cerr << "Failed " << image_num << "\n";
    }
    if(ret){
        std::cerr << "Selected image " << image_num << "\n";
    }
} 


uint8_t StarTracker::get_imu_temp(){
    uint8_t buf[1];
    bool ret = StarTracker::read_i2c(buf, 1, 0x01);
    if(!ret){
        std::cerr << "Failed temp read\n";
        return 0;
    }
    return buf[0];
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

float StarTracker::get_imu_frequency(){
    uint8_t buf[4];
    bool ret = StarTracker::read_i2c(buf, 4, 0x50);
    if(!ret){
        std::cerr << "Failed imu freq read\n";
        return 0.0f;
    }
    float freq[1]; 
    memcpy(&freq[0], buf, 4);
    return freq[0];
}

uint8_t StarTracker::get_lost_valid(){
    uint8_t buf[1];
    bool ret = StarTracker::read_i2c(buf, 1, 0x30);
    if(!ret){
        std::cerr << "Failed lost valid read\n";
        return 0.0f;
    }
    return buf[0];
}

float StarTracker::get_lost_ra(){
    uint8_t buf[4];
    bool ret = StarTracker::read_i2c(buf, 4, 0x31);
    if(!ret){
        std::cerr << "Failed lost ra read\n";
        return 0.0f;
    }
    float ra[1]; 
    memcpy(&ra[0], buf, 4);
    return ra[0];
}


float StarTracker::get_lost_dec(){
    uint8_t buf[4];
    bool ret = StarTracker::read_i2c(buf, 4, 0x35);
    if(!ret){
        std::cerr << "Failed lost dec read\n";
        return 0.0f;
    }
    float dec[1]; 
    memcpy(&dec[0], buf, 4);
    return dec[0];
}


float StarTracker::get_lost_roll(){
    uint8_t buf[4];
    bool ret = StarTracker::read_i2c(buf, 4, 0x39);
    if(!ret){
        std::cerr << "Failed lost roll read\n";
        return 0.0f;
    }
    float roll[1]; 
    memcpy(&roll[0], buf, 4);
    return roll[0];
}

float StarTracker::get_lost_i(){
    uint8_t buf[4];
    bool ret = StarTracker::read_i2c(buf, 4, 0x3D);
    if(!ret){
        std::cerr << "Failed lost i read\n";
        return 0.0f;
    }
    float i[1]; 
    memcpy(&i[0], buf, 4);
    return i[0];
}

float StarTracker::get_lost_j(){
    uint8_t buf[4];
    bool ret = StarTracker::read_i2c(buf, 4, 0x41);
    if(!ret){
        std::cerr << "Failed lost j read\n";
        return 0.0f;
    }
    float j[1]; 
    memcpy(&j[0], buf, 4);
    return j[0];
}

float StarTracker::get_lost_k(){
    uint8_t buf[4];
    bool ret = StarTracker::read_i2c(buf, 4, 0x45);
    if(!ret){
        std::cerr << "Failed lost k read\n";
        return 0.0f;
    }
    float k[1]; 
    memcpy(&k[0], buf, 4);
    return k[0];
}

float StarTracker::get_lost_real(){
    uint8_t buf[4];
    bool ret = StarTracker::read_i2c(buf, 4, 0x49);
    if(!ret){
        std::cerr << "Failed lost real read\n";
        return 0.0f;
    }
    float real[1]; 
    memcpy(&real[0], buf, 4);
    return real[0];
}


std::array<float, 7> StarTracker::get_lost_all(){
    uint8_t buf[29];

    bool ret = StarTracker::read_i2c(buf, sizeof(buf), 0x30);
    if (!ret) {
        std::cerr << "Failed lost all read\n";
        return {};
    }
    if(buf[0] != 1){
        std::cerr << "Lost all read, data not valid\n";
        return {};
    }

    std::array<float,7> lost_all = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    memcpy(&lost_all[0], buf+1, 4);   // copy raw bytes into float
    memcpy(&lost_all[1], buf+5, 4);  
    memcpy(&lost_all[2], buf+9, 4);  
    memcpy(&lost_all[3], buf+13, 4);  
    memcpy(&lost_all[4], buf+17, 4);  
    memcpy(&lost_all[5], buf+21, 4);  
    memcpy(&lost_all[6], buf+25, 4);  

    return lost_all;
}

float StarTracker::get_lost_frequency(){
    uint8_t buf[4];
    bool ret = StarTracker::read_i2c(buf, 4, 0x54);
    if(!ret){
        std::cerr << "Failed lost freq read\n";
        return 0.0f;
    }
    float freq[1]; 
    memcpy(&freq[0], buf, 4);
    return freq[0];
}


bool StarTracker::read_i2c(uint8_t *buf, size_t size, uint8_t addr){
    const char* device = "/dev/i2c-1";
    int file = open(device,O_RDWR);
    if (file < 0) {
        std::cerr << "Failed to open I2C device\n";
        return false;
    }
    if (ioctl(file, I2C_SLAVE, 0x30) < 0) {
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
    if (ioctl(file, I2C_SLAVE, 0x30) < 0) {
        std::cerr << "Failed to set I2C address\n";
        close(file);
        return false;
    }
    ssize_t byteWrite = write(file, buf, size);
    if(byteWrite<0){
        close(file);
        return false;
    }
    close(file);
    return true;

}



