#include <iostream> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int main(){
	const char* device = "/dev/i2c-1";
	int addr = 0x30; 
	uint8_t reg = 0x00; 

	int file = open(device, O_RDWR);
	if(file<0){
		std::cerr<<"failed to connect\n";
		close(file); 
		return 1; 
	}

	if(write(file, &reg, 1) != 1){
	std::cerr<< "failed to write to reg\n"; 
	close(file); 
	return 1; 
	}

	uint8_t buf[4]; 
	read(file,buf,4); 
	close (file);
	std::cout<<"recieved\n";
	for(int i = 0; i < 4; ++i){
	std::cout<<static_cast<int>(buf[i]); 
	}
	return 0; 
}
