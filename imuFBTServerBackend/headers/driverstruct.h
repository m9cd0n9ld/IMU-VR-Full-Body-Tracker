#pragma once

#include <globals.h>

int16_t map(float x, float in_min = -1, float in_max = 1, float out_min = -32767, float out_max = 32767) {
	return (int16_t)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

PACK(struct DriverPayload {
	uint8_t header;

	uint8_t id;
	bool available;
	float Px;
	float Py;
	float Pz;
	int16_t Qx;
	int16_t Qy;
	int16_t Qz;
	int16_t Qw;

	bool hmd_available;

	uint8_t footer;
});