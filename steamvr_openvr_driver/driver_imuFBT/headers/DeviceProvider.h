#pragma once

#include <Globals.h>
#include <openvr_driver.h>
#include <TrackerDriver.h>
#include <UDP.h>

using namespace vr;

class DeviceProvider : public IServerTrackedDeviceProvider
{
public:
	virtual EVRInitError Init(IVRDriverContext* pDriverContext);
	virtual void Cleanup();
	virtual const char* const* GetInterfaceVersions();
	virtual void RunFrame();
	virtual bool ShouldBlockStandbyMode();
	virtual void EnterStandby();
	virtual void LeaveStandby();
	void initDevice();
	void initLoop();

private:
	bool initRun = false;
	std::thread* initThread = NULL;

	UDP* udpThread = nullptr;

	TrackerDriver* m_pTracker1 = nullptr;
	TrackerDriver* m_pTracker2 = nullptr;
	TrackerDriver* m_pTracker3 = nullptr;
	TrackerDriver* m_pTracker4 = nullptr;
	TrackerDriver* m_pTracker5 = nullptr;
	TrackerDriver* m_pTracker6 = nullptr;
	TrackerDriver* m_pTracker7 = nullptr;
	TrackerDriver* m_pTracker8 = nullptr;
	TrackerDriver* m_pTracker9 = nullptr;
	TrackerDriver* m_pTracker10 = nullptr;
	TrackerDriver* m_pTracker11 = nullptr;
	TrackerDriver* m_pTracker12 = nullptr;
};