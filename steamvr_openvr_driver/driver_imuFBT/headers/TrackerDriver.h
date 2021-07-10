#pragma once

#include <Globals.h>
#include <openvr_driver.h>

using namespace vr;

class TrackerDriver : public ITrackedDeviceServerDriver
{

public:
	virtual void SetTrackerIndex(int32_t TrackerIndex);
	virtual void SetModel(std::string model);
	virtual void SetVersion(std::string version);
	virtual EVRInitError Activate(TrackedDeviceIndex_t unObjectId);
	virtual void Deactivate();
	virtual void EnterStandby();
	void* GetComponent(const char* pchComponentNameAndVersion);
	virtual void PowerOff();
	virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);
	virtual DriverPose_t GetPose();
	void RunFrame();

private:
	int32_t TrackerIndex = 0;
	std::string model = "";
	std::string version = "";

	TrackedDeviceIndex_t m_unObjectId = k_unTrackedDeviceIndexInvalid;
	PropertyContainerHandle_t m_ulPropertyContainer = k_ulInvalidPropertyContainer;
};