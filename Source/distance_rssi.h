#ifndef DISTANCE_RSSI_H   // guardas de cabe�alho, impedem inclus�es c�clicas
#define DISTANCE_RSSI_H

#include <math.h>

/* PARAM
   rssi = value in dB
   a = value in dB
*/
double getDistance_RSSI(double rssi, double a);

#endif
