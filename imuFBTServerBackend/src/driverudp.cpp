#include <driverudp.h>

bool driverudp::init() {
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == 0) {
		local.sin_family = AF_INET;
		inet_pton(AF_INET, "0.0.0.0", &local.sin_addr.s_addr);
		local.sin_port = htons(0);
		socketD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		int iTimeout = 100;
		setsockopt(socketD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&iTimeout, sizeof(iTimeout));

		localD.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &localD.sin_addr.s_addr);
		localD.sin_port = htons(driverPort);

		if (socketD != INVALID_SOCKET) {

			iResult = bind(socketD, (sockaddr*)&local, sizeof(local));

			if (iResult != SOCKET_ERROR) {
				struct sockaddr_in sin;
				socklen_t len = sizeof(sin);
				getsockname(socketD, (struct sockaddr*)&sin, &len);
				bridgePort = ntohs(sin.sin_port);

				DEBUG("Driver socket initialized\n");
			}
			else {
				WSACleanup();
				DEBUG("Driver socket bind failed\n");
				return false;
			}

		}
		else {
			WSACleanup();
			DEBUG("Driver socket invalid\n");
			return false;
		}

	}
	else
	{
		WSACleanup();
		DEBUG("Driver WSA startup failed\n");
		return false;
	}


	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == 0) {
		localb.sin_family = AF_INET;
		inet_pton(AF_INET, "0.0.0.0", &localb.sin_addr.s_addr);
		localb.sin_port = htons(broadPort);
		socketB = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		int iTimeout = 100;
		setsockopt(socketB, SOL_SOCKET, SO_RCVTIMEO, (const char*)&iTimeout, sizeof(iTimeout));
		int enable = 1;
		setsockopt(socketB, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));

		if (socketB != INVALID_SOCKET) {

			iResult = bind(socketB, (sockaddr*)&localb, sizeof(localb));

			if (iResult != SOCKET_ERROR) {
				char strport[8];
				char hostname[128];
				gethostname(hostname, sizeof(hostname));
				ZeroMemory(&hints, sizeof(hints));
				hints.ai_family = AF_INET;
				hints.ai_socktype = SOCK_DGRAM;
				hints.ai_protocol = IPPROTO_UDP;
				sprintf_s(strport, "%d", broadPort);
				getaddrinfo(hostname, strport, &hints, &result);

				DEBUG("Driver broadcast socket initialized\n");
			}
			else {
				WSACleanup();
				DEBUG("Driver broadcast socket bind failed\n");
				return false;
			}

		}
		else {
			WSACleanup();
			DEBUG("Driver broadcast socket invalid\n");
			return false;
		}

	}
	else
	{
		WSACleanup();
		DEBUG("Driver broadcast WSA startup failed\n");
		return false;
	}

	SocketActivated = true;
	pSocketThread = new std::thread(&driverudp::start, this);
	bSocketThread = new std::thread(&driverudp::startB, this);
	return true;
}

void driverudp::deinit() {
	if (SocketActivated) {
		SocketActivated = false;
		if (pSocketThread) {
			pSocketThread->join();
			delete pSocketThread;
			pSocketThread = nullptr;
		}
		if (bSocketThread) {
			bSocketThread->join();
			delete bSocketThread;
			bSocketThread = nullptr;
		}
		closesocket(socketD);
		closesocket(socketB);
		WSACleanup();
	}
}

void driverudp::startB() {
	while (SocketActivated) {
		memset(buffb, 0, sizeof(buffb));
		bytes_readb = recvfrom(socketB, buffb, sizeof(buffb), 0, (sockaddr*)&localb, &locallenb);
		if (bytes_readb == sizeof(Ping)) {
			ping = (Ping*)buffb;
			if (ping->header == uint8_t('I') && ping->footer == (uint8_t)'i') {
				if (ping->msg == 231) {
					bool from_local_host = false;
					char remoteIP[128];
					char ipstringbuffer[128];
					inet_ntop(AF_INET, &localb.sin_addr, remoteIP, sizeof(remoteIP));
					if (std::string(remoteIP).compare("127.0.0.1")) {
						from_local_host = true;
					}
					else {
						for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
							sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
							inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, sizeof(ipstringbuffer));
							if (std::string(ipstringbuffer).compare(std::string(remoteIP))) {
								from_local_host = true;
							}
						}
					}

					if (from_local_host) {
						t_driver_rx = std::chrono::high_resolution_clock::now();
						driverPort = ping->driverPort;
						localD.sin_port = htons(driverPort);
						DEBUG("Broadcast ping received from driver\n");
						if (driverPort != 0) {
							ack.header = (uint8_t)'I';
							ack.msg = 242;
							ack.bridgePort = bridgePort;
							ack.footer = (uint8_t)'i';
							sendto(socketB, (char*)&ack, sizeof(ack), 0, (sockaddr*)&localD, locallenD);
							DEBUG("Broadcast ack sent to driver\n");
						}
					}
				}
			}
		}
	}
}

void driverudp::start() {
	while (SocketActivated) {
		memset(buff, 0, sizeof(buff));
		bytes_read = recvfrom(socketD, buff, sizeof(buff), 0, (sockaddr*)&local, &locallen);

		if (bytes_read == sizeof(Ping)) {
			ping = (Ping*)buff;
			if (ping->header == uint8_t('I') && ping->footer == (uint8_t)'i') {
				if (ping->msg == 251) {
					t_driver_rx = std::chrono::high_resolution_clock::now();
					driverPort = ping->driverPort;
					localD.sin_port = htons(driverPort);
					DEBUG("Init settings request received from driver\n");
					if (init_recv && driverPort != 0) {
						init_settings.header = (uint8_t)'I';
						init_settings.feet_en = feet_en;
						init_settings.shin_en = shin_en;
						init_settings.thigh_en = thigh_en;
						init_settings.waist_en = waist_en;
						init_settings.chest_en = chest_en;
						init_settings.shoulder_en = shoulder_en;
						init_settings.upperarm_en = upperarm_en;
						init_settings.footer = (uint8_t)'i';
						sendto(socketD, (char*)&init_settings, sizeof(init_settings), 0, (sockaddr*)&localD, locallenD);
						DEBUG("Init settings sent to driver\n");
					}
				}
				else if (ping->msg == 241) {
					t_driver_rx = std::chrono::high_resolution_clock::now();
					driverPort = ping->driverPort;
					localD.sin_port = htons(driverPort);
					DEBUG("Unicast ping received from driver\n");
					if (driverPort != 0) {
						ack.header = (uint8_t)'I';
						ack.msg = 242;
						ack.bridgePort = bridgePort;
						ack.footer = (uint8_t)'i';
						sendto(socketD, (char*)&ack, sizeof(ack), 0, (sockaddr*)&localD, locallenD);
						DEBUG("Unicast ack sent to driver\n");
					}
				}
			}
		}

		t_end = std::chrono::high_resolution_clock::now();
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_driver_rx).count();
		if (elapsed_time_ms >= 5000) {
			driverPort = 0;
		}
	}
}

void driverudp::send(char* data, uint64_t len) {
	if (driverPort != 0) {
		sendto(socketD, data, len, 0, (sockaddr*)&localD, locallenD);
		DEBUGFAST("Driver payload sent to driver\n");
	}
}