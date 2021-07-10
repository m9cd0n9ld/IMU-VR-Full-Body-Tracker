#pragma once

#include <Globals.h>

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))

class ImuUDP
{
public:
	void init();
	void deinit();

private:
	void start();

	PACK(struct Payload {
		uint8_t header;
		uint8_t id;
		float x;
		float y;
		float z;
		float w;
		uint8_t footer;
	});

	PACK(struct PayloadSettings {
		uint8_t header;

		float lshin_x;
		float lshin_y;
		float lshin_z;

		float rshin_x;
		float rshin_y;
		float rshin_z;

		float lthigh_x;
		float lthigh_y;
		float lthigh_z;

		float rthigh_x;
		float rthigh_y;
		float rthigh_z;

		float waist_x;
		float waist_y;
		float waist_z;

		float chest_x;
		float chest_y;
		float chest_z;

		float shin;
		float thigh;
		float back;
		float head;
		float hip_width;
		float shin_sensor;
		float thigh_sensor;
		float waist_sensor;
		float chest_sensor;

		bool chest_en;

		uint8_t footer;
	});

	PACK(struct Calibrate {
		uint8_t header;
		uint8_t calib;
		uint8_t footer;
	});

	PACK(struct Ping {
		uint8_t header;
		uint8_t msg;
		uint16_t driverPort;
		uint8_t footer;
	});

	Payload* payload;
	PayloadSettings* payload_settings;
	Calibrate* calibrate;
	Ping ping;

	SOCKET socketS;
	int bytes_read;
	sockaddr_in local;
	int locallen = sizeof(local);
	bool bKeepReading = false;
	uint16_t driverPort = 0;
	char buff[128];

	sockaddr_in localT;
	int locallenT = sizeof(localT);
	const uint16_t serverPort = 6969;

	std::thread* pSocketThread = NULL;

	std::chrono::high_resolution_clock::time_point t_lshin_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_rshin_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_lthigh_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_rthigh_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_waist_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_chest_last = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point t_server_last = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();
	double elapsed_time_ms;
};
