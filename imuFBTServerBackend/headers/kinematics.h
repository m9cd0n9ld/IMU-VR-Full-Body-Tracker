#pragma once

#include <Eigen/Dense>
#include <math.h>

#define PI 3.14159265359

using namespace Eigen;

class BodyKinematics {

public:

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	BodyKinematics()
	{
		Q_lfoot = Quaternionf(1, 0, 0, 0);
		Q_rfoot = Quaternionf(1, 0, 0, 0);
		Q_lshin = Quaternionf(1, 0, 0, 0);
		Q_rshin = Quaternionf(1, 0, 0, 0);
		Q_lthigh = Quaternionf(1, 0, 0, 0);
		Q_rthigh = Quaternionf(1, 0, 0, 0);
		Q_waist = Quaternionf(1, 0, 0, 0);
		Q_chest = Quaternionf(1, 0, 0, 0);
		Q_lshoulder = Quaternionf(1, 0, 0, 0);
		Q_rshoulder = Quaternionf(1, 0, 0, 0);
		Q_lupperarm = Quaternionf(1, 0, 0, 0);
		Q_rupperarm = Quaternionf(1, 0, 0, 0);

		P_lfoot = Vector3f(0, 0, 0);
		P_rfoot = Vector3f(0, 0, 0);
		P_lshin = Vector3f(0, 0, 0);
		P_rshin = Vector3f(0, 0, 0);
		P_lthigh = Vector3f(0, 0, 0);
		P_rthigh = Vector3f(0, 0, 0);
		P_waist = Vector3f(0, 0, 0);
		P_chest = Vector3f(0, 0, 0);
		P_lshoulder = Vector3f(0, 0, 0);
		P_rshoulder = Vector3f(0, 0, 0);
		P_lupperarm = Vector3f(0, 0, 0);
		P_rupperarm = Vector3f(0, 0, 0);

		T_driver = Quaternionf(1, 0, 0, 0);

		T_lfoot = Quaternionf(1, 0, 0, 0);
		T_rfoot = Quaternionf(1, 0, 0, 0);
		T_lshin = Quaternionf(1, 0, 0, 0);
		T_rshin = Quaternionf(1, 0, 0, 0);
		T_lthigh = Quaternionf(1, 0, 0, 0);
		T_rthigh = Quaternionf(1, 0, 0, 0);
		T_waist = Quaternionf(1, 0, 0, 0);
		T_chest = Quaternionf(1, 0, 0, 0);
		T_lshoulder = Quaternionf(1, 0, 0, 0);
		T_rshoulder = Quaternionf(1, 0, 0, 0);
		T_lupperarm = Quaternionf(1, 0, 0, 0);
		T_rupperarm = Quaternionf(1, 0, 0, 0);

		offset_level_lfoot = Quaternionf(1, 0, 0, 0);
		offset_level_rfoot = Quaternionf(1, 0, 0, 0);
		offset_level_lshin = Quaternionf(1, 0, 0, 0);
		offset_level_rshin = Quaternionf(1, 0, 0, 0);
		offset_level_lthigh = Quaternionf(1, 0, 0, 0);
		offset_level_rthigh = Quaternionf(1, 0, 0, 0);
		offset_level_waist = Quaternionf(1, 0, 0, 0);
		offset_level_chest = Quaternionf(1, 0, 0, 0);
		offset_level_lshoulder = Quaternionf(1, 0, 0, 0);
		offset_level_rshoulder = Quaternionf(1, 0, 0, 0);
		offset_level_lupperarm = Quaternionf(1, 0, 0, 0);
		offset_level_rupperarm = Quaternionf(1, 0, 0, 0);

		offset_heading_lfoot = Quaternionf(1, 0, 0, 0);
		offset_heading_rfoot = Quaternionf(1, 0, 0, 0);
		offset_heading_lshin = Quaternionf(1, 0, 0, 0);
		offset_heading_rshin = Quaternionf(1, 0, 0, 0);
		offset_heading_lthigh = Quaternionf(1, 0, 0, 0);
		offset_heading_rthigh = Quaternionf(1, 0, 0, 0);
		offset_heading_waist = Quaternionf(1, 0, 0, 0);
		offset_heading_chest = Quaternionf(1, 0, 0, 0);
		offset_heading_lshoulder = Quaternionf(1, 0, 0, 0);
		offset_heading_rshoulder = Quaternionf(1, 0, 0, 0);
		offset_heading_lupperarm = Quaternionf(1, 0, 0, 0);
		offset_heading_rupperarm = Quaternionf(1, 0, 0, 0);

		shin = 0.5;
		thigh = 0.4;
		lback = 0.25;
		uback = 0.35;
		head = 0.2;
		shoulder = 0.1;
		hip_width = 0.3;
		shoulder_width = 0.3;
		foot_sensor = 0.05;
		shin_sensor = 0.2;
		thigh_sensor = 0.2;
		waist_sensor = 0.2;
		chest_sensor = 0.2;
		shoulder_sensor = 0.1;
		upperarm_sensor = 0.15;
		floor_offset = 0.0;

		lfoot_sensor_offset = Vector2f(0, 0);
		rfoot_sensor_offset = Vector2f(0, 0);
		lshin_sensor_offset = Vector2f(0, 0);
		rshin_sensor_offset = Vector2f(0, 0);
		lthigh_sensor_offset = Vector2f(0, 0);
		rthigh_sensor_offset = Vector2f(0, 0);
		waist_sensor_offset = Vector2f(0, 0);
		chest_sensor_offset = Vector2f(0, 0);
		lshoulder_sensor_offset = Vector2f(0, 0);
		rshoulder_sensor_offset = Vector2f(0, 0);
		lupperarm_sensor_offset = Vector2f(0, 0);
		rupperarm_sensor_offset = Vector2f(0, 0);
		head_sensor_offset = Vector2f(0, 0);
	}

	// Euler XYZ to quaternion
	Quaternionf euXYZ_to_quat(float x, float y, float z) {
		AngleAxisf X(x, Eigen::Vector3f::UnitX());
		AngleAxisf Y(y, Eigen::Vector3f::UnitY());
		AngleAxisf Z(z, Eigen::Vector3f::UnitZ());

		Quaternionf q = X * Y * Z;
		return q;
	}

	// Euler YXZ to quaternion
	Quaternionf euYXZ_to_quat(float y, float x, float z) {
		AngleAxisf X(x, Eigen::Vector3f::UnitX());
		AngleAxisf Y(y, Eigen::Vector3f::UnitY());
		AngleAxisf Z(z, Eigen::Vector3f::UnitZ());

		Quaternionf q = Y * X * Z;
		return q;
	}

	//Set positional parameters when in T-pose (body segment length and sensor position)
	void setParam(float shin, float thigh, float lback, float uback, float head, float shoulder, float hip_width, float shoulder_width, float foot_sensor, float shin_sensor, float thigh_sensor, float waist_sensor, float chest_sensor, float shoulder_sensor, float upperarm_sensor, float floor_offset) {
		this->shin = shin;						// knee to foot segment length
		this->thigh = thigh;					// knee to hip segment length
		this->lback = lback;					// hip to mid back segment length
		this->uback = uback;					// mid back to neck segment length
		this->head = head;						// neck to headset segment length
		this->shoulder = shoulder;				// neck to sholder segment length
		this->hip_width = hip_width;			// hip horizontal width
		this->shoulder_width = shoulder_width;	// shoulder horizontal width
		this->foot_sensor = foot_sensor;		// ankle joint to foot sensor position
		this->shin_sensor = shin_sensor;		// knee joint to shin sensor position
		this->thigh_sensor = thigh_sensor;		// hip joint to thigh sensor position
		this->waist_sensor = waist_sensor;		// hip joint to waist sensor position
		this->chest_sensor = chest_sensor;		// mid back joint to chest sensor position
		this->shoulder_sensor = shoulder_sensor;// body centre line to shoulder sensor position
		this->upperarm_sensor = upperarm_sensor;// shoulder joint to upper arm sensor position
		this->floor_offset = floor_offset;		// tracker min Y value
	}

	//Set sensor offset position when in T-pose
	void setSensorOffset(const Vector2f& lfoot, const Vector2f& rfoot, const Vector2f& lshin, const Vector2f& rshin, const Vector2f& lthigh, const Vector2f& rthigh, const Vector2f& waist, const Vector2f& chest, const Vector2f& lshoulder, const Vector2f& rshoulder, const Vector2f& lupperarm, const Vector2f& rupperarm, const Vector2f& head) {
		lfoot_sensor_offset = lfoot;
		rfoot_sensor_offset = rfoot;
		lshin_sensor_offset = lshin;
		rshin_sensor_offset = rshin;
		lthigh_sensor_offset = lthigh;
		rthigh_sensor_offset = rthigh;
		waist_sensor_offset = waist;
		chest_sensor_offset = chest;
		lshoulder_sensor_offset = lshoulder;
		rshoulder_sensor_offset = rshoulder;
		lupperarm_sensor_offset = lupperarm;
		rupperarm_sensor_offset = rupperarm;
		head_sensor_offset = head;
	}

	// Transform from IMU frame to driver frame (X right, Y up, Z back)
	void setDriverTransform(const Quaternionf& T_driver) {
		this->T_driver = T_driver;
	}

	// Transform from sensor frame to driver frame (X right, Y up, Z back) when in T-pose
	void setSensorTransform(const Quaternionf& T_lfoot, const Quaternionf& T_rfoot, const Quaternionf& T_lshin, const Quaternionf& T_rshin, const Quaternionf& T_lthigh, const Quaternionf& T_rthigh, const Quaternionf& T_waist, const Quaternionf& T_chest, const Quaternionf& T_lshoulder, const Quaternionf& T_rshoulder, const Quaternionf& T_lupperarm, const Quaternionf& T_rupperarm) {
		this->T_lfoot = T_lfoot;
		this->T_rfoot = T_rfoot;
		this->T_lshin = T_lshin;
		this->T_rshin = T_rshin;
		this->T_lthigh = T_lthigh;
		this->T_rthigh = T_rthigh;
		this->T_waist = T_waist;
		this->T_chest = T_chest;
		this->T_lshoulder = T_lshoulder;
		this->T_rshoulder = T_rshoulder;
		this->T_lupperarm = T_lupperarm;
		this->T_rupperarm = T_rupperarm;
	}

	// Set offset quaternion
	void setOffset(const Quaternionf& Imu_lfoot, const Quaternionf& Imu_rfoot, const Quaternionf& Imu_lshin, const Quaternionf& Imu_rshin, const Quaternionf& Imu_lthigh, const Quaternionf& Imu_rthigh, const Quaternionf& Imu_waist, const Quaternionf& Imu_chest, const Quaternionf& Imu_lshoulder, const Quaternionf& Imu_rshoulder, const Quaternionf& Imu_lupperarm, const Quaternionf& Imu_rupperarm, const Quaternionf& direction) {
		Quaternionf imu_lfoot = convertFrame(Imu_lfoot, T_lfoot);
		Quaternionf imu_rfoot = convertFrame(Imu_rfoot, T_rfoot);
		Quaternionf imu_lshin = convertFrame(Imu_lshin, T_lshin);
		Quaternionf imu_rshin = convertFrame(Imu_rshin, T_rshin);
		Quaternionf imu_lthigh = convertFrame(Imu_lthigh, T_lthigh);
		Quaternionf imu_rthigh = convertFrame(Imu_rthigh, T_rthigh);
		Quaternionf imu_waist = convertFrame(Imu_waist, T_waist);
		Quaternionf imu_chest = convertFrame(Imu_chest, T_chest);
		Quaternionf imu_lshoulder = convertFrame(Imu_lshoulder, T_lshoulder);
		Quaternionf imu_rshoulder = convertFrame(Imu_rshoulder, T_rshoulder);
		Quaternionf imu_lupperarm = convertFrame(Imu_lupperarm, T_lupperarm);
		Quaternionf imu_rupperarm = convertFrame(Imu_rupperarm, T_rupperarm);

		offset_level_lfoot = findLevelOffset(imu_lfoot);
		offset_level_rfoot = findLevelOffset(imu_rfoot);
		offset_level_lshin = findLevelOffset(imu_lshin);
		offset_level_rshin = findLevelOffset(imu_rshin);
		offset_level_lthigh = findLevelOffset(imu_lthigh);
		offset_level_rthigh = findLevelOffset(imu_rthigh);
		offset_level_waist = findLevelOffset(imu_waist);
		offset_level_chest = findLevelOffset(imu_chest);
		offset_level_lshoulder = findLevelOffset(imu_lshoulder);
		offset_level_rshoulder = findLevelOffset(imu_rshoulder);
		offset_level_lupperarm = findLevelOffset(imu_lupperarm);
		offset_level_rupperarm = findLevelOffset(imu_rupperarm);

		offset_heading_lfoot = findHeadingOffset(imu_lfoot, direction);
		offset_heading_rfoot = findHeadingOffset(imu_rfoot, direction);
		offset_heading_lshin = findHeadingOffset(imu_lshin, direction);
		offset_heading_rshin = findHeadingOffset(imu_rshin, direction);
		offset_heading_lthigh = findHeadingOffset(imu_lthigh, direction);
		offset_heading_rthigh = findHeadingOffset(imu_rthigh, direction);
		offset_heading_waist = findHeadingOffset(imu_waist, direction);
		offset_heading_chest = findHeadingOffset(imu_chest, direction);
		offset_heading_lshoulder = findHeadingOffset(imu_lshoulder, direction);
		offset_heading_rshoulder = findHeadingOffset(imu_rshoulder, direction);
		offset_heading_lupperarm = findHeadingOffset(imu_lupperarm, direction);
		offset_heading_rupperarm = findHeadingOffset(imu_rupperarm, direction);
	}

	// Forward kinematics
	void update(const Quaternionf& Imu_lfoot, const Quaternionf& Imu_rfoot, const Quaternionf& Imu_lshin, const Quaternionf& Imu_rshin, const Quaternionf& Imu_lthigh, const Quaternionf& Imu_rthigh, const Quaternionf& Imu_waist, const Quaternionf& Imu_chest, const Quaternionf& Imu_lshoulder, const Quaternionf& Imu_rshoulder, const Quaternionf& Imu_lupperarm, const Quaternionf& Imu_rupperarm, const Quaternionf& Q_hmd, const Vector3f& P_hmd) {
		Quaternionf imu_lfoot = offset_heading_lfoot * convertFrame(Imu_lfoot, T_lfoot) * offset_level_lfoot;
		Quaternionf imu_rfoot = offset_heading_rfoot * convertFrame(Imu_rfoot, T_rfoot) * offset_level_rfoot;
		Quaternionf imu_lshin = offset_heading_lshin * convertFrame(Imu_lshin, T_lshin) * offset_level_lshin;
		Quaternionf imu_rshin = offset_heading_rshin * convertFrame(Imu_rshin, T_rshin) * offset_level_rshin;
		Quaternionf imu_lthigh = offset_heading_lthigh * convertFrame(Imu_lthigh, T_lthigh) * offset_level_lthigh;
		Quaternionf imu_rthigh = offset_heading_rthigh * convertFrame(Imu_rthigh, T_rthigh) * offset_level_rthigh;
		Quaternionf imu_waist = offset_heading_waist * convertFrame(Imu_waist, T_waist) * offset_level_waist;
		Quaternionf imu_chest = offset_heading_chest * convertFrame(Imu_chest, T_chest) * offset_level_chest;
		Quaternionf imu_lshoulder = offset_heading_lshoulder * convertFrame(Imu_lshoulder, T_lshoulder) * offset_level_lshoulder;
		Quaternionf imu_rshoulder = offset_heading_rshoulder * convertFrame(Imu_rshoulder, T_rshoulder) * offset_level_rshoulder;
		Quaternionf imu_lupperarm = offset_heading_lupperarm * convertFrame(Imu_lupperarm, T_lupperarm) * offset_level_lupperarm;
		Quaternionf imu_rupperarm = offset_heading_rupperarm * convertFrame(Imu_rupperarm, T_rupperarm) * offset_level_rupperarm;
		
		Vector3f P_neck = P_hmd - (Q_hmd * Quaternionf(0, head_sensor_offset.coeff(0), head, head_sensor_offset.coeff(1)) * Q_hmd.inverse()).vec();
		Vector3f P_midshoulder = P_neck - (imu_chest * Quaternionf(0, 0, shoulder, 0) * imu_chest.inverse()).vec();
		Vector3f P_lshoulderjoint = P_midshoulder + (imu_lshoulder * Quaternionf(0, -shoulder_width / 2, 0, 0) * imu_lshoulder.inverse()).vec();
		Vector3f P_rshoulderjoint = P_midshoulder + (imu_rshoulder * Quaternionf(0, shoulder_width / 2, 0, 0) * imu_rshoulder.inverse()).vec();
		Vector3f P_midback = P_neck - (imu_chest * Quaternionf(0, 0, uback, 0) * imu_chest.inverse()).vec();
		Vector3f P_chip = P_midback - (imu_waist * Quaternionf(0, 0, lback, 0) * imu_waist.inverse()).vec();
		Vector3f P_lhip = P_chip + (imu_waist * Quaternionf(0, -hip_width / 2, 0, 0) * imu_waist.inverse()).vec();
		Vector3f P_rhip = P_chip + (imu_waist * Quaternionf(0, hip_width / 2, 0, 0) * imu_waist.inverse()).vec();
		Vector3f P_lknee = P_lhip + (imu_lthigh * Quaternionf(0, 0, -thigh, 0) * imu_lthigh.inverse()).vec();
		Vector3f P_rknee = P_rhip + (imu_rthigh * Quaternionf(0, 0, -thigh, 0) * imu_rthigh.inverse()).vec();
		Vector3f P_lheel = P_lknee + (imu_lshin * Quaternionf(0, 0, -shin, 0) * imu_lshin.inverse()).vec();
		Vector3f P_rheel = P_rknee + (imu_rshin * Quaternionf(0, 0, -shin, 0) * imu_rshin.inverse()).vec();

		P_lshoulder = P_midshoulder + (imu_lshoulder * Quaternionf(0, -shoulder_sensor, lshoulder_sensor_offset.coeff(1), lshoulder_sensor_offset.coeff(0)) * imu_lshoulder.inverse()).vec();
		Q_lshoulder = imu_lshoulder;
		if (P_lshoulder.y() < floor_offset) {
			P_lshoulder.y() = floor_offset;
		}

		P_rshoulder = P_midshoulder + (imu_rshoulder * Quaternionf(0, shoulder_sensor, rshoulder_sensor_offset.coeff(1), rshoulder_sensor_offset.coeff(0)) * imu_rshoulder.inverse()).vec();
		Q_rshoulder = imu_rshoulder;
		if (P_rshoulder.y() < floor_offset) {
			P_rshoulder.y() = floor_offset;
		}

		P_lupperarm = P_lshoulderjoint + (imu_lupperarm * Quaternionf(0, -upperarm_sensor, lupperarm_sensor_offset.coeff(1), lupperarm_sensor_offset.coeff(0)) * imu_lupperarm.inverse()).vec();
		Q_lupperarm = imu_lupperarm;
		if (P_lupperarm.y() < floor_offset) {
			P_lupperarm.y() = floor_offset;
		}

		P_rupperarm = P_rshoulderjoint + (imu_rupperarm * Quaternionf(0, upperarm_sensor, rupperarm_sensor_offset.coeff(1), rupperarm_sensor_offset.coeff(0)) * imu_rupperarm.inverse()).vec();
		Q_rupperarm = imu_rupperarm;
		if (P_rupperarm.y() < floor_offset) {
			P_rupperarm.y() = floor_offset;
		}

		P_chest = P_midback + (imu_chest * Quaternionf(0, chest_sensor_offset.coeff(0), chest_sensor, chest_sensor_offset.coeff(1)) * imu_chest.inverse()).vec();
		Q_chest = imu_chest;
		if (P_chest.y() < floor_offset) {
			P_chest.y() = floor_offset;
		}

		P_waist = P_chip + (imu_waist * Quaternionf(0, waist_sensor_offset.coeff(0), waist_sensor, waist_sensor_offset.coeff(1)) * imu_waist.inverse()).vec();
		Q_waist = imu_waist;
		if (P_waist.y() < floor_offset) {
			P_waist.y() = floor_offset;
		}

		P_lthigh = P_lhip + (imu_lthigh * Quaternionf(0, lthigh_sensor_offset.coeff(0), -thigh_sensor, lthigh_sensor_offset.coeff(1)) * imu_lthigh.inverse()).vec();
		Q_lthigh = imu_lthigh;
		if (P_lthigh.y() < floor_offset) {
			P_lthigh.y() = floor_offset;
		}

		P_rthigh = P_rhip + (imu_rthigh * Quaternionf(0, rthigh_sensor_offset.coeff(0), -thigh_sensor, rthigh_sensor_offset.coeff(1)) * imu_rthigh.inverse()).vec();
		Q_rthigh = imu_rthigh;
		if (P_rthigh.y() < floor_offset) {
			P_rthigh.y() = floor_offset;
		}

		P_lshin = P_lknee + (imu_lshin * Quaternionf(0, lshin_sensor_offset.coeff(0), -shin_sensor, lshin_sensor_offset.coeff(1)) * imu_lshin.inverse()).vec();
		Q_lshin = imu_lshin;
		if (P_lshin.y() < floor_offset) {
			P_lshin.y() = floor_offset;
		}

		P_rshin = P_rknee + (imu_rshin * Quaternionf(0, rshin_sensor_offset.coeff(0), -shin_sensor, rshin_sensor_offset.coeff(1)) * imu_rshin.inverse()).vec();
		Q_rshin = imu_rshin;
		if (P_rshin.y() < floor_offset) {
			P_rshin.y() = floor_offset;
		}

		P_lfoot = P_lheel + (imu_lfoot * Quaternionf(0, lfoot_sensor_offset.coeff(0), lfoot_sensor_offset.coeff(1), -foot_sensor) * imu_lfoot.inverse()).vec();
		Q_lfoot = imu_lfoot;
		if (P_lfoot.y() < floor_offset) {
			P_lfoot.y() = floor_offset;
		}

		P_rfoot = P_rheel + (imu_rfoot * Quaternionf(0, rfoot_sensor_offset.coeff(0), rfoot_sensor_offset.coeff(1), -foot_sensor) * imu_rfoot.inverse()).vec();
		Q_rfoot = imu_rfoot;
		if (P_rfoot.y() < floor_offset) {
			P_rfoot.y() = floor_offset;
		}
	}
	
	Quaternionf Q_lfoot;
	Quaternionf Q_rfoot;
	Quaternionf Q_lshin;
	Quaternionf Q_rshin;
	Quaternionf Q_lthigh;
	Quaternionf Q_rthigh;
	Quaternionf Q_waist;
	Quaternionf Q_chest;
	Quaternionf Q_lshoulder;
	Quaternionf Q_rshoulder;
	Quaternionf Q_lupperarm;
	Quaternionf Q_rupperarm;

	Vector3f P_lfoot;
	Vector3f P_rfoot;
	Vector3f P_lshin;
	Vector3f P_rshin;
	Vector3f P_lthigh;
	Vector3f P_rthigh;
	Vector3f P_waist;
	Vector3f P_chest;
	Vector3f P_lshoulder;
	Vector3f P_rshoulder;
	Vector3f P_lupperarm;
	Vector3f P_rupperarm;

private:

	// Convert original IMU reporting frame to driver reporting frame (X right, Y up, Z back)
	Quaternionf convertFrame(const Quaternionf& imu, const Quaternionf& T_imu_driver) {
		Quaternionf T_enu_imu = T_driver * T_imu_driver.inverse();
		Quaternionf imu_zero = T_enu_imu.inverse() * imu;	// Align imu value to driver frame
		return T_imu_driver.inverse() * imu_zero * T_imu_driver;	// Change reporting frame to driver frame
	}

	// Find leveled offset quaternion
	Quaternionf findLevelOffset(const Quaternionf& imu) {
		Quaternionf leveled_quat(imu.w(), 0, imu.y(), 0);
		leveled_quat.normalize();
		return imu.inverse() * leveled_quat;
	}

	// Find offset quaternion to specified heading
	Quaternionf findHeadingOffset(const Quaternionf& imu, const Quaternionf& direction) {
		Quaternionf heading_quat(imu.w(), 0, imu.y(), 0);
		heading_quat.normalize();
		return heading_quat.inverse() * direction;
	}

	Quaternionf T_driver;

	Quaternionf T_lfoot;
	Quaternionf T_rfoot;
	Quaternionf T_lshin;
	Quaternionf T_rshin;
	Quaternionf T_lthigh;
	Quaternionf T_rthigh;
	Quaternionf T_waist;
	Quaternionf T_chest;
	Quaternionf T_lshoulder;
	Quaternionf T_rshoulder;
	Quaternionf T_lupperarm;
	Quaternionf T_rupperarm;

	Quaternionf offset_level_lfoot;
	Quaternionf offset_level_rfoot;
	Quaternionf offset_level_lshin;
	Quaternionf offset_level_rshin;
	Quaternionf offset_level_lthigh;
	Quaternionf offset_level_rthigh;
	Quaternionf offset_level_waist;
	Quaternionf offset_level_chest;
	Quaternionf offset_level_lshoulder;
	Quaternionf offset_level_rshoulder;
	Quaternionf offset_level_lupperarm;
	Quaternionf offset_level_rupperarm;

	Quaternionf offset_heading_lfoot;
	Quaternionf offset_heading_rfoot;
	Quaternionf offset_heading_lshin;
	Quaternionf offset_heading_rshin;
	Quaternionf offset_heading_lthigh;
	Quaternionf offset_heading_rthigh;
	Quaternionf offset_heading_waist;
	Quaternionf offset_heading_chest;
	Quaternionf offset_heading_lshoulder;
	Quaternionf offset_heading_rshoulder;
	Quaternionf offset_heading_lupperarm;
	Quaternionf offset_heading_rupperarm;

	float shin;
	float thigh;
	float lback;
	float uback;
	float head;
	float shoulder;
	float hip_width;
	float shoulder_width;
	float foot_sensor;
	float shin_sensor;
	float thigh_sensor;
	float waist_sensor;
	float chest_sensor;
	float shoulder_sensor;
	float upperarm_sensor;
	float floor_offset;

	Vector2f lfoot_sensor_offset;
	Vector2f rfoot_sensor_offset;
	Vector2f lshin_sensor_offset;
	Vector2f rshin_sensor_offset;
	Vector2f lthigh_sensor_offset;
	Vector2f rthigh_sensor_offset;
	Vector2f waist_sensor_offset;
	Vector2f chest_sensor_offset;
	Vector2f lshoulder_sensor_offset;
	Vector2f rshoulder_sensor_offset;
	Vector2f lupperarm_sensor_offset;
	Vector2f rupperarm_sensor_offset;
	Vector2f head_sensor_offset;
};