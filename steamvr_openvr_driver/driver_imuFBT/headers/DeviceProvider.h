#pragma once

#include <Globals.h>
#include <openvr_driver.h>
#include <TrackerDriver.h>
#include <ImuUDP.h>

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
	void forwardKinematics();

private:
	bool kinematicsRun = false;
	std::thread* kinematicsThread = NULL;

	ImuUDP* udpThread = nullptr;

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

	std::chrono::high_resolution_clock::time_point t_hmd_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();
	double elapsed_time_ms;
};