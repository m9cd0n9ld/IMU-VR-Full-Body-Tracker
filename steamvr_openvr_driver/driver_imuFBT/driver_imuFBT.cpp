#include <openvr_driver.h>

#include <kinematics.h>

#include <thread>
#include <chrono>

#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#define BUFLEN 128

#define LSHIN 1
#define RSHIN 2
#define LTHIGH 3
#define RTHIGH 4
#define WAIST 5

using namespace vr;

#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C" 
#else
#error "Unsupported Platform."
#endif

PACK(struct Payload {
	uint8_t id;
	float x;
	float y;
	float z;
	float w;
});

PACK(struct PayloadSettings {
	float lshin_x;
	float lshin_y;
	float lshin_z;

	float rshin_x;
	float rshin_y;
	float rshin_z;

	float lthigh_x;
	float lthigh_y;
	float lthigh_z;

	float rthigh_x;
	float rthigh_y;
	float rthigh_z;

	float waist_x;
	float waist_y;
	float waist_z;

	float shin;
	float thigh;
	float back;
	float head;
	float hip_width;
	float shin_sensor;
	float thigh_sensor;
	float waist_sensor;
});

Payload* payload;

PayloadSettings* payload_settings;

Quaternionf imu_lshin(1, 0, 0, 0);
Quaternionf imu_rshin(1, 0, 0, 0);
Quaternionf imu_lthigh(1, 0, 0, 0);
Quaternionf imu_rthigh(1, 0, 0, 0);
Quaternionf imu_waist(1, 0, 0, 0);

bool lshin_available = false;
bool rshin_available = false;
bool lthigh_available = false;
bool rthigh_available = false;
bool waist_available = false;
bool hmd_available = false;

auto t_lshin_last = std::chrono::high_resolution_clock::now();
auto t_rshin_last = std::chrono::high_resolution_clock::now();
auto t_lthigh_last = std::chrono::high_resolution_clock::now();
auto t_rthigh_last = std::chrono::high_resolution_clock::now();
auto t_waist_last = std::chrono::high_resolution_clock::now();

auto t_end = std::chrono::high_resolution_clock::now();
double elapsed_time_ms;

SOCKET socketS;
int bytes_read;
sockaddr_in local;
int locallen = sizeof(local);
bool SocketActivated = false;
bool bKeepReading = false;
const uint16_t driverPort = 61035;

char buff[128];

std::thread* pSocketThread = NULL;

BodyKinematics* bk = new BodyKinematics();
Matrix3f mat_hmd = Matrix3f::Identity(3, 3);
Vector3f p_hmd(0, 0, 0);

void WinSockReadFunc()
{
	while (SocketActivated) {
		bKeepReading = true;
		while (bKeepReading) {
			memset(buff, 0, sizeof(buff));
			bytes_read = recvfrom(socketS, buff, sizeof(buff), 0, (sockaddr*)&local, &locallen);

			if (bytes_read == 17) {
				payload = (Payload*)buff;
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
				}
			}

			else if(bytes_read == 92) {
				payload_settings = (PayloadSettings*)buff;
				Quaternionf T_lshin = bk->euXYZ_to_quat(payload_settings->lshin_x, payload_settings->lshin_y, payload_settings->lshin_z);
				Quaternionf T_rshin = bk->euXYZ_to_quat(payload_settings->rshin_x, payload_settings->rshin_y, payload_settings->rshin_z);
				Quaternionf T_lthigh = bk->euXYZ_to_quat(payload_settings->lthigh_x, payload_settings->lthigh_y, payload_settings->lthigh_z);
				Quaternionf T_rthigh = bk->euXYZ_to_quat(payload_settings->rthigh_x, payload_settings->rthigh_y, payload_settings->rthigh_z);
				Quaternionf T_waist = bk->euXYZ_to_quat(payload_settings->waist_x, payload_settings->waist_y, payload_settings->waist_z);
				bk->setSensorTransform(T_lshin, T_rshin, T_lthigh, T_rthigh, T_waist);
				bk->setParam(payload_settings->shin,
					payload_settings->thigh,
					payload_settings->back,
					payload_settings->head,
					payload_settings->hip_width,
					payload_settings->shin_sensor,
					payload_settings->thigh_sensor,
					payload_settings->waist_sensor);
			}

			else {
				bKeepReading = false;
			}

			t_end = std::chrono::high_resolution_clock::now();

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
		}
	}
}

class DeviceDriver : public vr::ITrackedDeviceServerDriver
{
	int32_t TrackerIndex = 0;
	std::string model = "";
	std::string version = "";

public:
	DeviceDriver()
	{
		m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
		m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
	}

	virtual ~DeviceDriver()
	{
	}

	virtual void SetTrackerIndex(int32_t TrckIndex)
	{
		TrackerIndex = TrckIndex;
	}

	virtual void SetModel(std::string model)
	{
		this->model = model;
	}

	virtual void SetVersion(std::string version)
	{
		this->version = version;
	}

	virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId)
	{
		m_unObjectId = unObjectId;
		m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

		vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_OptOut);

		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_TrackingSystemName_String, "IMUMotionTracker");
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, model.c_str());
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, model.c_str());
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, "{imuFBT}frame");
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_WillDriftInYaw_Bool, false);
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ManufacturerName_String, "IMUMotionTracker");
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_TrackingFirmwareVersion_String, version.c_str());
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_HardwareRevision_String, version.c_str());

		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ConnectedWirelessDongle_String, version.c_str());
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_DeviceIsWireless_Bool, true);
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_DeviceIsCharging_Bool, false);
		VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_DeviceBatteryPercentage_Float, 1.0f);

		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_Firmware_UpdateAvailable_Bool, false);
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_Firmware_ManualUpdate_Bool, true);
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_Firmware_ManualUpdateURL_String, "https://www.google.com");
		VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_HardwareRevision_Uint64, 0);
		VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_FirmwareVersion_Uint64, 0);
		VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_FPGAVersion_Uint64, 0);
		VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_VRCVersion_Uint64, 0);
		VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_RadioVersion_Uint64, 0);
		VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_DongleVersion_Uint64, 0);



		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_DeviceProvidesBatteryStatus_Bool, true);
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_DeviceCanPowerOff_Bool, true);
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_Firmware_ProgrammingTarget_String, version.c_str());



		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_Firmware_ForceUpdateRequired_Bool, false);

		VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_ParentDriver_Uint64, 0);
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ResourceRoot_String, "imuFBT");
		std::string RegisteredDeviceType_String = std::string("imuFBT/");
		RegisteredDeviceType_String += model.c_str();
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RegisteredDeviceType_String, RegisteredDeviceType_String.c_str());
		VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_InputProfilePath_String, "{imuFBT}/input/imuFBT_profile.json");
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_NeverTracked_Bool, false);


		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_Identifiable_Bool, true);
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_CanWirelessIdentify_Bool, true);

		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_Firmware_RemindUpdate_Bool, false);

		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_ReportsTimeSinceVSync_Bool, false);

		VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);
		VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_PreviousUniverseId_Uint64, 2);

		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_DisplaySupportsRuntimeFramerateChange_Bool, false);
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_DisplaySupportsAnalogGain_Bool, false);
		VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_DisplayMinAnalogGain_Float, 1.0f);
		VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_DisplayMaxAnalogGain_Float, 1.0f);


		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasDisplayComponent_Bool, false);
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasCameraComponent_Bool, false);
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasDriverDirectModeComponent_Bool, false);
		VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasVirtualDisplayComponent_Bool, false);

		VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerHandSelectionPriority_Int32, 0);

		VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/Button0/click", &ButtonComponent[0]);
		VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/Button1/click", &ButtonComponent[1]);
		VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/Button2/click", &ButtonComponent[2]);
		VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/Button3/click", &ButtonComponent[3]);
		VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/Button4/click", &ButtonComponent[4]);
		VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/Button5/click", &ButtonComponent[5]);
		VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/Button6/click", &ButtonComponent[6]);
		VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/Button7/click", &ButtonComponent[7]);

		VRDriverInput()->CreateScalarComponent(m_ulPropertyContainer, "/input/Trigger0/value", &TriggerComponent[0], EVRScalarType::VRScalarType_Absolute, EVRScalarUnits::VRScalarUnits_NormalizedOneSided);
		VRDriverInput()->CreateScalarComponent(m_ulPropertyContainer, "/input/Trigger1/value", &TriggerComponent[1], EVRScalarType::VRScalarType_Absolute, EVRScalarUnits::VRScalarUnits_NormalizedOneSided);

		VRDriverInput()->CreateScalarComponent(m_ulPropertyContainer, "/input/Joystick0/x", &JoystickComponent[0], EVRScalarType::VRScalarType_Absolute, EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
		VRDriverInput()->CreateScalarComponent(m_ulPropertyContainer, "/input/Joystick0/y", &JoystickComponent[1], EVRScalarType::VRScalarType_Absolute, EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);

		VRDriverInput()->CreateHapticComponent(m_ulPropertyContainer, "/output/haptic", &HapticComponent);

		return VRInitError_None;
	}

	virtual void Deactivate()
	{
		m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	}

	virtual void EnterStandby()
	{
	}

	void* GetComponent(const char* pchComponentNameAndVersion)
	{
		if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version))
		{
			return (vr::IVRDisplayComponent*)this;
		}

		return NULL;
	}

	virtual void PowerOff()
	{
	}

	virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
	{
		if (unResponseBufferSize >= 1)
			pchResponseBuffer[0] = 0;
	}

	virtual DriverPose_t GetPose()
	{
		DriverPose_t pose = { 0 };

		if (SocketActivated) {
			switch (TrackerIndex) {
			case LSHIN:
				if (lshin_available) {
					if (lthigh_available && waist_available) {
						pose.poseIsValid = true;
						pose.result = TrackingResult_Running_OK;
						pose.deviceIsConnected = true;
					}
					else {
						pose.poseIsValid = true;
						pose.result = TrackingResult_Fallback_RotationOnly;
						pose.deviceIsConnected = true;
					}
					
				}
				else {
					pose.poseIsValid = false;
					pose.result = TrackingResult_Running_OutOfRange;
					pose.deviceIsConnected = false;
				}
				break;
			case RSHIN:
				if (rshin_available) {
					if (rthigh_available && waist_available) {
						pose.poseIsValid = true;
						pose.result = TrackingResult_Running_OK;
						pose.deviceIsConnected = true;
					}
					else {
						pose.poseIsValid = true;
						pose.result = TrackingResult_Fallback_RotationOnly;
						pose.deviceIsConnected = true;
					}

				}
				else {
					pose.poseIsValid = false;
					pose.result = TrackingResult_Running_OutOfRange;
					pose.deviceIsConnected = false;
				}
				break;
			case LTHIGH:
				if (lthigh_available) {
					if (waist_available) {
						pose.poseIsValid = true;
						pose.result = TrackingResult_Running_OK;
						pose.deviceIsConnected = true;
					}
					else {
						pose.poseIsValid = true;
						pose.result = TrackingResult_Fallback_RotationOnly;
						pose.deviceIsConnected = true;
					}

				}
				else {
					pose.poseIsValid = false;
					pose.result = TrackingResult_Running_OutOfRange;
					pose.deviceIsConnected = false;
				}
				break;
			case RTHIGH:
				if (rthigh_available) {
					if (waist_available) {
						pose.poseIsValid = true;
						pose.result = TrackingResult_Running_OK;
						pose.deviceIsConnected = true;
					}
					else {
						pose.poseIsValid = true;
						pose.result = TrackingResult_Fallback_RotationOnly;
						pose.deviceIsConnected = true;
					}

				}
				else {
					pose.poseIsValid = false;
					pose.result = TrackingResult_Running_OutOfRange;
					pose.deviceIsConnected = false;
				}
				break;
			case WAIST:
				if (waist_available) {
					pose.poseIsValid = true;
					pose.result = TrackingResult_Running_OK;
					pose.deviceIsConnected = true;
				}
				else {
					pose.poseIsValid = false;
					pose.result = TrackingResult_Running_OutOfRange;
					pose.deviceIsConnected = false;
				}
				break;
			}
		}
		else
		{
			pose.poseIsValid = false;
			pose.result = TrackingResult_Uninitialized;
			pose.deviceIsConnected = false;
		}

		if (!hmd_available) {
			pose.poseIsValid = false;
			pose.result = TrackingResult_Running_OutOfRange;
			pose.deviceIsConnected = false;
		}

		HmdQuaternion_t quat;
		quat.x = 0;
		quat.y = 0;
		quat.z = 0;
		quat.w = 1;

		pose.qWorldFromDriverRotation = quat;
		pose.qDriverFromHeadRotation = quat;

		switch (TrackerIndex) {
		case LSHIN:
			pose.vecPosition[0] = bk->P_lshin.x();
			pose.vecPosition[1] = bk->P_lshin.y();
			pose.vecPosition[2] = bk->P_lshin.z();

			pose.qRotation.x = bk->Q_lshin.x();
			pose.qRotation.y = bk->Q_lshin.y();
			pose.qRotation.z = bk->Q_lshin.z();
			pose.qRotation.w = bk->Q_lshin.w();
			break;
		case RSHIN:
			pose.vecPosition[0] = bk->P_rshin.x();
			pose.vecPosition[1] = bk->P_rshin.y();
			pose.vecPosition[2] = bk->P_rshin.z();

			pose.qRotation.x = bk->Q_rshin.x();
			pose.qRotation.y = bk->Q_rshin.y();
			pose.qRotation.z = bk->Q_rshin.z();
			pose.qRotation.w = bk->Q_rshin.w();
			break;
		case LTHIGH:
			pose.vecPosition[0] = bk->P_lthigh.x();
			pose.vecPosition[1] = bk->P_lthigh.y();
			pose.vecPosition[2] = bk->P_lthigh.z();

			pose.qRotation.x = bk->Q_lthigh.x();
			pose.qRotation.y = bk->Q_lthigh.y();
			pose.qRotation.z = bk->Q_lthigh.z();
			pose.qRotation.w = bk->Q_lthigh.w();
			break;
		case RTHIGH:
			pose.vecPosition[0] = bk->P_rthigh.x();
			pose.vecPosition[1] = bk->P_rthigh.y();
			pose.vecPosition[2] = bk->P_rthigh.z();

			pose.qRotation.x = bk->Q_rthigh.x();
			pose.qRotation.y = bk->Q_rthigh.y();
			pose.qRotation.z = bk->Q_rthigh.z();
			pose.qRotation.w = bk->Q_rthigh.w();
			break;
		case WAIST:
			pose.vecPosition[0] = bk->P_waist.x();
			pose.vecPosition[1] = bk->P_waist.y();
			pose.vecPosition[2] = bk->P_waist.z();

			pose.qRotation.x = bk->Q_waist.x();
			pose.qRotation.y = bk->Q_waist.y();
			pose.qRotation.z = bk->Q_waist.z();
			pose.qRotation.w = bk->Q_waist.w();
			break;
		}

		return pose;
	}

	void RunFrame()
	{
		if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid)
		{
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
		}
	}

private:
	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	VRInputComponentHandle_t ButtonComponent[8]{ 0 };
	VRInputComponentHandle_t TriggerComponent[2]{ 0 };
	VRInputComponentHandle_t JoystickComponent[2]{ 0 };
	VRInputComponentHandle_t HapticComponent{ 0 };
};

class DeviceProvider : public IServerTrackedDeviceProvider
{
public:
	virtual EVRInitError Init(vr::IVRDriverContext* pDriverContext);
	virtual void Cleanup();
	virtual const char* const* GetInterfaceVersions() { return vr::k_InterfaceVersions; }
	virtual void RunFrame();
	virtual bool ShouldBlockStandbyMode() { return false; }
	virtual void EnterStandby() {}
	virtual void LeaveStandby() {}

private:
	DeviceDriver* m_pTracker1 = nullptr;
	DeviceDriver* m_pTracker2 = nullptr;
	DeviceDriver* m_pTracker3 = nullptr;
	DeviceDriver* m_pTracker4 = nullptr;
	DeviceDriver* m_pTracker5 = nullptr;
};

DeviceProvider g_serverDriverNull;

EVRInitError DeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == 0) {
		local.sin_family = AF_INET;
		inet_pton(AF_INET, "0.0.0.0", &local.sin_addr.s_addr);
		local.sin_port = htons(driverPort);

		socketS = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		u_long nonblocking_enabled = true;
		ioctlsocket(socketS, FIONBIO, &nonblocking_enabled);

		if (socketS != INVALID_SOCKET) {

			iResult = bind(socketS, (sockaddr*)&local, sizeof(local));

			if (iResult != SOCKET_ERROR) {
				SocketActivated = true;
				pSocketThread = new std::thread(WinSockReadFunc);
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

	m_pTracker1 = new DeviceDriver();
	m_pTracker1->SetTrackerIndex(LSHIN);
	m_pTracker1->SetModel("lfoot");
	m_pTracker1->SetVersion("0.0.1");
	vr::VRServerDriverHost()->TrackedDeviceAdded("lfoot", vr::TrackedDeviceClass_GenericTracker, m_pTracker1);

	m_pTracker2 = new DeviceDriver();
	m_pTracker2->SetTrackerIndex(RSHIN);
	m_pTracker2->SetModel("rfoot");
	m_pTracker2->SetVersion("0.0.1");
	vr::VRServerDriverHost()->TrackedDeviceAdded("rfoot", vr::TrackedDeviceClass_GenericTracker, m_pTracker2);

	m_pTracker3 = new DeviceDriver();
	m_pTracker3->SetTrackerIndex(WAIST);
	m_pTracker3->SetModel("waist");
	m_pTracker3->SetVersion("0.0.1");
	vr::VRServerDriverHost()->TrackedDeviceAdded("waist", vr::TrackedDeviceClass_GenericTracker, m_pTracker3);

	m_pTracker4 = new DeviceDriver();
	m_pTracker4->SetTrackerIndex(LTHIGH);
	m_pTracker4->SetModel("lknee");
	m_pTracker4->SetVersion("0.0.1");
	vr::VRServerDriverHost()->TrackedDeviceAdded("lknee", vr::TrackedDeviceClass_GenericTracker, m_pTracker4);

	m_pTracker5 = new DeviceDriver();
	m_pTracker5->SetTrackerIndex(RTHIGH);
	m_pTracker5->SetModel("rknee");
	m_pTracker5->SetVersion("0.0.1");
	vr::VRServerDriverHost()->TrackedDeviceAdded("rknee", vr::TrackedDeviceClass_GenericTracker, m_pTracker5);

	// Transform from IMU ENU frame (X right, Y front, Z up) to driver frame (X right, Y up, Z back)
	bk->setDriverTransform(bk->euXYZ_to_quat(90.0 * PI / 180.0, 0, 0)); // Rotate X 90 deg

	return VRInitError_None;
}

void DeviceProvider::Cleanup()
{
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
		bk->update(imu_lshin, imu_rshin, imu_lthigh, imu_rthigh, imu_waist, Quaternionf(mat_hmd), p_hmd);
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
			bk->setOffset(imu_lshin, imu_rshin, imu_lthigh, imu_rthigh, imu_waist, direction);
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
}

HMD_DLL_EXPORT void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
	if (0 == strcmp(IServerTrackedDeviceProvider_Version, pInterfaceName))
	{
		return &g_serverDriverNull;
	}

	if (pReturnCode)
		*pReturnCode = VRInitError_Init_InterfaceNotFound;

	return NULL;
}