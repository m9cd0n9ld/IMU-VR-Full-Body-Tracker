#pragma once

#include <thread>
#include <chrono>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

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

#define DRIVER_VERSION 0.7

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))

extern bool SocketActivated;
extern bool init_recv;

extern bool feet_en;
extern bool shin_en;
extern bool thigh_en;
extern bool waist_en;
extern bool chest_en;
extern bool shoulder_en;
extern bool upperarm_en;

extern bool hmd_available;

extern bool lfoot_available;
extern float Px_lfoot;
extern float Py_lfoot;
extern float Pz_lfoot;
extern int16_t Qx_lfoot;
extern int16_t Qy_lfoot;
extern int16_t Qz_lfoot;
extern int16_t Qw_lfoot;

extern bool rfoot_available;
extern float Px_rfoot;
extern float Py_rfoot;
extern float Pz_rfoot;
extern int16_t Qx_rfoot;
extern int16_t Qy_rfoot;
extern int16_t Qz_rfoot;
extern int16_t Qw_rfoot;

extern bool lshin_available;
extern float Px_lshin;
extern float Py_lshin;
extern float Pz_lshin;
extern int16_t Qx_lshin;
extern int16_t Qy_lshin;
extern int16_t Qz_lshin;
extern int16_t Qw_lshin;

extern bool rshin_available;
extern float Px_rshin;
extern float Py_rshin;
extern float Pz_rshin;
extern int16_t Qx_rshin;
extern int16_t Qy_rshin;
extern int16_t Qz_rshin;
extern int16_t Qw_rshin;

extern bool lthigh_available;
extern float Px_lthigh;
extern float Py_lthigh;
extern float Pz_lthigh;
extern int16_t Qx_lthigh;
extern int16_t Qy_lthigh;
extern int16_t Qz_lthigh;
extern int16_t Qw_lthigh;

extern bool rthigh_available;
extern float Px_rthigh;
extern float Py_rthigh;
extern float Pz_rthigh;
extern int16_t Qx_rthigh;
extern int16_t Qy_rthigh;
extern int16_t Qz_rthigh;
extern int16_t Qw_rthigh;

extern bool waist_available;
extern float Px_waist;
extern float Py_waist;
extern float Pz_waist;
extern int16_t Qx_waist;
extern int16_t Qy_waist;
extern int16_t Qz_waist;
extern int16_t Qw_waist;

extern bool chest_available;
extern float Px_chest;
extern float Py_chest;
extern float Pz_chest;
extern int16_t Qx_chest;
extern int16_t Qy_chest;
extern int16_t Qz_chest;
extern int16_t Qw_chest;

extern bool lshoulder_available;
extern float Px_lshoulder;
extern float Py_lshoulder;
extern float Pz_lshoulder;
extern int16_t Qx_lshoulder;
extern int16_t Qy_lshoulder;
extern int16_t Qz_lshoulder;
extern int16_t Qw_lshoulder;

extern bool rshoulder_available;
extern float Px_rshoulder;
extern float Py_rshoulder;
extern float Pz_rshoulder;
extern int16_t Qx_rshoulder;
extern int16_t Qy_rshoulder;
extern int16_t Qz_rshoulder;
extern int16_t Qw_rshoulder;

extern bool lupperarm_available;
extern float Px_lupperarm;
extern float Py_lupperarm;
extern float Pz_lupperarm;
extern int16_t Qx_lupperarm;
extern int16_t Qy_lupperarm;
extern int16_t Qz_lupperarm;
extern int16_t Qw_lupperarm;

extern bool rupperarm_available;
extern float Px_rupperarm;
extern float Py_rupperarm;
extern float Pz_rupperarm;
extern int16_t Qx_rupperarm;
extern int16_t Qy_rupperarm;
extern int16_t Qz_rupperarm;
extern int16_t Qw_rupperarm;