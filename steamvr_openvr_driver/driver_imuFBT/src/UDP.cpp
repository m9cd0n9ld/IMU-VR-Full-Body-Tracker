#include <UDP.h>

void UDP::init() {
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == 0) {
		local.sin_family = AF_INET;
		inet_pton(AF_INET, "0.0.0.0", &local.sin_addr.s_addr);
		local.sin_port = htons(0);
		socketS = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		int iTimeout = 100;
		setsockopt(socketS, SOL_SOCKET, SO_RCVTIMEO, (const char*)&iTimeout, sizeof(iTimeout));

		localB.sin_family = AF_INET;
		inet_pton(AF_INET, "255.255.255.255", &localB.sin_addr.s_addr);
		localB.sin_port = htons(broadPort);
		socketB = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		char broadcast = 1;
		setsockopt(socketB, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

		localT.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &localT.sin_addr.s_addr);
		localT.sin_port = htons(bridgePort);

		if (socketS != INVALID_SOCKET) {

			iResult = bind(socketS, (sockaddr*)&local, sizeof(local));

			if (iResult != SOCKET_ERROR) {
				struct sockaddr_in sin;
				socklen_t len = sizeof(sin);
				getsockname(socketS, (struct sockaddr*)&sin, &len);
				driverPort = ntohs(sin.sin_port);

				SocketActivated = true;
				pSocketThread = new std::thread(&UDP::start, this);
			}
			else {
				WSACleanup();
				SocketActivated = false;
			}

		}
		else {
			WSACleanup();
			SocketActivated = false;
		}

	}
	else
	{
		WSACleanup();
		SocketActivated = false;
	}
}

void UDP::start()
{
	while (SocketActivated) {
		memset(buff, 0, sizeof(buff));
		bytes_read = recvfrom(socketS, buff, sizeof(buff), 0, (sockaddr*)&local, &locallen);

		if (bytes_read == sizeof(InitSettings)) {
			initsettings = (InitSettings*)buff;
			if (initsettings->header == (uint8_t)'I' && initsettings->footer == (uint8_t)'i') {
				feet_en = initsettings->feet_en;
				shin_en = initsettings->shin_en;
				thigh_en = initsettings->thigh_en;
				waist_en = initsettings->waist_en;
				chest_en = initsettings->chest_en;
				shoulder_en = initsettings->shoulder_en;
				upperarm_en = initsettings->upperarm_en;
				init_recv = true;
				initialized = true;
			}
		}

		else if (bytes_read == sizeof(Ack)) {
			ack = (Ack*)buff;
			if (ack->header == (uint8_t)'I' && ack->footer == (uint8_t)'i') {
				if (ack->msg == 242) {
					t_bridge_rx = std::chrono::high_resolution_clock::now();
					bridgePort = ack->bridgePort;
					localT.sin_port = htons(bridgePort);
					toBroadcast = false;
				}
			}
		}

		else if (bytes_read == sizeof(DriverPayload)) {
			driverpayload = (DriverPayload*)buff;
			if (driverpayload->header == (uint8_t)'I' && driverpayload->footer == (uint8_t)'i') {
				setValue(driverpayload);
			}
		}

		t_end = std::chrono::high_resolution_clock::now();

		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_bridge_last).count();
		if (elapsed_time_ms >= 1000) {
			t_bridge_last = std::chrono::high_resolution_clock::now();
			if (toBroadcast) {
				ping.header = (uint8_t)'I';
				ping.msg = 231;
				ping.driverPort = driverPort;
				ping.footer = (uint8_t)'i';
				sendto(socketB, (char*)&ping, sizeof(ping), 0, (sockaddr*)&localB, locallenB);
			}
			else {
				if (!initialized) {
					ping.header = (uint8_t)'I';
					ping.msg = 251;
					ping.driverPort = driverPort;
					ping.footer = (uint8_t)'i';
				}
				else {
					ping.header = (uint8_t)'I';
					ping.msg = 241;
					ping.driverPort = driverPort;
					ping.footer = (uint8_t)'i';
				}
				if (bridgePort != 0) {
					sendto(socketS, (char*)&ping, sizeof(ping), 0, (sockaddr*)&localT, locallenT);
				}
			}
		}

		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_bridge_rx).count();
		if (elapsed_time_ms >= 5000) {
			bridgePort = 0;
			toBroadcast = true;
		}

		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_lfoot_last).count();
		if (elapsed_time_ms >= 5000) {
			lfoot_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_rfoot_last).count();
		if (elapsed_time_ms >= 5000) {
			rfoot_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_lshin_last).count();
		if (elapsed_time_ms >= 5000) {
			lshin_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_rshin_last).count();
		if (elapsed_time_ms >= 5000) {
			rshin_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_lthigh_last).count();
		if (elapsed_time_ms >= 5000) {
			lthigh_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_rthigh_last).count();
		if (elapsed_time_ms >= 5000) {
			rthigh_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_waist_last).count();
		if (elapsed_time_ms >= 5000) {
			waist_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_chest_last).count();
		if (elapsed_time_ms >= 5000) {
			chest_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_lshoulder_last).count();
		if (elapsed_time_ms >= 5000) {
			lshoulder_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_rshoulder_last).count();
		if (elapsed_time_ms >= 5000) {
			rshoulder_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_lupperarm_last).count();
		if (elapsed_time_ms >= 5000) {
			lupperarm_available = false;
		}
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_rupperarm_last).count();
		if (elapsed_time_ms >= 5000) {
			rupperarm_available = false;
		}
	}
}

void UDP::deinit() {
	if (SocketActivated) {
		SocketActivated = false;
		if (pSocketThread) {
			pSocketThread->join();
			delete pSocketThread;
			pSocketThread = nullptr;
		}
		closesocket(socketS);
		WSACleanup();
	}
}

void UDP::setValue(DriverPayload* driverpayload) {

	hmd_available = driverpayload->hmd_available;

	switch (driverpayload->id) {

	case LFOOT:
		t_lfoot_last = std::chrono::high_resolution_clock::now();
		lfoot_available = driverpayload->available;
		Px_lfoot = driverpayload->Px;
		Py_lfoot = driverpayload->Py;
		Pz_lfoot = driverpayload->Pz;
		Qx_lfoot = driverpayload->Qx;
		Qy_lfoot = driverpayload->Qy;
		Qz_lfoot = driverpayload->Qz;
		Qw_lfoot = driverpayload->Qw;
		break;

	case RFOOT:
		t_rfoot_last = std::chrono::high_resolution_clock::now();
		rfoot_available = driverpayload->available;
		Px_rfoot = driverpayload->Px;
		Py_rfoot = driverpayload->Py;
		Pz_rfoot = driverpayload->Pz;
		Qx_rfoot = driverpayload->Qx;
		Qy_rfoot = driverpayload->Qy;
		Qz_rfoot = driverpayload->Qz;
		Qw_rfoot = driverpayload->Qw;
		break;

	case LSHIN:
		t_lshin_last = std::chrono::high_resolution_clock::now();
		lshin_available = driverpayload->available;
		Px_lshin = driverpayload->Px;
		Py_lshin = driverpayload->Py;
		Pz_lshin = driverpayload->Pz;
		Qx_lshin = driverpayload->Qx;
		Qy_lshin = driverpayload->Qy;
		Qz_lshin = driverpayload->Qz;
		Qw_lshin = driverpayload->Qw;
		break;

	case RSHIN:
		t_rshin_last = std::chrono::high_resolution_clock::now();
		rshin_available = driverpayload->available;
		Px_rshin = driverpayload->Px;
		Py_rshin = driverpayload->Py;
		Pz_rshin = driverpayload->Pz;
		Qx_rshin = driverpayload->Qx;
		Qy_rshin = driverpayload->Qy;
		Qz_rshin = driverpayload->Qz;
		Qw_rshin = driverpayload->Qw;
		break;

	case LTHIGH:
		t_lthigh_last = std::chrono::high_resolution_clock::now();
		lthigh_available = driverpayload->available;
		Px_lthigh = driverpayload->Px;
		Py_lthigh = driverpayload->Py;
		Pz_lthigh = driverpayload->Pz;
		Qx_lthigh = driverpayload->Qx;
		Qy_lthigh = driverpayload->Qy;
		Qz_lthigh = driverpayload->Qz;
		Qw_lthigh = driverpayload->Qw;
		break;

	case RTHIGH:
		t_rthigh_last = std::chrono::high_resolution_clock::now();
		rthigh_available = driverpayload->available;
		Px_rthigh = driverpayload->Px;
		Py_rthigh = driverpayload->Py;
		Pz_rthigh = driverpayload->Pz;
		Qx_rthigh = driverpayload->Qx;
		Qy_rthigh = driverpayload->Qy;
		Qz_rthigh = driverpayload->Qz;
		Qw_rthigh = driverpayload->Qw;
		break;

	case WAIST:
		t_waist_last = std::chrono::high_resolution_clock::now();
		waist_available = driverpayload->available;
		Px_waist = driverpayload->Px;
		Py_waist = driverpayload->Py;
		Pz_waist = driverpayload->Pz;
		Qx_waist = driverpayload->Qx;
		Qy_waist = driverpayload->Qy;
		Qz_waist = driverpayload->Qz;
		Qw_waist = driverpayload->Qw;
		break;

	case CHEST:
		t_chest_last = std::chrono::high_resolution_clock::now();
		chest_available = driverpayload->available;
		Px_chest = driverpayload->Px;
		Py_chest = driverpayload->Py;
		Pz_chest = driverpayload->Pz;
		Qx_chest = driverpayload->Qx;
		Qy_chest = driverpayload->Qy;
		Qz_chest = driverpayload->Qz;
		Qw_chest = driverpayload->Qw;
		break;

	case LSHOULDER:
		t_lshoulder_last = std::chrono::high_resolution_clock::now();
		lshoulder_available = driverpayload->available;
		Px_lshoulder = driverpayload->Px;
		Py_lshoulder = driverpayload->Py;
		Pz_lshoulder = driverpayload->Pz;
		Qx_lshoulder = driverpayload->Qx;
		Qy_lshoulder = driverpayload->Qy;
		Qz_lshoulder = driverpayload->Qz;
		Qw_lshoulder = driverpayload->Qw;
		break;

	case RSHOULDER:
		t_rshoulder_last = std::chrono::high_resolution_clock::now();
		rshoulder_available = driverpayload->available;
		Px_rshoulder = driverpayload->Px;
		Py_rshoulder = driverpayload->Py;
		Pz_rshoulder = driverpayload->Pz;
		Qx_rshoulder = driverpayload->Qx;
		Qy_rshoulder = driverpayload->Qy;
		Qz_rshoulder = driverpayload->Qz;
		Qw_rshoulder = driverpayload->Qw;
		break;

	case LUPPERARM:
		t_lupperarm_last = std::chrono::high_resolution_clock::now();
		lupperarm_available = driverpayload->available;
		Px_lupperarm = driverpayload->Px;
		Py_lupperarm = driverpayload->Py;
		Pz_lupperarm = driverpayload->Pz;
		Qx_lupperarm = driverpayload->Qx;
		Qy_lupperarm = driverpayload->Qy;
		Qz_lupperarm = driverpayload->Qz;
		Qw_lupperarm = driverpayload->Qw;
		break;

	case RUPPERARM:
		t_rupperarm_last = std::chrono::high_resolution_clock::now();
		rupperarm_available = driverpayload->available;
		Px_rupperarm = driverpayload->Px;
		Py_rupperarm = driverpayload->Py;
		Pz_rupperarm = driverpayload->Pz;
		Qx_rupperarm = driverpayload->Qx;
		Qy_rupperarm = driverpayload->Qy;
		Qz_rupperarm = driverpayload->Qz;
		Qw_rupperarm = driverpayload->Qw;
		break;
	}
}