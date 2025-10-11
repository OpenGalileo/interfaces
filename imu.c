/*
Updated by Sarah - 10/11/2025
*/
#include "imu.h"

uint8_t DEV_ADDR = BNO088_ADDR<<1; 
I2C_HandleTypeDef* hi2c; 


/*
Version for using default addresss
*/
int imu_init(OperModes op_mode, PowerModes pwr_mode, I2C_HandleTypeDef *handle){
    hi2c = handle; 
    uint8_t buf[10] = {0};
    uint8_t read_addr = 0x00;
    HAL_I2C_Master_Transmit(hi2c, DEV_ADDR, &read_addr, 1, 1000 );
    HAL_I2C_Master_Receive(hi2c, DEV_ADDR, &buf[0], 1, 1000 );
    if(buf[0]!=0xA0){
        printf("Could not establish communication with device\n\r"); 
        return -1; 
    }

    //Configure power mode 
	HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_PWR_MODE_ADDR, 1, &pwr_mode, 1, 1000);

    //Configure operating mode
    HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_OPR_MODE_ADDR, 1, &op_mode, 1, 1000);

    return 0; 
}

/*
Version for using different address
*/
int imu_init(OperModes op_mode, PowerModes pwr_mode, I2C_HandleTypeDef *handle, uint8_t alt_addr){
    DEV_ADDR = alt_addr<<1; 
    hi2c = handle; 

    uint8_t buf[10] = {0};
    uint8_t read_addr = 0x00;
    HAL_I2C_Master_Transmit(hi2c, DEV_ADDR, &read_addr, 1, 1000 );
    HAL_I2C_Master_Receive(hi2c, DEV_ADDR, &buf[0], 1, 1000 );
    if(buf[0]!=0xA0){
        printf("Could not establish communication with device\n\r"); 
        return -1; 
    }

    //Configure power mode 
	HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_PWR_MODE_ADDR, 1, &pwr_mode, 1, 1000);

    //Configure operating mode
    HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_OPR_MODE_ADDR, 1, &op_mode, 1, 1000);

    return 0; 
}

uint8_t readTemperature(){
    uint8_t buf[10] = {0}; 
    HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_TEMP_ADDR, 1, &buf[0], 1, 1000);
    return buf[0]; 
}


void readQuaternion(float* quaternion){
    uint8_t buf[8] = {0};
    HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_QUATERNION_DATA_W_LSB_ADDR, 1, &buf[0], 8, 1000);

    float scale = 1.0f / 16384.0f;

    quaternion[0] = (int16_t)(buf[0]|(buf[1]<<8))*scale; 
    quaternion[1] = (int16_t)(buf[2]|(buf[3]<<8))*scale; 
    quaternion[2] = (int16_t)(buf[4]|(buf[5]<<8))*scale; 
    quaternion[3] = (int16_t)(buf[6]|(buf[7]<<8))*scale; 
}

void readEuler(float* euler){
    uint8_t buf[6] = {0};
    HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_EULER_H_LSB_ADDR, 1, &buf[0], 6, 1000);

    float scale = 1.0f / 16.0f;

    euler[0] = (int16_t)(buf[0]|(buf[1]<<8))*scale; 
    euler[1] = (int16_t)(buf[2]|(buf[3]<<8))*scale; 
    euler[2] = (int16_t)(buf[4]|(buf[5]<<8))*scale; 
}

void readAccelerometer(float* accel){
    uint8_t buf[6] = {0};
    HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_ACCEL_DATA_X_MSB_ADDR, 1, &buf[0], 6, 1000);

    float scale = 1.0f / 100.0f;

    accel[0] = (int16_t)(buf[0]|(buf[1]<<8))*scale; 
    accel[1] = (int16_t)(buf[2]|(buf[3]<<8))*scale; 
    accel[2] = (int16_t)(buf[4]|(buf[5]<<8))*scale; 
}

void readMagnetometer(float* mag){
    uint8_t buf[6] = {0};
    HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_MAG_DATA_X_LSB_ADDR, 1, &buf[0], 6, 1000);

    float scale = 1.0f / 16.0f;

    mag[0] = (int16_t)(buf[0]|(buf[1]<<8))*scale; 
    mag[1] = (int16_t)(buf[2]|(buf[3]<<8))*scale; 
    mag[2] = (int16_t)(buf[4]|(buf[5]<<8))*scale; 
}

void readGyroscope(float* gyro){
    uint8_t buf[6] = {0};
    HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_GYRO_DATA_X_LSB_ADDR, 1, &buf[0], 6, 1000);

    float scale = 1.0f / 16.0f;

    gyro[0] = (int16_t)(buf[0]|(buf[1]<<8))*scale; 
    gyro[1] = (int16_t)(buf[2]|(buf[3]<<8))*scale; 
    gyro[2] = (int16_t)(buf[4]|(buf[5]<<8))*scale; 
}

//just setting power mode to suspend
void sleep(){
    HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_PWR_MODE_ADDR, 1, POWER_MODE_SUSPEND, 1, 1000);
}

//just setting power mode to normal
void sleep(){
    HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_PWR_MODE_ADDR, 1, POWER_MODE_NORMAL, 1, 1000);
}