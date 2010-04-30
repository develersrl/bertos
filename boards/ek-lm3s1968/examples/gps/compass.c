#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <math.h>
#include "compass.h"

static const char *_compass_heading[] =
{
	"N", "NNE", "NE", "ENE",
	"E", "ESE", "SE", "SSE",
	"S", "SSW", "SW", "WSW",
	"W", "WNW", "NW", "NNW",
};

/**
 * Use the Haversine formula to calculate great-circle distances between the
 * two points.
 *
 * The Haversine formula remains particularly well-conditioned for numerical
 * computation even at small distances, unlike calculations based on the
 * spherical law of cosines.
 */
float distance(float lat1, float lon1, float lat2, float lon2)
{
	const float PLANET_RADIUS = 6371000;
	float d_lat = deg2rad(lat2 - lat1);
	float d_lon = deg2rad(lon2 - lon1);

	float a = sin(d_lat / 2) * sin(d_lat / 2) +
			cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *
			sin(d_lon / 2) * sin(d_lon / 2);
	float c = 2 * atan2(sqrt(a), sqrt(1 - a));

	return PLANET_RADIUS * c;
}

/**
 * Evaluate the bearing (also known as forward azimuth) using spherical law
 * coordinates.
 *
 * The bearing is a straight line along a great-circle arc from the start point
 * to the destination point.
 */
int bearing(float lat1, float lon1, float lat2, float lon2)
{
	float res;

	res = rad2deg(atan2(sin(deg2rad(lon2 - lon1)) *
		cos(deg2rad(lat2)), cos(deg2rad(lat1)) *
		sin(deg2rad(lat2)) - sin(deg2rad(lat1)) *
		cos(deg2rad(lat2)) * cos(deg2rad(lon2) -
		deg2rad(lon1))));
	return ((int)res + 360) % 360;
}

const char *compass_heading(int bearing)
{
	ASSERT(bearing >= 0 && bearing < 360);
	/*
	 * bearing / 22.5
	 */
	return _compass_heading[(bearing << 4) / 360];
}
