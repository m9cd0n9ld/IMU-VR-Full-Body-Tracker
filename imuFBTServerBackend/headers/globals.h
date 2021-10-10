#pragma once

#include <openvr.h>
#include <Eigen/Dense>
#include <kinematics.h>
#include <thread>
#include <chrono>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))

#define TO_DEBUG false
#define DEBUG if (TO_DEBUG) printf

#define TO_DEBUG_FAST false
#define DEBUGFAST if (TO_DEBUG_FAST) printf

#define LFOOT 1
#define RFOOT 2
#define LSHIN 3
#define RSHIN 4
#define LTHIGH 5
#define RTHIGH 6
#define WAIST 7
#define CHEST 8
#define LSHOULDER 9
#define RSHOULDER 10
#define LUPPERARM 11
#define RUPPERARM 12


extern Quaternionf imu_lfoot;
extern Quaternionf imu_rfoot;
extern Quaternionf imu_lshin;
extern Quaternionf imu_rshin;
extern Quaternionf imu_lthigh;
extern Quaternionf imu_rthigh;
extern Quaternionf imu_waist;
extern Quaternionf imu_chest;
extern Quaternionf imu_lshoulder;
extern Quaternionf imu_rshoulder;
extern Quaternionf imu_lupperarm;
extern Quaternionf imu_rupperarm;

extern bool lfoot_available;
extern bool rfoot_available;
extern bool lshin_available;
extern bool rshin_available;
extern bool lthigh_available;
extern bool rthigh_available;
extern bool waist_available;
extern bool chest_available;
extern bool lshoulder_available;
extern bool rshoulder_available;
extern bool lupperarm_available;
extern bool rupperarm_available;
extern bool hmd_available;

extern bool init_recv;
extern bool feet_en;
extern bool shin_en;
extern bool thigh_en;
extern bool waist_en;
extern bool chest_en;
extern bool shoulder_en;
extern bool upperarm_en;

extern bool override_feet;

extern BodyKinematics* bk;
extern Matrix3f mat_hmd;
extern Vector3f p_hmd;

extern bool stream_viewer_data;