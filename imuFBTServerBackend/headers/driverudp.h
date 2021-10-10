#pragma once

#include <globals.h>

class driverudp
{
public:
	bool init();
	void deinit();
	void send(char* data, uint64_t len);

private:
	void start();
	void startB();

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

	Ping* ping;
	Ack ack;
	InitSettings init_settings;

	SOCKET socketD;
	int bytes_read;
	sockaddr_in local;
	int locallen = sizeof(local);
	uint16_t bridgePort = 0;
	char buff[16];

	sockaddr_in localD;
	int locallenD = sizeof(localD);
	uint16_t driverPort = 0;

	SOCKET socketB;
	int bytes_readb;
	sockaddr_in localb;
	int locallenb = sizeof(localb);
	const uint16_t broadPort = 6969;
	char buffb[16];

	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;
	struct sockaddr_in* sockaddr_ipv4;

	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point t_driver_rx = std::chrono::high_resolution_clock::now();
	double elapsed_time_ms;

	std::thread* pSocketThread = NULL;
	std::thread* bSocketThread = NULL;
	
	bool SocketActivated = false;
};