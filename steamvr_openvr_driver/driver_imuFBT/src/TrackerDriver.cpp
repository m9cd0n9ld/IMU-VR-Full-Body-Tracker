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
				if (chest_enable) {
					if (chest_available) {
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
					pose.poseIsValid = true;
					pose.result = TrackingResult_Running_OK;
					pose.deviceIsConnected = true;
				}
			}
			else {
				pose.poseIsValid = false;
				pose.result = TrackingResult_Running_OutOfRange;
				pose.deviceIsConnected = false;
			}
			break;
		case CHEST:
			if (chest_available && chest_enable) {
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
	case CHEST:
		pose.vecPosition[0] = bk->P_chest.x();
		pose.vecPosition[1] = bk->P_chest.y();
		pose.vecPosition[2] = bk->P_chest.z();

		pose.qRotation.x = bk->Q_chest.x();
		pose.qRotation.y = bk->Q_chest.y();
		pose.qRotation.z = bk->Q_chest.z();
		pose.qRotation.w = bk->Q_chest.w();
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