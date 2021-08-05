#include <DeviceProvider.h>

using namespace vr;

bool SocketActivated = false;

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

bool feet_enable = false;

BodyKinematics* bk = new BodyKinematics();
Matrix3f mat_hmd = Matrix3f::Identity(3, 3);
Vector3f p_hmd(0, 0, 0);

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

	m_pTracker1 = new TrackerDriver();
	m_pTracker1->SetTrackerIndex(LFOOT);
	m_pTracker1->SetModel("lfoot");
	m_pTracker1->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("lfoot", TrackedDeviceClass_GenericTracker, m_pTracker1);

	m_pTracker2 = new TrackerDriver();
	m_pTracker2->SetTrackerIndex(RFOOT);
	m_pTracker2->SetModel("rfoot");
	m_pTracker2->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("rfoot", TrackedDeviceClass_GenericTracker, m_pTracker2);

	m_pTracker3 = new TrackerDriver();
	m_pTracker3->SetTrackerIndex(WAIST);
	m_pTracker3->SetModel("waist");
	m_pTracker3->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("waist", TrackedDeviceClass_GenericTracker, m_pTracker3);

	m_pTracker4 = new TrackerDriver();
	m_pTracker4->SetTrackerIndex(LSHIN);
	m_pTracker4->SetModel("lshin");
	m_pTracker4->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("lshin", TrackedDeviceClass_GenericTracker, m_pTracker4);

	m_pTracker5 = new TrackerDriver();
	m_pTracker5->SetTrackerIndex(RSHIN);
	m_pTracker5->SetModel("rshin");
	m_pTracker5->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("rshin", TrackedDeviceClass_GenericTracker, m_pTracker5);

	m_pTracker6 = new TrackerDriver();
	m_pTracker6->SetTrackerIndex(LTHIGH);
	m_pTracker6->SetModel("lthigh");
	m_pTracker6->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("lthigh", TrackedDeviceClass_GenericTracker, m_pTracker6);

	m_pTracker7 = new TrackerDriver();
	m_pTracker7->SetTrackerIndex(RTHIGH);
	m_pTracker7->SetModel("rthigh");
	m_pTracker7->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("rthigh", TrackedDeviceClass_GenericTracker, m_pTracker7);

	m_pTracker8 = new TrackerDriver();
	m_pTracker8->SetTrackerIndex(CHEST);
	m_pTracker8->SetModel("chest");
	m_pTracker8->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("chest", TrackedDeviceClass_GenericTracker, m_pTracker8);

	m_pTracker9 = new TrackerDriver();
	m_pTracker9->SetTrackerIndex(LSHOULDER);
	m_pTracker9->SetModel("lshoulder");
	m_pTracker9->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("lshoulder", TrackedDeviceClass_GenericTracker, m_pTracker9);

	m_pTracker10 = new TrackerDriver();
	m_pTracker10->SetTrackerIndex(RSHOULDER);
	m_pTracker10->SetModel("rshoulder");
	m_pTracker10->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("rshoulder", TrackedDeviceClass_GenericTracker, m_pTracker10);

	m_pTracker11 = new TrackerDriver();
	m_pTracker11->SetTrackerIndex(LUPPERARM);
	m_pTracker11->SetModel("lupperarm");
	m_pTracker11->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("lupperarm", TrackedDeviceClass_GenericTracker, m_pTracker11);

	m_pTracker12 = new TrackerDriver();
	m_pTracker12->SetTrackerIndex(RUPPERARM);
	m_pTracker12->SetModel("rupperarm");
	m_pTracker12->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("rupperarm", TrackedDeviceClass_GenericTracker, m_pTracker12);

	// Transform from IMU ENU frame (X right, Y front, Z up) to driver frame (X right, Y up, Z back)
	bk->setDriverTransform(bk->euXYZ_to_quat(90.0 * PI / 180.0, 0, 0)); // Rotate X 90 deg

	udpThread = new ImuUDP();
	udpThread->init();

	kinematicsRun = true;
	kinematicsThread = new std::thread(&DeviceProvider::forwardKinematics, this);

	return VRInitError_None;
}

void DeviceProvider::Cleanup()
{
	udpThread->deinit();
	delete udpThread;
	udpThread = NULL;

	kinematicsRun = false;
	kinematicsThread->join();
	delete kinematicsThread;
	kinematicsThread = NULL;

	delete m_pTracker1;
	m_pTracker1 = NULL;
	delete m_pTracker2;
	m_pTracker2 = NULL;
	delete m_pTracker3;
	m_pTracker3 = NULL;
	delete m_pTracker4;
	m_pTracker4 = NULL;
	delete m_pTracker5;
	m_pTracker5 = NULL;
	delete m_pTracker6;
	m_pTracker6 = NULL;
	delete m_pTracker7;
	m_pTracker7 = NULL;
	delete m_pTracker8;
	m_pTracker8 = NULL;
	delete m_pTracker9;
	m_pTracker9 = NULL;
	delete m_pTracker10;
	m_pTracker10 = NULL;
	delete m_pTracker11;
	m_pTracker11 = NULL;
	delete m_pTracker12;
	m_pTracker12 = NULL;
}

const char* const* DeviceProvider::GetInterfaceVersions()
{
	return k_InterfaceVersions;
}

void DeviceProvider::RunFrame()
{
	if (m_pTracker1)
	{
		m_pTracker1->RunFrame();
	}
	if (m_pTracker2)
	{
		m_pTracker2->RunFrame();
	}
	if (m_pTracker3)
	{
		m_pTracker3->RunFrame();
	}
	if (m_pTracker4)
	{
		m_pTracker4->RunFrame();
	}
	if (m_pTracker5)
	{
		m_pTracker5->RunFrame();
	}
	if (m_pTracker6)
	{
		m_pTracker6->RunFrame();
	}
	if (m_pTracker7)
	{
		m_pTracker7->RunFrame();
	}
	if (m_pTracker8)
	{
		m_pTracker8->RunFrame();
	}
	if (m_pTracker9)
	{
		m_pTracker9->RunFrame();
	}
	if (m_pTracker10)
	{
		m_pTracker10->RunFrame();
	}
	if (m_pTracker11)
	{
		m_pTracker11->RunFrame();
	}
	if (m_pTracker12)
	{
		m_pTracker12->RunFrame();
	}
}

bool DeviceProvider::ShouldBlockStandbyMode()
{
	return false;
}

void DeviceProvider::EnterStandby() 
{
}

void DeviceProvider::LeaveStandby()
{
}

void DeviceProvider::forwardKinematics() {
	while (kinematicsRun) {
		TrackedDevicePose_t trackedDevicePoses[k_unMaxTrackedDeviceCount];
		VRServerDriverHost()->GetRawTrackedDevicePoses(0, trackedDevicePoses, k_unMaxTrackedDeviceCount);

		if (trackedDevicePoses[k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
			HmdMatrix34_t hmdMatrix = trackedDevicePoses[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					mat_hmd(i, j) = hmdMatrix.m[i][j];
				}
			}
			for (int k = 0; k < 3; k++) {
				p_hmd(k) = hmdMatrix.m[k][3];
			}
			hmd_available = true;
			t_hmd_last = std::chrono::high_resolution_clock::now();
		}

		bk->update(imu_lfoot, imu_rfoot, imu_lshin, imu_rshin, imu_lthigh, imu_rthigh, imu_waist, imu_chest, imu_lshoulder, imu_rshoulder, imu_lupperarm, imu_rupperarm, Quaternionf(mat_hmd), p_hmd);

		t_end = std::chrono::high_resolution_clock::now();
		elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_hmd_last).count();
		if (elapsed_time_ms >= 1000) {
			hmd_available = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}