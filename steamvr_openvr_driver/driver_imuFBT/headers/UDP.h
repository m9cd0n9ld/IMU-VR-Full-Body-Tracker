#pragma once

#include <Globals.h>
#include <driverstruct.h>

class UDP
{
public:
	void init();
	void deinit();

private:
	void start();
	void setValue(DriverPayload* driverpayload);

	PACK(struct Ping {
		uint8_t header;
		uint8_t msg;
		uint16_t driverPort;
		uint8_t footer;
	});

	PACK(struct Ack {
		uint8_t header;
		uint8_t msg;
		uint16_t bridgePort;
		uint8_t footer;
	});

	
	Ping ping;
	Ack* ack;
	InitSettings* initsettings;
	DriverPayload* driverpayload;

	SOCKET socketS;
	int bytes_read;
	sockaddr_in local;
	int locallen = sizeof(local);
	uint16_t driverPort = 0;
	char buff[350];

	sockaddr_in localT;
	int locallenT = sizeof(localT);
	uint16_t bridgePort = 0;

	SOCKET socketB;
	sockaddr_in localB;
	int locallenB = sizeof(localB);
	const uint16_t broadPort = 6969;
	bool toBroadcast = true;

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

	std::chrono::high_resolution_clock::time_point t_bridge_last = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point t_bridge_rx = std::chrono::high_resolution_clock::now();
	double elapsed_time_ms;

	bool initialized = false;
};
