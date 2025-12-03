#include "quat_math.h"
#include <stdio.h> // Include for potential debugging/prints

// --- Function Definitions --- //
/**
 * @brief Multiplies two quaternions (qC = qA * qB).
 */
quat quat_multiply(const quat qA, const quat qB) { //check implementations
    quat qC;
    // NOTE!!!!! MULTI[LICATION IS NOT COMMUNICATIVE, QA*Qb DOES NOT EQAL QB*QA]
    // a        b     c     d          w        x     y     z
    // qa.real, qa.x, qa.y, qa.z       qb.real, qb.x, qb.y, qb.z

    // aw - bx - cy - dz
    qC.real = qA.real*qB.real - qA.x*qB.x - qA.y*qB.y - qA.z*qB.z;
    // ax + bw + cz - dy
    qC.x= qA.real*qB.x + qA.x*qB.real + qA.y*qB.z - qA.z*qB.y;
    // ay - bz + cw + dx
    qC.y= qA.real*qB.y - qA.x*qB.z + qA.y*qB.real + qA.z*qB.x;
    // az + by - cx + dw 
    qC.z= qA.real*qB.z + qA.x *qB.y - qA.y*qB.x + qA.z*qB.real;
   
    return qC;
}

/**
 * @brief Calculates the conjugate (inverse) of a quaternion.
 */
quat quat_conjugate(const quat q) {
    quat q_conj;

    // The conjugate only flips the sign of the vector components
    q_conj.real = q.real;
    q_conj.x = -q.x;
    q_conj.y = -q.y;
    q_conj.z = -q.z;

    return q_conj;
}

quat quat_nearest_angle(const quat qnew, const quat qprev){
    quat qresult;
    double dot = 
        qnew.real * qprev.real + 
        qnew.x * qprev.x +
        qnew.y * qprev.y +
        qnew.z * qprev.z;
    qresult = qnew;
    if(dot < 0.0){
        qresult.real = -qresult.real;
        qresult.x = -qresult.x;
        qresult.y = -qresult.y;
        qresult.z = -qresult.z;
    }
    return qresult;
}

/**
 * @brief Normalizes a quaternion to ensure it is a unit quaternion.
 */
//NOTE: our quaternions should automatically be unit - LOST is for sure, TODO: check IMU
quat quat_normalize(quat q) {
    double norm = sqrt(q.real*q.real + q.x*q.x + q.y*q.y + q.z*q.z);
    if (norm > 1e-12) { // Check for near-zero norm
        q.real /= norm;
        q.x /= norm;
        q.y /= norm;
        q.z /= norm;
    } else {
        // Handle error: return identity or print error
        q.real = 1.0; q.x = 0.0; q.y = 0.0; q.z = 0.0;
        // In a real system, you would log this error and handle it robustly
    }
    
    return q;
}


