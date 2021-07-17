#include <ImuUDP.h>

void ImuUDP::init() {
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

		localT.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &localT.sin_addr.s_addr);
		localT.sin_port = htons(serverPort);

		if (socketS != INVALID_SOCKET) {

			iResult = bind(socketS, (sockaddr*)&local, sizeof(local));

			if (iResult != SOCKET_ERROR) {
				struct sockaddr_in sin;
				socklen_t len = sizeof(sin);
				getsockname(socketS, (struct sockaddr*)&sin, &len);
				driverPort = ntohs(sin.sin_port);

				SocketActivated = true;
				pSocketThread = new std::thread(&ImuUDP::start, this);
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

void ImuUDP::start()
{
	while (SocketActivated) {
		bKeepReading = true;
		while (bKeepReading) {
			memset(buff, 0, sizeof(buff));
			bytes_read = recvfrom(socketS, buff, sizeof(buff), 0, (sockaddr*)&local, &locallen);

			if (bytes_read == 19) {
				payload = (Payload*)buff;
				if (payload->header == (uint8_t)'I' && payload->footer == (uint8_t)'i') {
					switch (payload->id) {
					case LSHIN:
						imu_lshin = Quaternionf(payload->w, payload->x, payload->y, payload->z);
						t_lshin_last = std::chrono::high_resolution_clock::now();
						lshin_available = true;
						break;
					case RSHIN:
						imu_rshin = Quaternionf(payload->w, payload->x, payload->y, payload->z);
						t_rshin_last = std::chrono::high_resolution_clock::now();
						rshin_available = true;
						break;
					case LTHIGH:
						imu_lthigh = Quaternionf(payload->w, payload->x, payload->y, payload->z);
						t_lthigh_last = std::chrono::high_resolution_clock::now();
						lthigh_available = true;
						break;
					case RTHIGH:
						imu_rthigh = Quaternionf(payload->w, payload->x, payload->y, payload->z);
						t_rthigh_last = std::chrono::high_resolution_clock::now();
						rthigh_available = true;
						break;
					case WAIST:
						imu_waist = Quaternionf(payload->w, payload->x, payload->y, payload->z);
						t_waist_last = std::chrono::high_resolution_clock::now();
						waist_available = true;
						break;
					case CHEST:
						imu_chest = Quaternionf(payload->w, payload->x, payload->y, payload->z);
						t_chest_last = std::chrono::high_resolution_clock::now();
						chest_available = true;
						break;
					}
				}
			}

			else if (bytes_read == 58) {
				offset_settings = (OffsetSettings*)buff;
				if (offset_settings->header == (uint8_t)'I' && offset_settings->footer == (uint8_t)'i') {
					Vector2f lshin_offset(offset_settings->lshin_x, offset_settings->lshin_z);
					Vector2f rshin_offset(offset_settings->rshin_x, offset_settings->rshin_z);
					Vector2f lthigh_offset(offset_settings->lthigh_x, offset_settings->lthigh_z);
					Vector2f rthigh_offset(offset_settings->rthigh_x, offset_settings->rthigh_z);
					Vector2f waist_offset(offset_settings->waist_x, offset_settings->waist_z);
					Vector2f chest_offset(offset_settings->chest_x, offset_settings->chest_z);
					Vector2f head_offset(offset_settings->head_x, offset_settings->head_z);
					bk->setHorizontalOffset(lshin_offset, rshin_offset, lthigh_offset, rthigh_offset, waist_offset, chest_offset, head_offset);
				}
			}

			else if (bytes_read == 111) {
				payload_settings = (PayloadSettings*)buff;
				if (payload_settings->header == (uint8_t)'I' && payload_settings->footer == (uint8_t)'i') {
					Quaternionf T_lshin = bk->euXYZ_to_quat(payload_settings->lshin_x, payload_settings->lshin_y, payload_settings->lshin_z);
					Quaternionf T_rshin = bk->euXYZ_to_quat(payload_settings->rshin_x, payload_settings->rshin_y, payload_settings->rshin_z);
					Quaternionf T_lthigh = bk->euXYZ_to_quat(payload_settings->lthigh_x, payload_settings->lthigh_y, payload_settings->lthigh_z);
					Quaternionf T_rthigh = bk->euXYZ_to_quat(payload_settings->rthigh_x, payload_settings->rthigh_y, payload_settings->rthigh_z);
					Quaternionf T_waist = bk->euXYZ_to_quat(payload_settings->waist_x, payload_settings->waist_y, payload_settings->waist_z);
					Quaternionf T_chest = bk->euXYZ_to_quat(payload_settings->chest_x, payload_settings->chest_y, payload_settings->chest_z);
					bk->setSensorTransform(T_lshin, T_rshin, T_lthigh, T_rthigh, T_waist, T_chest);
					bk->setParam(payload_settings->shin,
						payload_settings->thigh,
						payload_settings->back,
						payload_settings->head,
						payload_settings->hip_width,
						payload_settings->shin_sensor,
						payload_settings->thigh_sensor,
						payload_settings->waist_sensor,
						payload_settings->chest_sensor,
						payload_settings->chest_en);
					chest_enable = payload_settings->chest_en;
				}
			}

			else if (bytes_read == 3) {
				calibrate = (Calibrate*)buff;
				if (calibrate->header == (uint8_t)'I' && calibrate->footer == (uint8_t)'i') {
					if (calibrate->calib == 51) {
						float yaw = atan2f(-mat_hmd(2, 0), mat_hmd(0, 0));
						Quaternionf direction = bk->euYXZ_to_quat(yaw, 0, 0);
						bk->setOffset(imu_lshin, imu_rshin, imu_lthigh, imu_rthigh, imu_waist, imu_chest, direction);
					}
				}
			}

			else {
				bKeepReading = false;
			}

			t_end = std::chrono::high_resolution_clock::now();

			elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_server_last).count();
			if (elapsed_time_ms >= 1000) {
				ping.header = (uint8_t)'I';
				ping.msg = 87;
				ping.driverPort = driverPort;
				ping.footer = (uint8_t)'i';
				sendto(socketS, (char*)&ping, sizeof(ping), 0, (sockaddr*)&localT, locallenT);
				t_server_last = std::chrono::high_resolution_clock::now();
			}

			elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_lshin_last).count();
			if (elapsed_time_ms >= 1000) {
				lshin_available = false;
			}
			elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_rshin_last).count();
			if (elapsed_time_ms >= 1000) {
				rshin_available = false;
			}
			elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_lthigh_last).count();
			if (elapsed_time_ms >= 1000) {
				lthigh_available = false;
			}
			elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_rthigh_last).count();
			if (elapsed_time_ms >= 1000) {
				rthigh_available = false;
			}
			elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_waist_last).count();
			if (elapsed_time_ms >= 1000) {
				waist_available = false;
			}
			elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_chest_last).count();
			if (elapsed_time_ms >= 1000) {
				chest_available = false;
			}
		}
	}
}

void ImuUDP::deinit() {
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