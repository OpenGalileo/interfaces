#include "external_interface.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int main(){
    StarTracker star; 
    uint8_t mode = IDLE_PI_OFF; 
    while(1){
        star.set_mode(mode);
        if(mode >=4){
            mode = 0;
        }
        else{
            mode++;
        }
        std::array<float,3> imu_euler = star.get_imu_euler();
        std::cout<<imu_euler[0]<<" "<<imu_euler[1]<<" "<<imu_euler[2]<<std::endl; 
        sleep(3);
    }
}
