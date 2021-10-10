#include <TrackerDriver.h>

using namespace vr;

void TrackerDriver::SetTrackerIndex(int32_t TrackerIndex)
{
	this->TrackerIndex = TrackerIndex;
}

void TrackerDriver::SetModel(std::string model)
{
	this->model = model;
}

void TrackerDriver::SetVersion(std::string version)
{
	this->version = version;
}

EVRInitError TrackerDriver::Activate(TrackedDeviceIndex_t unObjectId)
{
	m_unObjectId = unObjectId;
	m_ulPropertyContainer = VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

	VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_OptOut);

	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_TrackingSystemName_String, "IMUMotionTracker");
	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, model.c_str());
	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, model.c_str());
	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, "{imuFBT}frame");
	VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_WillDriftInYaw_Bool, false);
	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ManufacturerName_String, "IMUMotionTracker");
	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_TrackingFirmwareVersion_String, version.c_str());
	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_HardwareRevision_String, version.c_str());

	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ResourceRoot_String, "imuFBT");
	std::string RegisteredDeviceType_String = std::string("imuFBT/");
	RegisteredDeviceType_String += model.c_str();
	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RegisteredDeviceType_String, RegisteredDeviceType_String.c_str());
	VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_InputProfilePath_String, "{imuFBT}/input/imuFBT_profile.json");
	VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_NeverTracked_Bool, false);

	VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);

	VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasDisplayComponent_Bool, false);
	VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasCameraComponent_Bool, false);
	VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasDriverDirectModeComponent_Bool, false);
	VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasVirtualDisplayComponent_Bool, false);

	VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerHandSelectionPriority_Int32, -1);

	return VRInitError_None;
}

void TrackerDriver::Deactivate()
{
	m_unObjectId = k_unTrackedDeviceIndexInvalid;
}

void TrackerDriver::EnterStandby()
{
}

void* TrackerDriver::GetComponent(const char* pchComponentNameAndVersion)
{
	if (!_stricmp(pchComponentNameAndVersion, IVRDisplayComponent_Version))
	{
		return (IVRDisplayComponent*)this;
	}

	return NULL;
}

void TrackerDriver::PowerOff()
{
}

void TrackerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

DriverPose_t TrackerDriver::GetPose()
{
	DriverPose_t pose = { 0 };

	if (SocketActivated) {
		switch (TrackerIndex) {
		case LFOOT:
			if (lfoot_available) {
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
		case RFOOT:
			if (rfoot_available) {
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
		case LSHIN:
			if (lshin_available) {
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
		case RSHIN:
			if (rshin_available) {
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
		case LTHIGH:
			if (lthigh_available) {
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
		case RTHIGH:
			if (rthigh_available) {
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
		case CHEST:
			if (chest_available) {
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
		case LSHOULDER:
			if (lshoulder_available) {
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
		case RSHOULDER:
			if (rshoulder_available) {
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
		case LUPPERARM:
			if (lupperarm_available) {
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
		case RUPPERARM:
			if (rupperarm_available) {
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

	HmdQuaternion_t quat;
	quat.x = 0;
	quat.y = 0;
	quat.z = 0;
	quat.w = 1;

	pose.qWorldFromDriverRotation = quat;
	pose.qDriverFromHeadRotation = quat;

	switch (TrackerIndex) {
	case LFOOT:
		pose.vecPosition[0] = Px_lfoot;
		pose.vecPosition[1] = Py_lfoot;
		pose.vecPosition[2] = Pz_lfoot;

		pose.qRotation.x = map(Qx_lfoot);
		pose.qRotation.y = map(Qy_lfoot);
		pose.qRotation.z = map(Qz_lfoot);
		pose.qRotation.w = map(Qw_lfoot);
		break;
	case RFOOT:
		pose.vecPosition[0] = Px_rfoot;
		pose.vecPosition[1] = Py_rfoot;
		pose.vecPosition[2] = Pz_rfoot;

		pose.qRotation.x = map(Qx_rfoot);
		pose.qRotation.y = map(Qy_rfoot);
		pose.qRotation.z = map(Qz_rfoot);
		pose.qRotation.w = map(Qw_rfoot);
		break;
	case LSHIN:
		pose.vecPosition[0] = Px_lshin;
		pose.vecPosition[1] = Py_lshin;
		pose.vecPosition[2] = Pz_lshin;

		pose.qRotation.x = map(Qx_lshin);
		pose.qRotation.y = map(Qy_lshin);
		pose.qRotation.z = map(Qz_lshin);
		pose.qRotation.w = map(Qw_lshin);
		break;
	case RSHIN:
		pose.vecPosition[0] = Px_rshin;
		pose.vecPosition[1] = Py_rshin;
		pose.vecPosition[2] = Pz_rshin;

		pose.qRotation.x = map(Qx_rshin);
		pose.qRotation.y = map(Qy_rshin);
		pose.qRotation.z = map(Qz_rshin);
		pose.qRotation.w = map(Qw_rshin);
		break;
	case LTHIGH:
		pose.vecPosition[0] = Px_lthigh;
		pose.vecPosition[1] = Py_lthigh;
		pose.vecPosition[2] = Pz_lthigh;

		pose.qRotation.x = map(Qx_lthigh);
		pose.qRotation.y = map(Qy_lthigh);
		pose.qRotation.z = map(Qz_lthigh);
		pose.qRotation.w = map(Qw_lthigh);
		break;
	case RTHIGH:
		pose.vecPosition[0] = Px_rthigh;
		pose.vecPosition[1] = Py_rthigh;
		pose.vecPosition[2] = Pz_rthigh;

		pose.qRotation.x = map(Qx_rthigh);
		pose.qRotation.y = map(Qy_rthigh);
		pose.qRotation.z = map(Qz_rthigh);
		pose.qRotation.w = map(Qw_rthigh);
		break;
	case WAIST:
		pose.vecPosition[0] = Px_waist;
		pose.vecPosition[1] = Py_waist;
		pose.vecPosition[2] = Pz_waist;

		pose.qRotation.x = map(Qx_waist);
		pose.qRotation.y = map(Qy_waist);
		pose.qRotation.z = map(Qz_waist);
		pose.qRotation.w = map(Qw_waist);
		break;
	case CHEST:
		pose.vecPosition[0] = Px_chest;
		pose.vecPosition[1] = Py_chest;
		pose.vecPosition[2] = Pz_chest;

		pose.qRotation.x = map(Qx_chest);
		pose.qRotation.y = map(Qy_chest);
		pose.qRotation.z = map(Qz_chest);
		pose.qRotation.w = map(Qw_chest);
		break;
	case LSHOULDER:
		pose.vecPosition[0] = Px_lshoulder;
		pose.vecPosition[1] = Py_lshoulder;
		pose.vecPosition[2] = Pz_lshoulder;

		pose.qRotation.x = map(Qx_lshoulder);
		pose.qRotation.y = map(Qy_lshoulder);
		pose.qRotation.z = map(Qz_lshoulder);
		pose.qRotation.w = map(Qw_lshoulder);
		break;
	case RSHOULDER:
		pose.vecPosition[0] = Px_rshoulder;
		pose.vecPosition[1] = Py_rshoulder;
		pose.vecPosition[2] = Pz_rshoulder;

		pose.qRotation.x = map(Qx_rshoulder);
		pose.qRotation.y = map(Qy_rshoulder);
		pose.qRotation.z = map(Qz_rshoulder);
		pose.qRotation.w = map(Qw_rshoulder);
		break;
	case LUPPERARM:
		pose.vecPosition[0] = Px_lupperarm;
		pose.vecPosition[1] = Py_lupperarm;
		pose.vecPosition[2] = Pz_lupperarm;

		pose.qRotation.x = map(Qx_lupperarm);
		pose.qRotation.y = map(Qy_lupperarm);
		pose.qRotation.z = map(Qz_lupperarm);
		pose.qRotation.w = map(Qw_lupperarm);
		break;
	case RUPPERARM:
		pose.vecPosition[0] = Px_rupperarm;
		pose.vecPosition[1] = Py_rupperarm;
		pose.vecPosition[2] = Pz_rupperarm;

		pose.qRotation.x = map(Qx_rupperarm);
		pose.qRotation.y = map(Qy_rupperarm);
		pose.qRotation.z = map(Qz_rupperarm);
		pose.qRotation.w = map(Qw_rupperarm);
		break;
	}

	return pose;
}

void TrackerDriver::RunFrame()
{
	if (m_unObjectId != k_unTrackedDeviceIndexInvalid)
	{
		VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
	}
}