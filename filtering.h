#include "quat_math.h"

quat fuse_imu(quat qimu);
/** @brief should take in most recent imu data and return fused imu data
 ** @param //take in most recent imu data 
 *  @return constantly output fused/corrected data
 * 
 */

void updateFromLost(quat qstar, quat qimu);
/** @brief update qoffset with new startracker data
 * @param //most recent startracker data, recent imu 
 * @return no return but internally updates the stored offset and replaced prev_star  
 * 
 */

quat slerp(quat q1, quat q2, double t);
/** @brief this computes the average sloped updates of the IMU data
 *  @param //two quaternions to average, weight value (default should be 0.1)
 *  @return averaged quaternion
 * 
 */






