#ifndef COMPASS_H
#define COMPASS_H

#define PI 3.14159265358979323846

INLINE float deg2rad(float deg)
{
	return deg * PI / 180;
}

INLINE float rad2deg(float rad)
{
	return rad * 180 / PI;
}

float distance(float lat1, float lon1, float lat2, float lon2);
int bearing(float lat1, float lon1, float lat2, float lon2);
const char *compass_heading(int bearing);

#endif /* COMPASS_H */
