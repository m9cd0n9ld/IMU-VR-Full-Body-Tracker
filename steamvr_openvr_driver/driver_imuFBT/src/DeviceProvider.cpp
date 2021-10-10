#include <DeviceProvider.h>

using namespace vr;

bool SocketActivated = false;
bool init_recv = false;

bool feet_en = false;
bool shin_en = false;
bool thigh_en = false;
bool waist_en = false;
bool chest_en = false;
bool shoulder_en = false;
bool upperarm_en = false;

bool hmd_available = false;

bool lfoot_available = false;
float Px_lfoot = 0;
float Py_lfoot = 0;
float Pz_lfoot = 0;
int16_t Qx_lfoot = 0;
int16_t Qy_lfoot = 0;
int16_t Qz_lfoot = 0;
int16_t Qw_lfoot = 32767;

bool rfoot_available = false;
float Px_rfoot = 0;
float Py_rfoot = 0;
float Pz_rfoot = 0;
int16_t Qx_rfoot = 0;
int16_t Qy_rfoot = 0;
int16_t Qz_rfoot = 0;
int16_t Qw_rfoot = 32767;

bool lshin_available = false;
float Px_lshin = 0;
float Py_lshin = 0;
float Pz_lshin = 0;
int16_t Qx_lshin = 0;
int16_t Qy_lshin = 0;
int16_t Qz_lshin = 0;
int16_t Qw_lshin = 32767;

bool rshin_available = false;
float Px_rshin = 0;
float Py_rshin = 0;
float Pz_rshin = 0;
int16_t Qx_rshin = 0;
int16_t Qy_rshin = 0;
int16_t Qz_rshin = 0;
int16_t Qw_rshin = 32767;

bool lthigh_available = false;
float Px_lthigh = 0;
float Py_lthigh = 0;
float Pz_lthigh = 0;
int16_t Qx_lthigh = 0;
int16_t Qy_lthigh = 0;
int16_t Qz_lthigh = 0;
int16_t Qw_lthigh = 32767;

bool rthigh_available = false;
float Px_rthigh = 0;
float Py_rthigh = 0;
float Pz_rthigh = 0;
int16_t Qx_rthigh = 0;
int16_t Qy_rthigh = 0;
int16_t Qz_rthigh = 0;
int16_t Qw_rthigh = 32767;

bool waist_available = false;
float Px_waist = 0;
float Py_waist = 0;
float Pz_waist = 0;
int16_t Qx_waist = 0;
int16_t Qy_waist = 0;
int16_t Qz_waist = 0;
int16_t Qw_waist = 32767;

bool chest_available = false;
float Px_chest = 0;
float Py_chest = 0;
float Pz_chest = 0;
int16_t Qx_chest = 0;
int16_t Qy_chest = 0;
int16_t Qz_chest = 0;
int16_t Qw_chest = 32767;

bool lshoulder_available = false;
float Px_lshoulder = 0;
float Py_lshoulder = 0;
float Pz_lshoulder = 0;
int16_t Qx_lshoulder = 0;
int16_t Qy_lshoulder = 0;
int16_t Qz_lshoulder = 0;
int16_t Qw_lshoulder = 32767;

bool rshoulder_available = false;
float Px_rshoulder = 0;
float Py_rshoulder = 0;
float Pz_rshoulder = 0;
int16_t Qx_rshoulder = 0;
int16_t Qy_rshoulder = 0;
int16_t Qz_rshoulder = 0;
int16_t Qw_rshoulder = 32767;

bool lupperarm_available = false;
float Px_lupperarm = 0;
float Py_lupperarm = 0;
float Pz_lupperarm = 0;
int16_t Qx_lupperarm = 0;
int16_t Qy_lupperarm = 0;
int16_t Qz_lupperarm = 0;
int16_t Qw_lupperarm = 32767;

bool rupperarm_available = false;
float Px_rupperarm = 0;
float Py_rupperarm = 0;
float Pz_rupperarm = 0;
int16_t Qx_rupperarm = 0;
int16_t Qy_rupperarm = 0;
int16_t Qz_rupperarm = 0;
int16_t Qw_rupperarm = 32767;

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

	udpThread = new UDP();
	udpThread->init();

	initRun = true;
	initThread = new std::thread(&DeviceProvider::initLoop, this);

	return VRInitError_None;
}

void DeviceProvider::Cleanup()
{
	udpThread->deinit();
	delete udpThread;
	udpThread = NULL;

	if (initRun) {
		initRun = false;
		initThread->join();
		delete initThread;
		initThread = NULL;
	}

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

void DeviceProvider::initDevice() {
	if (feet_en) {
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
	}
	
	if (waist_en) {
		m_pTracker3 = new TrackerDriver();
		m_pTracker3->SetTrackerIndex(WAIST);
		m_pTracker3->SetModel("waist");
		m_pTracker3->SetVersion(std::to_string(DRIVER_VERSION));
		VRServerDriverHost()->TrackedDeviceAdded("waist", TrackedDeviceClass_GenericTracker, m_pTracker3);
	}
	
	if (shin_en) {
		m_pTracker4 = new TrackerDriver();
		m_pTracker4->SetTrackerIndex(LSHIN);
		m_pTracker4->SetModel("llowerleg");
		m_pTracker4->SetVersion(std::to_string(DRIVER_VERSION));
		VRServerDriverHost()->TrackedDeviceAdded("llowerleg", TrackedDeviceClass_GenericTracker, m_pTracker4);

		m_pTracker5 = new TrackerDriver();
		m_pTracker5->SetTrackerIndex(RSHIN);
		m_pTracker5->SetModel("rlowerleg");
		m_pTracker5->SetVersion(std::to_string(DRIVER_VERSION));
		VRServerDriverHost()->TrackedDeviceAdded("rlowerleg", TrackedDeviceClass_GenericTracker, m_pTracker5);
	}
	
	if (thigh_en) {
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
	}
	
	if (chest_en) {
		m_pTracker8 = new TrackerDriver();
		m_pTracker8->SetTrackerIndex(CHEST);
		m_pTracker8->SetModel("chest");
		m_pTracker8->SetVersion(std::to_string(DRIVER_VERSION));
		VRServerDriverHost()->TrackedDeviceAdded("chest", TrackedDeviceClass_GenericTracker, m_pTracker8);
	}
	
	if (shoulder_en) {
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
	}
	
	if (upperarm_en) {
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
	}
}

void DeviceProvider::initLoop() {
	while (initRun) {
		if (init_recv) {
			initDevice();
			initRun = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}