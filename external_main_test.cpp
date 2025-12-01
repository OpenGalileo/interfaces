
#include "external_interface.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int main(){
    StarTracker star; 
    uint8_t mode = PI_ONLY;
    star.set_mode(mode);
    star.set_mode(mode);
    //star.set_mode(mode);  
    while(1){
        std::array<float,3> lost = star.get_lost_all_test();
        std::cout<<lost[0]<<" "<<lost[1]<<" "<<lost[2]<<std::endl; 
        sleep(5);
    }
}
