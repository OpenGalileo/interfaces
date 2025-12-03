#include "quat_math.h"
#include "filtering.h"
#include <stdio.h> // Include for potential debugging/prints

//function defs
/** ASSUMPTIONS: qoffset is a quat variable stored somewhere
 *               quat_fuse_imu is what outputs IMU data fusion mode
 *               updatefromlost updates the offset that is used in fusion
 *               quat_slerp averages data over time? not really sure if we care to have it
 * 
 * NOTE: lots of normalizing, might not need all of it
 */

quat fuse_imu(quat qimu){ //should take in most recent imu data and return fused imu data
    extern quat qoffset;
    qimu= quat_normalize(qimu); // ensure normalized
    quat qfused = quat_multiply(qoffset, qimu); //transform ref with quat multiplication
    return quat_normalize(qfused); //ensure output is normalized
}

void updateFromLost(quat qstar, quat qimu_last){ //should take in newest qstar data and last imu reading
    //this allows us to update the qoffset that corrects for the current startracker data difference from  current imu data
    extern quat qoffset;
    extern quat qstar_prev;

    qstar = quat_normalize(qstar); // ensure normalized
    qstar = quat_nearest_angle(qstar, qstar_prev);//make sire angles align
    qstar_prev = qstar; // replace most recent star

    quat qimu_inv = quat_conjugate(qimu_last); //report imubody-->absolute 
    //instead of imubody-->absolute

    quat qoffset_new = quat_multiply(qstar, qimu_inv);//create new offset with updated star data --> 
    //star body -->j2000 x absolute -->imu body (AB  = B-->A), absolute -->(imu body->star body) --> j2000
    qoffset_new = quat_normalize(qoffset_new); // ensure normalized 

    qoffset = qoffset_new; // can also optionally smooth
    //qoffset = slerp(qoffset, qoffset_new, 0.1); //this would replace direct assignment
}

quat slerp(quat q1, quat q2, double t){
    quat result;
    q1 = quat_normalize(q1);
    q2 = quat_normalize(q2); // ensure normal

    //q2 = quat_nearest_angle(q2, q1); // ensure no 180 degree flip

    double dot =  //part of slerp averaging, gets cos(angle)
        q1.real*q2.real + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z;

    //QUESTION if i fix q2 before dot product do i need this: chat says yes
    
     if (dot < 0.0) {
        dot = -dot;
        q2.real = -q2.real;
        q2.x = -q2.x;
        q2.y = -q2.y;
        q2.z = -q2.z;
    }
    
    if (dot > 1.0) dot = 1.0; // clamping dot to prevvent floating point errors
    if (dot < -1.0) dot = -1.0;

     if (dot > 0.9995) { // QUESTION if angle is small just average linearly? or can i just return the new one?
        result.real = q1.real + t * (q2.real - q1.real);
        result.x    = q1.x    + t * (q2.x    - q1.x);
        result.y    = q1.y    + t * (q2.y    - q1.y);
        result.z    = q1.z    + t * (q2.z    - q1.z);
        return quat_normalize(result);
    }

    double theta = acos(dot); // takes angle, using cos(theta) from dot
     
    if (fabs(theta) < 1e-6)
    {
        return q2;// use more recent? 
    }

    //compute weights to blend angles
    double sin_theta = sin(theta);
    double w0 = sin((1.0 - t) * theta) / sin_theta;
    double w1 = sin(t * theta) / sin_theta;

    result.real = w0 * q1.real + w1 * q2.real;
    result.x    = w0 * q1.x    + w1 * q2.x;
    result.y    = w0 * q1.y    + w1 * q2.y;
    result.z    = w0 * q1.z    + w1 * q2.z;

    return quat_normalize(result);
}










