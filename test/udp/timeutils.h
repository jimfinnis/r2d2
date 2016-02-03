/**
 * @file time.h
 * @brief  Brief description of file.
 *
 */

#ifndef __TIME_H
#define __TIME_H

double time_diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    
    double t = temp.tv_sec;
    double ns = temp.tv_nsec;
    t += ns*1e-9;
    return t;
}
double gettime(){
    timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    
    double t = ts.tv_nsec;
    t *= 1e-9;
    t += ts.tv_sec;
    return t;
}



#endif /* __TIME_H */
