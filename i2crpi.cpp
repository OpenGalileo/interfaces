#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int main(){
const char* device = "/dev/i2c-1";
int addr = 0x18;
uint8_t reg = 0x0C;

int file = open(device, O_RDWR);
ioctl(file, I2C_SLAVE, 0x18);
if(file<0){
std::cerr<<"failed to connect\n";
close(file);
return 1;
}

//if(write(file, &reg, 1) != 1){
//std::cerr<< "failed to write to reg\n";
//close(file);
//return 1;
//}
while(1){
write(file, &reg, 1);

uint8_t buf[6];
buf[0] = 0;
buf[1] = 0;
buf[2] = 0;
buf[3] = 0;  
read(file,buf, 6);
//close (file);
std::cout<<"recieved\n";
for(int i = 0; i < 6; ++i){
std::cout<<static_cast<int>(buf[i]);
}
//return 0;
sleep(3);
//for(volatile int j = 0; j<400000000; j++){buf[0] = j;}
}
return 0;
}