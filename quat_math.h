#include <math.h>

/**
 * @brief Structure to hold a quaternion [real, x, y, z].
 * * Quaternions are used to represent 3D rotations. The convention is:
 * - w: scalar component (cos(theta/2))
 * - x, y, z: vector component (axis * sin(theta/2))
 */
typedef struct {
    double real;
    double x;
    double y;
    double z;
} quat;

// --- Function Declarations ---

/**
 * @brief Multiplies two quaternions (qA * qB).
 * * The product qC = qA * qB represents rotation qB followed by rotation qA.
 * * @param qA The first quaternion (left-hand side).
 * @param qB The second quaternion (right-hand side).
 * @return quaternion_t The resulting quaternion (qC).
 */
quat quat_multiply(const quat qA, const quat qB);

/**
 * @brief Calculates the conjugate (inverse) of a quaternion.
 * * For a unit quaternion, the conjugate is its inverse: q * q* = [1, 0, 0, 0].
 * * @param q The input quaternion.
 * @return quaternion_t The resulting conjugate quaternion.
 */
quat quat_conjugate(const quat q);

/**
 * @brief Normalizes a quaternion to ensure it is a unit quaternion.
 * * Unit quaternions are required to accurately represent rotations.
 * * @param q The input quat.
 * @return quat - The resulting normalized quaternion.
 */
quat quat_normalize(quat q);

/**
 * @brief chooses the smaller/nearer angle to prevent 80 degree flips
 * * @param  qnew and qprev, representing last two angles to ensure no flip
 * @return the quat of the correct signedness
 */
quat quat_nearest_angle(const quat qnew, const quat qprev);

