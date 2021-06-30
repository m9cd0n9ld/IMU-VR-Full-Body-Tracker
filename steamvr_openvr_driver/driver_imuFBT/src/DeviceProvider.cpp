#include <DeviceProvider.h>

using namespace vr;

bool SocketActivated = false;

Quaternionf imu_lshin(1, 0, 0, 0);
Quaternionf imu_rshin(1, 0, 0, 0);
Quaternionf imu_lthigh(1, 0, 0, 0);
Quaternionf imu_rthigh(1, 0, 0, 0);
Quaternionf imu_waist(1, 0, 0, 0);
Quaternionf imu_chest(1, 0, 0, 0);

bool lshin_available = false;
bool rshin_available = false;
bool lthigh_available = false;
bool rthigh_available = false;
bool waist_available = false;
bool chest_available = false;
bool hmd_available = false;

bool chest_enable = false;

BodyKinematics* bk = new BodyKinematics();
Matrix3f mat_hmd = Matrix3f::Identity(3, 3);
Vector3f p_hmd(0, 0, 0);

ImuUDP* udpThread = new ImuUDP();

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

	m_pTracker1 = new TrackerDriver();
	m_pTracker1->SetTrackerIndex(LSHIN);
	m_pTracker1->SetModel("lfoot");
	m_pTracker1->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("lfoot", TrackedDeviceClass_GenericTracker, m_pTracker1);

	m_pTracker2 = new TrackerDriver();
	m_pTracker2->SetTrackerIndex(RSHIN);
	m_pTracker2->SetModel("rfoot");
	m_pTracker2->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("rfoot", TrackedDeviceClass_GenericTracker, m_pTracker2);

	m_pTracker3 = new TrackerDriver();
	m_pTracker3->SetTrackerIndex(WAIST);
	m_pTracker3->SetModel("waist");
	m_pTracker3->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("waist", TrackedDeviceClass_GenericTracker, m_pTracker3);

	m_pTracker4 = new TrackerDriver();
	m_pTracker4->SetTrackerIndex(LTHIGH);
	m_pTracker4->SetModel("lknee");
	m_pTracker4->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("lknee", TrackedDeviceClass_GenericTracker, m_pTracker4);

	m_pTracker5 = new TrackerDriver();
	m_pTracker5->SetTrackerIndex(RTHIGH);
	m_pTracker5->SetModel("rknee");
	m_pTracker5->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("rknee", TrackedDeviceClass_GenericTracker, m_pTracker5);

	m_pTracker6 = new TrackerDriver();
	m_pTracker6->SetTrackerIndex(CHEST);
	m_pTracker6->SetModel("chest");
	m_pTracker6->SetVersion(std::to_string(DRIVER_VERSION));
	VRServerDriverHost()->TrackedDeviceAdded("chest", TrackedDeviceClass_GenericTracker, m_pTracker6);

	// Transform from IMU ENU frame (X right, Y front, Z up) to driver frame (X right, Y up, Z back)
	bk->setDriverTransform(bk->euXYZ_to_quat(90.0 * PI / 180.0, 0, 0)); // Rotate X 90 deg

	udpThread->init();

	return VRInitError_None;
}

void DeviceProvider::Cleanup()
{
	udpThread->deinit();
	delete udpThread;
	udpThread = NULL;

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
}

const char* const* DeviceProvider::GetInterfaceVersions()
{
	return k_InterfaceVersions;
}

void DeviceProvider::RunFrame()
{
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
		bk->update(imu_lshin, imu_rshin, imu_lthigh, imu_rthigh, imu_waist, imu_chest, Quaternionf(mat_hmd), p_hmd);
		hmd_available = true;
	}
	else {
		hmd_available = false;
	}

	VREvent_t vrEvent;
	while (VRServerDriverHost()->PollNextEvent(&vrEvent, sizeof(vrEvent)))
	{
		switch (vrEvent.eventType) {
		case VREvent_StandingZeroPoseReset:
			float yaw = atan2f(-mat_hmd(2, 0), mat_hmd(0, 0));
			Quaternionf direction = bk->euYXZ_to_quat(yaw, 0, 0);
			bk->setOffset(imu_lshin, imu_rshin, imu_lthigh, imu_rthigh, imu_waist, imu_chest, direction);
		}
	}

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