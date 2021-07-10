#pragma once

#include <kinematics.h>
#include <thread>
#include <chrono>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define LSHIN 1
#define RSHIN 2
#define LTHIGH 3
#define RTHIGH 4
#define WAIST 5
#define CHEST 6

#define DRIVER_VERSION 0.2

extern bool SocketActivated;

extern Quaternionf imu_lshin;
extern Quaternionf imu_rshin;
extern Quaternionf imu_lthigh;
extern Quaternionf imu_rthigh;
extern Quaternionf imu_waist;
extern Quaternionf imu_chest;

extern bool lshin_available;
extern bool rshin_available;
extern bool lthigh_available;
extern bool rthigh_available;
extern bool waist_available;
extern bool chest_available;
extern bool hmd_available;

extern bool chest_enable;

extern BodyKinematics* bk;
extern Matrix3f mat_hmd;
extern Vector3f p_hmd;