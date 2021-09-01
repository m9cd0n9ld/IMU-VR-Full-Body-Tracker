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
	float map(float x, float in_min, float in_max, float out_min, float out_max);
	void setValue(uint8_t id, float x, float y, float z, float w);

	PACK(struct PayloadExt {
		uint8_t header;
		uint8_t id;
		int16_t x;
		int16_t y;
		int16_t z;
		int16_t w;
		uint8_t id_ext;
		int16_t x_ext;
		int16_t y_ext;
		int16_t z_ext;
		int16_t w_ext;
		uint8_t footer;
	});

	PACK(struct Payload {
		uint8_t header;
		uint8_t id;
		int16_t x;
		int16_t y;
		int16_t z;
		int16_t w;
		uint8_t footer;
	});

	PACK(struct PayloadSettings {
		uint8_t header;

		float lfoot_x;
		float lfoot_y;
		float lfoot_z;

		float rfoot_x;
		float rfoot_y;
		float rfoot_z;

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

		float lshoulder_x;
		float lshoulder_y;
		float lshoulder_z;

		float rshoulder_x;
		float rshoulder_y;
		float rshoulder_z;

		float lupperarm_x;
		float lupperarm_y;
		float lupperarm_z;

		float rupperarm_x;
		float rupperarm_y;
		float rupperarm_z;

		float shin;
		float thigh;
		float lback;
		float uback;
		float head;
		float shoulder;
		float hip_width;
		float shoulder_width;
		float foot_sensor;
		float shin_sensor;
		float thigh_sensor;
		float waist_sensor;
		float chest_sensor;
		float shoulder_sensor;
		float upperarm_sensor;

		float floor_offset;
		bool override_feet;

		uint8_t footer;
	});

	PACK(struct OffsetSettings {
		uint8_t header;

		float lfoot_1;
		float lfoot_2;

		float rfoot_1;
		float rfoot_2;

		float lshin_1;
		float lshin_2;

		float rshin_1;
		float rshin_2;

		float lthigh_1;
		float lthigh_2;

		float rthigh_1;
		float rthigh_2;

		float waist_1;
		float waist_2;

		float chest_1;
		float chest_2;

		float lshoulder_1;
		float lshoulder_2;

		float rshoulder_1;
		float rshoulder_2;

		float lupperarm_1;
		float lupperarm_2;

		float rupperarm_1;
		float rupperarm_2;

		float head_1;
		float head_2;

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
	};)

	PayloadExt* payloadext;
	Payload* payload;
	PayloadSettings* payload_settings;
	OffsetSettings* offset_settings;
	Calibrate* calibrate;
	Ping ping;
	InitSettings* init_settings;

	SOCKET socketS;
	int bytes_read;
	sockaddr_in local;
	int locallen = sizeof(local);
	uint16_t driverPort = 0;
	char buff[256];

	sockaddr_in localT;
	int locallenT = sizeof(localT);
	const uint16_t serverPort = 6969;

	std::thread* pSocketThread = NULL;

	std::chrono::high_resolution_clock::time_point t_lfoot_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_rfoot_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_lshin_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_rshin_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_lthigh_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_rthigh_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_waist_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_chest_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_lshoulder_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_rshoulder_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_lupperarm_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_rupperarm_last = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point t_server_last = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();
	double elapsed_time_ms;

	bool initialized = false;
};
