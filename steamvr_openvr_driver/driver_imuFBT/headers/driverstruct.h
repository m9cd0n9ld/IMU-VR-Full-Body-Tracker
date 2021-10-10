#pragma once

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))


PACK(struct InitSettings {
	uint8_t header;
	bool feet_en;
	bool shin_en;
	bool thigh_en;
	bool waist_en;
	bool chest_en;
	bool shoulder_en;
	bool upperarm_en;
	uint8_t footer;
});

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