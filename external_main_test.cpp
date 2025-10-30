#include "external_interface.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int main(){
    StarTracker star; 
    while(1){
        std::array<float,3> imu_euler = star.get_imu_euler();
        std::cout<<imu_euler[0]<<" "<<imu_euler[1]<<" "<<imu_euler[2]<<std::endl; 
        sleep(3);
    }
}