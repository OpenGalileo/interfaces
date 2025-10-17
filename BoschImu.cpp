/*
Updated by Sarah - 10/11/2025
*/
#include "imu.hpp"
#include "stdio.h"

uint8_t DEV_ADDR = BNO088_ADDR<<1;
//I2C_HandleTypeDef* hi2c;

Bosch_Imu::Bosch_Imu(I2C_HandleTypeDef *handle) {
    hi2c = handle;
    DEV_ADDR = 0x28 << 1;
}

Bosch_Imu::Bosch_Imu(I2C_HandleTypeDef *handle, uint8_t addr) {
    hi2c = handle;
    DEV_ADDR = addr << 1;
}

/**
 * Basic write function for writing to data registers on the IMU
 * --Arguments--
 * reg - register to write to
 * dataBuf - pointer to buffer of data to send
 */
void Bosch_Imu::raw_write(uint8_t reg, uint8_t* dataBuf) {
    // Send the command
    HAL_I2C_Mem_Write(hi2c, DEV_ADDR, reg, 1, dataBuf, sizeof(*dataBuf), 1000);
    // Add a 10 ms delay to prevent overrunning the IMU
    HAL_Delay(10);
}


/**
 * Basic function for reading register data from the IMU
 * --Arguments--
 * reg - register to read from
 * dataBuf - pointer to the first index of the data buffer to store to
 * regLength - Number of bytes to read
 */
void Bosch_Imu::raw_read(uint8_t reg, uint8_t* dataBuf, uint8_t regLength) {
    // Send the command and get the response back
    HAL_I2C_Mem_Read(hi2c, DEV_ADDR, reg, 1, dataBuf, regLength, 1000);
    // Add a 10 ms delay to prevent overrunning the IMU
    HAL_Delay(10);
}

/*
Init function for setting up IMU into a given mode
Version for using default addresss
*/
int Bosch_Imu::imu_init(OperModes op_mode, PowerModes pwr_mode){
    uint8_t buf[10] = {0};
    uint8_t read_addr = 0x00;
    // TODO: Same as the other on line 52
    // If swapping over is fine, use the line below
    // raw_read(read_addr, &buf[0], 1);
    HAL_I2C_Master_Transmit(hi2c, DEV_ADDR, &read_addr, 1, 1000 );
    HAL_I2C_Master_Receive(hi2c, DEV_ADDR, &buf[0], 1, 1000 );
    if(buf[0]!=0xA0){
        printf("Could not establish communication with device\n\r");
        return -1;
    }

    //Configure power mode
    uint8_t tempPwrMode = (uint8_t)pwr_mode;
    raw_write(BNO055_PWR_MODE_ADDR, &tempPwrMode);
	// HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_PWR_MODE_ADDR, 1, &pwr_mode, 1, 1000);

    // Add a bit of a delay for the system to start waking up
    HAL_Delay(500);

    //Configure operating mode
    uint8_t tempOpMode = (uint8_t)op_mode;
    raw_write(BNO055_OPR_MODE_ADDR, &tempOpMode);
    // HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_OPR_MODE_ADDR, 1, &op_mode, 1, 1000);

    // Another delay to ensure sensors are waking up
    HAL_Delay(1000);

    return 0;
}

/*
Init function for setting up IMU into a given mode
Version for using different address
*/
//int Bosch_Imu::imu_init(OperModes op_mode, PowerModes pwr_mode, uint8_t alt_addr){
//    DEV_ADDR = alt_addr<<1;
//
//    uint8_t buf[10] = {0};
//    uint8_t read_addr = 0x00;
//    // TODO: Left this as-is for now in case there's a specific reason.
//    // If not, just uncomment the raw_read line and delete the Master_Trasnmit / Master_Receive lines
//    // raw_read(read_addr, &buf[0], 1);
//    HAL_I2C_Master_Transmit(hi2c, DEV_ADDR, &read_addr, 1, 1000 );
//    HAL_I2C_Master_Receive(hi2c, DEV_ADDR, &buf[0], 1, 1000 );
//    if(buf[0]!=0xA0){
//        printf("Could not establish communication with device\n\r");
//        return -1;
//    }
//
//    //Configure power mode
//    uint8_t tempPwrMode = (uint8_t)pwr_mode;
//    raw_write(BNO055_PWR_MODE_ADDR, &tempPwrMode);
//	// HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_PWR_MODE_ADDR, 1, &pwr_mode, 1, 1000);
//
//    // Add a bit of a delay for the system to start waking up
//    HAL_Delay(50);
//
//    //Configure operating mode
//    uint8_t tempOpMode = (uint8_t)op_mode;
//    raw_write(BNO055_OPR_MODE_ADDR, &tempOpMode);
//    // HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_OPR_MODE_ADDR, 1, &op_mode, 1, 1000);
//
//    // Another delay to ensure sensors are waking up
//    HAL_Delay(1000);
//
//    return 0;
//}

/**
 * Reads internal CPU temperature of the sensor.
 * IMU must be in a running operational mode to report a non zero value
 */
uint8_t Bosch_Imu::readTemperature(){
    uint8_t buf[10] = {0};
    raw_read(BNO055_TEMP_ADDR, &buf[0], 1);
    // HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_TEMP_ADDR, 1, &buf[0], 1, 1000);
    return buf[0];
}


/**
 * Reads the calculated Quaternion calculated by the IMU.
 * IMU must be in a valid fusion operational mode that supports quaternion calculations
 */
void Bosch_Imu::readQuaternion(float* quaternion){
    uint8_t buf[8] = {0};
    raw_read(BNO055_QUATERNION_DATA_W_LSB_ADDR, &buf[0], 8);
    // HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_QUATERNION_DATA_W_LSB_ADDR, 1, &buf[0], 8, 1000);

    float scale = 1.0f / 16384.0f;

//    quaternion[0] = (int16_t)(buf[0]|(buf[1]<<8))*scale;
//    quaternion[1] = (int16_t)(buf[2]|(buf[3]<<8))*scale;
//    quaternion[2] = (int16_t)(buf[4]|(buf[5]<<8))*scale;
//    quaternion[3] = (int16_t)(buf[6]|(buf[7]<<8))*scale;

    int16_t temp = buf[0] | ((int16_t)buf[1]<<8);
    quaternion[0] = (float)temp * scale;

    temp = buf[2] | ((int16_t)buf[3]<<8);
    quaternion[1] = (float)temp * scale;

    temp = buf[4] | ((int16_t)buf[5]<<8);
    quaternion[2] = (float)temp * scale;

    temp = buf[6] | ((int16_t)buf[7]<<8);
    quaternion[3] = (float)temp * scale;
}

/**
 * Reads the Euler angles calculated by the IMU.
 * IMU must be in a valid fusion operation mode to generate non zero values.
 */
void Bosch_Imu::readEuler(float* euler){
    uint8_t buf[6] = {0};
    raw_read(BNO055_EULER_H_LSB_ADDR, &buf[0], 6);
    // HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_EULER_H_LSB_ADDR, 1, &buf[0], 6, 1000);

    float scale = 1.0f / 16.0f;

//    euler[0] = (int16_t)(buf[0]|(buf[1]<<8))*scale;
//    euler[1] = (int16_t)(buf[2]|(buf[3]<<8))*scale;
//    euler[2] = (int16_t)(buf[4]|(buf[5]<<8))*scale;

    int16_t temp = buf[0] | (int16_t)(buf[1]<<8);
    *euler = 1.3;
    euler[0] = (float)temp * scale;

    temp = buf[2] | ((int16_t)buf[3]<<8);
    float temp_f = float(temp);
    euler[1] = temp_f * scale;

    temp = buf[4] | ((int16_t)buf[5]<<8);
    euler[2] = (float)temp * scale;
}

/**
 * Reads the accelerometer from the IMU.
 * IMU must be in an operational mode that activates the accelerometer to return non zero values.
 */
void Bosch_Imu::readAccelerometer(float* accel){
    uint8_t buf[6] = {0};
    raw_read(BNO055_ACCEL_DATA_X_LSB_ADDR, &buf[0], 6);
    // HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_ACCEL_DATA_X_MSB_ADDR, 1, &buf[0], 6, 1000);

    float scale = 1.0f / 100.0f;

//    accel[0] = (int16_t)(buf[0]|(buf[1]<<8))*scale;
//    accel[1] = (int16_t)(buf[2]|(buf[3]<<8))*scale;
//    accel[2] = (int16_t)(buf[4]|(buf[5]<<8))*scale;


    int16_t temp = buf[0] | (int16_t)(buf[1]<<8);
    accel[0] = (float)temp * scale;

    temp = buf[2] | ((int16_t)buf[3]<<8);
    accel[1] = (float)temp * scale;

    temp = buf[4] | ((int16_t)buf[5]<<8);
    accel[2] = (float)temp * scale;

}

/**
 * Reads the magnatometer from the IMU.
 * IMU must be in an operational mode that activates the magnatometer to return non zero values.
 */
void Bosch_Imu::readMagnetometer(float* mag){
    uint8_t buf[6] = {0};
    raw_read(BNO055_MAG_DATA_X_LSB_ADDR, &buf[0], 6);
    // HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_MAG_DATA_X_LSB_ADDR, 1, &buf[0], 6, 1000);

    float scale = 1.0f / 16.0f;

//    mag[0] = (int16_t)(buf[0]|(buf[1]<<8))*scale;
//    mag[1] = (int16_t)(buf[2]|(buf[3]<<8))*scale;
//    mag[2] = (int16_t)(buf[4]|(buf[5]<<8))*scale;

    int16_t temp = buf[0] | (int16_t)(buf[1]<<8);
    mag[0] = (float)temp * scale;

    temp = buf[2] | ((int16_t)buf[3]<<8);
    mag[1] = (float)temp * scale;

    temp = buf[4] | ((int16_t)buf[5]<<8);
    mag[2] = (float)temp * scale;
}


/**
 * Reads the gyroscope from the IMU.
 * IMU must be in an operational mode that activates the gyroscope to return non zero values.
 */
void Bosch_Imu::readGyroscope(float* gyro){
    uint8_t buf[6] = {0};
    raw_read(BNO055_GYRO_DATA_X_LSB_ADDR, &buf[0], 6);
    // HAL_I2C_Mem_Read(hi2c, DEV_ADDR,  BNO055_GYRO_DATA_X_LSB_ADDR, 1, &buf[0], 6, 1000);

    float scale = 1.0f / 16.0f;

    int16_t temp = buf[0] | (int16_t)(buf[1]<<8);
    gyro[0] = (float)temp * scale;

    temp = buf[2] | ((int16_t)buf[3]<<8);
    gyro[1] = (float)temp * scale;

    temp = buf[4] | ((int16_t)buf[5]<<8);
    gyro[2] = (float)temp * scale;
}


/**
 * Sends the command to the IMU to put it into a suspend state.
 */
void Bosch_Imu::sleep(){
	uint8_t tempPwrMode = (uint8_t)POWER_MODE_SUSPEND;
    raw_write(BNO055_PWR_MODE_ADDR, &tempPwrMode);
    // HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_PWR_MODE_ADDR, 1, &POWER_MODE_SUSPEND, 1, 1000);
}


/**
 * Wakes the IMU up and puts it into a given operational mode.
 * If no mode is given, IMU will be put into the 9DOF fusion operation mode.
 */
void Bosch_Imu::wakeup(OperModes op_mode = OPERATION_MODE_NDOF){

    // Set the power mode to normal
	uint8_t tempPowerMode = POWER_MODE_NORMAL;
    raw_write(BNO055_PWR_MODE_ADDR, &tempPowerMode);
	// HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_PWR_MODE_ADDR, 1, &POWER_MODE_NORMAL, 1, 1000);

    // Wait a few ms for system to update
    HAL_Delay(50);

    // Configure operating mode
    uint8_t tempOpMode = (uint8_t)op_mode;
    raw_write(BNO055_OPR_MODE_ADDR, &tempOpMode);
    // HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_OPR_MODE_ADDR, 1, &op_mode, 1, 1000);

    // Wait a few more ms for the sensors to startup
    HAL_Delay(50);
}

/**
 * Performs a soft reset on the IMU.
 * After reset, IMU will be put into passed in operation mode.
 * If no mode is passed in, IMU will be put into 9DOF mode.
 */
void Bosch_Imu::reset(OperModes op_mode = OPERATION_MODE_NDOF) {
    // Send the command to reset the IMU
	uint8_t rebootReg = 0x20;
    raw_write(BNO055_SYS_TRIGGER_ADDR, &rebootReg);
    // HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_SYS_TRIGGER_ADDR, 1, &0x20, 1, 1000);
    // MAssive delay for full reboot
    HAL_Delay(1000);
    // Send the command for setting the power mode to normal
    // NOTE: Might not be needed, but we don't want to risk it going into LOW_POWER
    uint8_t tempPowerMode = (uint8_t)POWER_MODE_NORMAL;
    raw_write(BNO055_PWR_MODE_ADDR, &tempPowerMode);
    // HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_PWR_MODE_ADDR, 1, &POWER_MODE_NORMAL, 1, 1000);
    // Delay for the power mode change
    HAL_Delay(100);
    // Write the operational mode
    uint8_t tempOpMode = (uint8_t)op_mode;
    raw_write(BNO055_OPR_MODE_ADDR, &tempOpMode);
    // HAL_I2C_Mem_Write(hi2c, DEV_ADDR, BNO055_OPR_MODE_ADDR, 1, &op_mode, 1, 1000);
    // Delay for the sensors to start up
    HAL_Delay(100);
}
