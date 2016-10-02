/*RSSI = -10 * n * log(distance) - A
  n ranges from 2 to 4
  A is the RSSI value equivalent of distance of 1 meter between transmitter and receiver

  distance = 10^-(RSSI + A)/(10 * n)
*/
#include "distance_rssi.h"
#include <math.h>

double getDistance_RSSI(double rssi, double a) {

    return pow(10, (a-rssi) / (10 * 2));
}
