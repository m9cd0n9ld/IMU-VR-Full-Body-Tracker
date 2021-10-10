#include <globals.h>
#include <serverudp.h>
#include <driverudp.h>
#include <driverstruct.h>

using namespace vr;
using namespace Eigen;

#define TARGET_RESOLUTION_MS 1
#define TARGET_INTERVAL_MS 5


Quaternionf imu_lfoot(1, 0, 0, 0);
Quaternionf imu_rfoot(1, 0, 0, 0);
Quaternionf imu_lshin(1, 0, 0, 0);
Quaternionf imu_rshin(1, 0, 0, 0);
Quaternionf imu_lthigh(1, 0, 0, 0);
Quaternionf imu_rthigh(1, 0, 0, 0);
Quaternionf imu_waist(1, 0, 0, 0);
Quaternionf imu_chest(1, 0, 0, 0);
Quaternionf imu_lshoulder(1, 0, 0, 0);
Quaternionf imu_rshoulder(1, 0, 0, 0);
Quaternionf imu_lupperarm(1, 0, 0, 0);
Quaternionf imu_rupperarm(1, 0, 0, 0);

bool lfoot_available = false;
bool rfoot_available = false;
bool lshin_available = false;
bool rshin_available = false;
bool lthigh_available = false;
bool rthigh_available = false;
bool waist_available = false;
bool chest_available = false;
bool lshoulder_available = false;
bool rshoulder_available = false;
bool lupperarm_available = false;
bool rupperarm_available = false;
bool hmd_available = false;

bool init_recv = false;
bool feet_en = false;
bool shin_en = false;
bool thigh_en = false;
bool waist_en = false;
bool chest_en = false;
bool shoulder_en = false;
bool upperarm_en = false;

bool override_feet = false;

BodyKinematics* bk = new BodyKinematics();
Matrix3f mat_hmd = Matrix3f::Identity(3, 3);
Vector3f p_hmd(0, 0, 0);

bool stream_viewer_data = false;


std::chrono::high_resolution_clock::time_point t_hmd_end = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point t_hmd_last = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point t_viewer_last = std::chrono::high_resolution_clock::now();
double hmd_elapsed_time_ms;


IVRSystem* sys;

TIMECAPS tc;
UINT wTimerRes;
MMRESULT mmresult;
MMRESULT timer_id;

serverudp* udpS = new serverudp();
driverudp* udpD = new driverudp();

bool loop = false;

DriverPayload driverpayload;

void sendPayload(int i, bool viewer=false) {

	driverpayload.header = (uint8_t)'I';
	driverpayload.footer = (uint8_t)'i';

	driverpayload.hmd_available = hmd_available;

	switch (i) {

	case LFOOT:
		driverpayload.id = LFOOT;

		if (!override_feet) {
			driverpayload.available = lfoot_available;
			driverpayload.Px = bk->P_lfoot.x();
			driverpayload.Py = bk->P_lfoot.y();
			driverpayload.Pz = bk->P_lfoot.z();
			driverpayload.Qx = map(bk->Q_lfoot.x());
			driverpayload.Qy = map(bk->Q_lfoot.y());
			driverpayload.Qz = map(bk->Q_lfoot.z());
			driverpayload.Qw = map(bk->Q_lfoot.w());
		}
		else {
			driverpayload.available = lshin_available;
			driverpayload.Px = bk->P_lshin.x();
			driverpayload.Py = bk->P_lshin.y();
			driverpayload.Pz = bk->P_lshin.z();
			driverpayload.Qx = map(bk->Q_lshin.x());
			driverpayload.Qy = map(bk->Q_lshin.y());
			driverpayload.Qz = map(bk->Q_lshin.z());
			driverpayload.Qw = map(bk->Q_lshin.w());
		}
		
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (feet_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case RFOOT:
		driverpayload.id = RFOOT;

		if (!override_feet) {
			driverpayload.available = rfoot_available;
			driverpayload.Px = bk->P_rfoot.x();
			driverpayload.Py = bk->P_rfoot.y();
			driverpayload.Pz = bk->P_rfoot.z();
			driverpayload.Qx = map(bk->Q_rfoot.x());
			driverpayload.Qy = map(bk->Q_rfoot.y());
			driverpayload.Qz = map(bk->Q_rfoot.z());
			driverpayload.Qw = map(bk->Q_rfoot.w());
		}
		else {
			driverpayload.available = rshin_available;
			driverpayload.Px = bk->P_rshin.x();
			driverpayload.Py = bk->P_rshin.y();
			driverpayload.Pz = bk->P_rshin.z();
			driverpayload.Qx = map(bk->Q_rshin.x());
			driverpayload.Qy = map(bk->Q_rshin.y());
			driverpayload.Qz = map(bk->Q_rshin.z());
			driverpayload.Qw = map(bk->Q_rshin.w());
		}
		
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (feet_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case LSHIN:
		driverpayload.id = LSHIN;
		driverpayload.available = lshin_available;
		driverpayload.Px = bk->P_lshin.x();
		driverpayload.Py = bk->P_lshin.y();
		driverpayload.Pz = bk->P_lshin.z();
		driverpayload.Qx = map(bk->Q_lshin.x());
		driverpayload.Qy = map(bk->Q_lshin.y());
		driverpayload.Qz = map(bk->Q_lshin.z());
		driverpayload.Qw = map(bk->Q_lshin.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (shin_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case RSHIN:
		driverpayload.id = RSHIN;
		driverpayload.available = rshin_available;
		driverpayload.Px = bk->P_rshin.x();
		driverpayload.Py = bk->P_rshin.y();
		driverpayload.Pz = bk->P_rshin.z();
		driverpayload.Qx = map(bk->Q_rshin.x());
		driverpayload.Qy = map(bk->Q_rshin.y());
		driverpayload.Qz = map(bk->Q_rshin.z());
		driverpayload.Qw = map(bk->Q_rshin.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (shin_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case LTHIGH:
		driverpayload.id = LTHIGH;
		driverpayload.available = lthigh_available;
		driverpayload.Px = bk->P_lthigh.x();
		driverpayload.Py = bk->P_lthigh.y();
		driverpayload.Pz = bk->P_lthigh.z();
		driverpayload.Qx = map(bk->Q_lthigh.x());
		driverpayload.Qy = map(bk->Q_lthigh.y());
		driverpayload.Qz = map(bk->Q_lthigh.z());
		driverpayload.Qw = map(bk->Q_lthigh.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (thigh_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case RTHIGH:
		driverpayload.id = RTHIGH;
		driverpayload.available = rthigh_available;
		driverpayload.Px = bk->P_rthigh.x();
		driverpayload.Py = bk->P_rthigh.y();
		driverpayload.Pz = bk->P_rthigh.z();
		driverpayload.Qx = map(bk->Q_rthigh.x());
		driverpayload.Qy = map(bk->Q_rthigh.y());
		driverpayload.Qz = map(bk->Q_rthigh.z());
		driverpayload.Qw = map(bk->Q_rthigh.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (thigh_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case WAIST:
		driverpayload.id = WAIST;
		driverpayload.available = waist_available;
		driverpayload.Px = bk->P_waist.x();
		driverpayload.Py = bk->P_waist.y();
		driverpayload.Pz = bk->P_waist.z();
		driverpayload.Qx = map(bk->Q_waist.x());
		driverpayload.Qy = map(bk->Q_waist.y());
		driverpayload.Qz = map(bk->Q_waist.z());
		driverpayload.Qw = map(bk->Q_waist.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (waist_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case CHEST:
		driverpayload.id = CHEST;
		driverpayload.available = chest_available;
		driverpayload.Px = bk->P_chest.x();
		driverpayload.Py = bk->P_chest.y();
		driverpayload.Pz = bk->P_chest.z();
		driverpayload.Qx = map(bk->Q_chest.x());
		driverpayload.Qy = map(bk->Q_chest.y());
		driverpayload.Qz = map(bk->Q_chest.z());
		driverpayload.Qw = map(bk->Q_chest.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (chest_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case LSHOULDER:
		driverpayload.id = LSHOULDER;
		driverpayload.available = lshoulder_available;
		driverpayload.Px = bk->P_lshoulder.x();
		driverpayload.Py = bk->P_lshoulder.y();
		driverpayload.Pz = bk->P_lshoulder.z();
		driverpayload.Qx = map(bk->Q_lshoulder.x());
		driverpayload.Qy = map(bk->Q_lshoulder.y());
		driverpayload.Qz = map(bk->Q_lshoulder.z());
		driverpayload.Qw = map(bk->Q_lshoulder.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (shoulder_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case RSHOULDER:
		driverpayload.id = RSHOULDER;
		driverpayload.available = rshoulder_available;
		driverpayload.Px = bk->P_rshoulder.x();
		driverpayload.Py = bk->P_rshoulder.y();
		driverpayload.Pz = bk->P_rshoulder.z();
		driverpayload.Qx = map(bk->Q_rshoulder.x());
		driverpayload.Qy = map(bk->Q_rshoulder.y());
		driverpayload.Qz = map(bk->Q_rshoulder.z());
		driverpayload.Qw = map(bk->Q_rshoulder.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (shoulder_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case LUPPERARM:
		driverpayload.id = LUPPERARM;
		driverpayload.available = lupperarm_available;
		driverpayload.Px = bk->P_lupperarm.x();
		driverpayload.Py = bk->P_lupperarm.y();
		driverpayload.Pz = bk->P_lupperarm.z();
		driverpayload.Qx = map(bk->Q_lupperarm.x());
		driverpayload.Qy = map(bk->Q_lupperarm.y());
		driverpayload.Qz = map(bk->Q_lupperarm.z());
		driverpayload.Qw = map(bk->Q_lupperarm.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (upperarm_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;

	case RUPPERARM:
		driverpayload.id = RUPPERARM;
		driverpayload.available = rupperarm_available;
		driverpayload.Px = bk->P_rupperarm.x();
		driverpayload.Py = bk->P_rupperarm.y();
		driverpayload.Pz = bk->P_rupperarm.z();
		driverpayload.Qx = map(bk->Q_rupperarm.x());
		driverpayload.Qy = map(bk->Q_rupperarm.y());
		driverpayload.Qz = map(bk->Q_rupperarm.z());
		driverpayload.Qw = map(bk->Q_rupperarm.w());
		
		switch (viewer) {
		case true:
			udpS->send((char*)&driverpayload, sizeof(driverpayload));
			break;
		case false:
			if (upperarm_en) {
				udpD->send((char*)&driverpayload, sizeof(driverpayload));
			}
			break;
		}
		break;
	}
}

void CALLBACK onTimeFunc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	DEBUGFAST("Timer event called\n");
	TrackedDevicePose_t trackedDevicePoses[k_unMaxTrackedDeviceCount];
	sys->GetDeviceToAbsoluteTrackingPose(TrackingUniverseStanding, 0, trackedDevicePoses, k_unMaxTrackedDeviceCount);
	if (trackedDevicePoses[k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
		t_hmd_last = std::chrono::high_resolution_clock::now();
		hmd_available = true;
		HmdMatrix34_t hmdMatrix = trackedDevicePoses[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				mat_hmd(i, j) = hmdMatrix.m[i][j];
			}
		}
		for (int k = 0; k < 3; k++) {
			p_hmd(k) = hmdMatrix.m[k][3];
		}
	}
	t_hmd_end = std::chrono::high_resolution_clock::now();
	hmd_elapsed_time_ms = std::chrono::duration<double, std::milli>(t_hmd_end - t_hmd_last).count();
	if (hmd_elapsed_time_ms > 5000) {
		hmd_available = false;
	}
	bk->update(imu_lfoot, imu_rfoot, imu_lshin, imu_rshin, imu_lthigh, imu_rthigh, imu_waist, imu_chest, imu_lshoulder, imu_rshoulder, imu_lupperarm, imu_rupperarm, Quaternionf(mat_hmd), p_hmd);

	for (int i = 1; i < 13; i++) {
		sendPayload(i, false);
	}

	hmd_elapsed_time_ms = std::chrono::duration<double, std::milli>(t_hmd_end - t_viewer_last).count();
	if (hmd_elapsed_time_ms >= 100) {
		t_viewer_last = std::chrono::high_resolution_clock::now();
		if (stream_viewer_data) {
			for (int i = 1; i < 13; i++) {
				sendPayload(i, true);
			}
		}
	}

	VREvent_t event;
	while (sys->PollNextEvent(&event, sizeof(event))) {
		if (event.eventType == VREvent_Quit) {
			loop = false;
			sys->AcknowledgeQuit_Exiting();
			sys = nullptr;
			VR_Shutdown();
			timeKillEvent(wTimerID);
			break;
		}
	}
}

int main()
{
	// Transform from IMU ENU frame (X right, Y front, Z up) to driver frame (X right, Y up, Z back)
	bk->setDriverTransform(bk->euXYZ_to_quat(90.0 * PI / 180.0, 0, 0)); // Rotate X 90 deg

	while (true) {
		EVRInitError error;
		sys = VR_Init(&error, VRApplication_Background);
		if (error != 0) {
			DEBUG("error %s\n", VR_GetVRInitErrorAsSymbol(error));
		}
		else {
			DEBUG("SteamVR app initialized\n");
			break;
		}
		Sleep(1000);
	}
	

	timeGetDevCaps(&tc, sizeof(TIMECAPS));
	wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION_MS), tc.wPeriodMax);
	mmresult = timeBeginPeriod(wTimerRes);
	if (mmresult == TIMERR_NOERROR) {
		timer_id = timeSetEvent(TARGET_INTERVAL_MS, TARGET_RESOLUTION_MS, (LPTIMECALLBACK)onTimeFunc, DWORD(1), TIME_PERIODIC);
		if (timer_id != NULL) {
			DEBUG("Timer event initialized\n");
		}
		else {
			timeEndPeriod(wTimerRes);
			VR_Shutdown();
			DEBUG("Timer event error\n");
			return -1;
		}
	}
	else {
		VR_Shutdown();
		DEBUG("Timer error\n");
		return -1;
	}

	if (udpS->init()) {
		DEBUG("Server UDP initialized\n");
	}
	else {
		timeKillEvent(timer_id);
		timeEndPeriod(wTimerRes);
		VR_Shutdown();
		DEBUG("Server UDP failed\n");
		return -1;
	}

	if (udpD->init()) {
		DEBUG("Driver UDP initialized\n");
	}
	else {
		udpS->deinit();
		timeKillEvent(timer_id);
		timeEndPeriod(wTimerRes);
		VR_Shutdown();
		DEBUG("Driver UDP failed\n");
		return -1;
	}
	
	loop = true;
	while (loop) {
		Sleep(1000);
	}
	udpS->deinit();
	udpD->deinit();
	timeEndPeriod(wTimerRes);
	DEBUG("Shutting down\n");
	return 0;
}