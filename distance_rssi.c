/*RSSI = -10 * n * log(distance) - A
  n ranges from 2 to 4
  A is the RSSI value equivalent of distance of 1 meter between transmitter and receiver

  distance = 10^-(RSSI + A)/(10 * n)
*/
#include "distance_rssi.h"
#include <math.h>

double getDistance_RSSI(double rssi, double a) {

	double ratio = rssi*1.0/a;
  	if (ratio < 1.0) {
    		return pow(ratio,10);
	}
  	else {
    		double accuracy =  (0.89976)*pow(ratio,7.7095) + 0.111;    
    	return accuracy;
  	}
}
