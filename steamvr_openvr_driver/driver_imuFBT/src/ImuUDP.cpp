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

float ImuUDP::map(float x, float in_min = -32767, float in_max = 32767, float out_min = -1, float out_max = 1) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ImuUDP::setValue(uint8_t id, float x, float y, float z, float w) {
	switch (id) {
	case LFOOT:
		imu_lfoot = Quaternionf(w, x, y, z);
		t_lfoot_last = std::chrono::high_resolution_clock::now();
		lfoot_available = true;
		break;
	case RFOOT:
		imu_rfoot = Quaternionf(w, x, y, z);
		t_rfoot_last = std::chrono::high_resolution_clock::now();
		rfoot_available = true;
		break;
	case LSHIN:
		imu_lshin = Quaternionf(w, x, y, z);
		t_lshin_last = std::chrono::high_resolution_clock::now();
		lshin_available = true;
		break;
	case RSHIN:
		imu_rshin = Quaternionf(w, x, y, z);
		t_rshin_last = std::chrono::high_resolution_clock::now();
		rshin_available = true;
		break;
	case LTHIGH:
		imu_lthigh = Quaternionf(w, x, y, z);
		t_lthigh_last = std::chrono::high_resolution_clock::now();
		lthigh_available = true;
		break;
	case RTHIGH:
		imu_rthigh = Quaternionf(w, x, y, z);
		t_rthigh_last = std::chrono::high_resolution_clock::now();
		rthigh_available = true;
		break;
	case WAIST:
		imu_waist = Quaternionf(w, x, y, z);
		t_waist_last = std::chrono::high_resolution_clock::now();
		waist_available = true;
		break;
	case CHEST:
		imu_chest = Quaternionf(w, x, y, z);
		t_chest_last = std::chrono::high_resolution_clock::now();
		chest_available = true;
		break;
	case LSHOULDER:
		imu_lshoulder = Quaternionf(w, x, y, z);
		t_lshoulder_last = std::chrono::high_resolution_clock::now();
		lshoulder_available = true;
		break;
	case RSHOULDER:
		imu_rshoulder = Quaternionf(w, x, y, z);
		t_rshoulder_last = std::chrono::high_resolution_clock::now();
		rshoulder_available = true;
		break;
	case LUPPERARM:
		imu_lupperarm = Quaternionf(w, x, y, z);
		t_lupperarm_last = std::chrono::high_resolution_clock::now();
		lupperarm_available = true;
		break;
	case RUPPERARM:
		imu_rupperarm = Quaternionf(w, x, y, z);
		t_rupperarm_last = std::chrono::high_resolution_clock::now();
		rupperarm_available = true;
		break;
	}
}

void ImuUDP::start()
{
	while (SocketActivated) {
		memset(buff, 0, sizeof(buff));
		bytes_read = recvfrom(socketS, buff, sizeof(buff), 0, (sockaddr*)&local, &locallen);

		if (bytes_read == sizeof(PayloadExt)) {
			payloadext = (PayloadExt*)buff;
			if (payloadext->header == (uint8_t)'I' && payloadext->footer == (uint8_t)'i') {
				setValue(payloadext->id, map(payloadext->x), map(payloadext->y), map(payloadext->z), map(payloadext->w));
				setValue(payloadext->id_ext, map(payloadext->x_ext), map(payloadext->y_ext), map(payloadext->z_ext), map(payloadext->w_ext));
			}
		}

		else if (bytes_read == sizeof(Payload)) {
			payload = (Payload*)buff;
			if (payload->header == (uint8_t)'I' && payload->footer == (uint8_t)'i') {
				setValue(payload->id, map(payload->x), map(payload->y), map(payload->z), map(payload->w));
			}
		}

		else if (bytes_read == sizeof(InitSettings)) {
			init_settings = (InitSettings*)buff;
			if (init_settings->header == (uint8_t)'I' && init_settings->footer == (uint8_t)'i') {
				feet_en = init_settings->feet_en;
				shin_en = init_settings->shin_en;
				thigh_en = init_settings->thigh_en;
				waist_en = init_settings->waist_en;
				chest_en = init_settings->chest_en;
				shoulder_en = init_settings->shoulder_en;
				upperarm_en = init_settings->upperarm_en;
				init_recv = true;
				initialized = true;
			}
		}

		else if (bytes_read == sizeof(OffsetSettings)) {
			offset_settings = (OffsetSettings*)buff;
			if (offset_settings->header == (uint8_t)'I' && offset_settings->footer == (uint8_t)'i') {
				Vector2f lfoot_offset(offset_settings->lfoot_1, offset_settings->lfoot_2);
				Vector2f rfoot_offset(offset_settings->rfoot_1, offset_settings->rfoot_2);
				Vector2f lshin_offset(offset_settings->lshin_1, offset_settings->lshin_2);
				Vector2f rshin_offset(offset_settings->rshin_1, offset_settings->rshin_2);
				Vector2f lthigh_offset(offset_settings->lthigh_1, offset_settings->lthigh_2);
				Vector2f rthigh_offset(offset_settings->rthigh_1, offset_settings->rthigh_2);
				Vector2f waist_offset(offset_settings->waist_1, offset_settings->waist_2);
				Vector2f chest_offset(offset_settings->chest_1, offset_settings->chest_2);
				Vector2f lshoulder_offset(offset_settings->lshoulder_1, offset_settings->lshoulder_2);
				Vector2f rshoulder_offset(offset_settings->rshoulder_1, offset_settings->rshoulder_2);
				Vector2f lupperarm_offset(offset_settings->lupperarm_1, offset_settings->lupperarm_2);
				Vector2f rupperarm_offset(offset_settings->rupperarm_1, offset_settings->rupperarm_2);
				Vector2f head_offset(offset_settings->head_1, offset_settings->head_2);
				bk->setSensorOffset(lfoot_offset, rfoot_offset, lshin_offset, rshin_offset, lthigh_offset, rthigh_offset, waist_offset, chest_offset, lshoulder_offset, rshoulder_offset, lupperarm_offset, rupperarm_offset, head_offset);
			}
		}

		else if (bytes_read == sizeof(PayloadSettings)) {
			payload_settings = (PayloadSettings*)buff;
			if (payload_settings->header == (uint8_t)'I' && payload_settings->footer == (uint8_t)'i') {
				Quaternionf T_lfoot = bk->euXYZ_to_quat(payload_settings->lfoot_x, payload_settings->lfoot_y, payload_settings->lfoot_z);
				Quaternionf T_rfoot = bk->euXYZ_to_quat(payload_settings->rfoot_x, payload_settings->rfoot_y, payload_settings->rfoot_z);
				Quaternionf T_lshin = bk->euXYZ_to_quat(payload_settings->lshin_x, payload_settings->lshin_y, payload_settings->lshin_z);
				Quaternionf T_rshin = bk->euXYZ_to_quat(payload_settings->rshin_x, payload_settings->rshin_y, payload_settings->rshin_z);
				Quaternionf T_lthigh = bk->euXYZ_to_quat(payload_settings->lthigh_x, payload_settings->lthigh_y, payload_settings->lthigh_z);
				Quaternionf T_rthigh = bk->euXYZ_to_quat(payload_settings->rthigh_x, payload_settings->rthigh_y, payload_settings->rthigh_z);
				Quaternionf T_waist = bk->euXYZ_to_quat(payload_settings->waist_x, payload_settings->waist_y, payload_settings->waist_z);
				Quaternionf T_chest = bk->euXYZ_to_quat(payload_settings->chest_x, payload_settings->chest_y, payload_settings->chest_z);
				Quaternionf T_lshoulder = bk->euXYZ_to_quat(payload_settings->lshoulder_x, payload_settings->lshoulder_y, payload_settings->lshoulder_z);
				Quaternionf T_rshoulder = bk->euXYZ_to_quat(payload_settings->rshoulder_x, payload_settings->rshoulder_y, payload_settings->rshoulder_z);
				Quaternionf T_lupperarm = bk->euXYZ_to_quat(payload_settings->lupperarm_x, payload_settings->lupperarm_y, payload_settings->lupperarm_z);
				Quaternionf T_rupperarm = bk->euXYZ_to_quat(payload_settings->rupperarm_x, payload_settings->rupperarm_y, payload_settings->rupperarm_z);
				bk->setSensorTransform(T_lfoot, T_rfoot, T_lshin, T_rshin, T_lthigh, T_rthigh, T_waist, T_chest, T_lshoulder, T_rshoulder, T_lupperarm, T_rupperarm);
				bk->setParam(payload_settings->shin,
					payload_settings->thigh,
					payload_settings->lback,
					payload_settings->uback,
					payload_settings->head,
					payload_settings->shoulder,
					payload_settings->hip_width,
					payload_settings->shoulder_width,
					payload_settings->foot_sensor,
					payload_settings->shin_sensor,
					payload_settings->thigh_sensor,
					payload_settings->waist_sensor,
					payload_settings->chest_sensor,
					payload_settings->shoulder_sensor,
					payload_settings->upperarm_sensor,
					payload_settings->floor_offset);
				override_feet = payload_settings->override_feet;
			}
		}

		else if (bytes_read == sizeof(Calibrate)) {
			calibrate = (Calibrate*)buff;
			if (calibrate->header == (uint8_t)'I' && calibrate->footer == (uint8_t)'i') {
				if (calibrate->calib == 51) {
					float yaw = atan2f(-mat_hmd(2, 0), mat_hmd(0, 0));
					Quaternionf direction = bk->euYXZ_to_quat(yaw, 0, 0);
					bk->setOffset(imu_lfoot, imu_rfoot, imu_lshin, imu_rshin, imu_lthigh, imu_rthigh, imu_waist, imu_chest, imu_lshoulder, imu_rshoulder, imu_lupperarm, imu_rupperarm, direction);
				}
			}
		}

		t_end = std::chrono::high_resolution_clock::now();

		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_server_last).count();
		if (elapsed_time_ms >= 1000) {
			if (!initialized) {
				ping.header = (uint8_t)'I';
				ping.msg = 97;
				ping.driverPort = driverPort;
				ping.footer = (uint8_t)'i';
			}
			else {
				ping.header = (uint8_t)'I';
				ping.msg = 87;
				ping.driverPort = driverPort;
				ping.footer = (uint8_t)'i';
			}
			sendto(socketS, (char*)&ping, sizeof(ping), 0, (sockaddr*)&localT, locallenT);
			t_server_last = std::chrono::high_resolution_clock::now();
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