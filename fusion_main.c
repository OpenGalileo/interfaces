#include "quat_math.h"
#include "filtering.h"
#include <math.h>

//INPUTS
quat qstar; //= pull_from_lost(time frequency t)
quat qimu; // = pull_from_imu(time freqeuncy t)

//STORED HERE
quat qoffset = {1.0, 0.0, 0.0, 0.0};
quat qfused; // output of fuse function
quat qfused_avg; // OPTIONAL?
quat qfused_prev = {1.0, 0.0, 0.0, 0.0}; //stored so fusion can be avaerged MAYBE OPTIONAL
quat qstar_prev; //updated in updatelost
quat qimu_last = {1.0, 0.0, 0.0, 0.0}; //stores last imu value just for constant access in update from lost 
//^^ could also store imu data at time lost is requested to run? or average all imu data over that period? idk

int main(void){
    while(1){
        //if(/*new imu data*/){
            qfused = fuse_imu(qimu);//fuse current imu data with offset
            qfused_avg = slerp(qfused_prev, qfused, 0.1); // average old fused data with this for smoother transition
            qfused_prev = qfused; //update last fusion calculation
            qimu_last = qimu; // update last imu data, might not be necessary
        //}

        //if(/*new startracker data*/){
            updateFromLost(qstar, qimu_last); // creates new offset (or averages towards new offset)
            //using most recent lost data and stored imu data (could just use current imu data)?
        //}

    }
}

//if in fusion mode, would return qfused or qfused_Avg
