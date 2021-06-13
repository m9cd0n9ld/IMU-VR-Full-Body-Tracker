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
		Q_lshin = Quaternionf(1, 0, 0, 0);
		Q_rshin = Quaternionf(1, 0, 0, 0);
		Q_lthigh = Quaternionf(1, 0, 0, 0);
		Q_rthigh = Quaternionf(1, 0, 0, 0);
		Q_waist = Quaternionf(1, 0, 0, 0);

		P_lshin = Vector3f(0, 0, 0);
		P_rshin = Vector3f(0, 0, 0);
		P_lthigh = Vector3f(0, 0, 0);
		P_rthigh = Vector3f(0, 0, 0);
		P_waist = Vector3f(0, 0, 0);

		T_driver = Quaternionf(1, 0, 0, 0);

		T_lshin = Quaternionf(1, 0, 0, 0);
		T_rshin = Quaternionf(1, 0, 0, 0);
		T_lthigh = Quaternionf(1, 0, 0, 0);
		T_rthigh = Quaternionf(1, 0, 0, 0);
		T_waist = Quaternionf(1, 0, 0, 0);

		offset_level_lshin = Quaternionf(1, 0, 0, 0);
		offset_level_rshin = Quaternionf(1, 0, 0, 0);
		offset_level_lthigh = Quaternionf(1, 0, 0, 0);
		offset_level_rthigh = Quaternionf(1, 0, 0, 0);
		offset_level_waist = Quaternionf(1, 0, 0, 0);

		offset_heading_lshin = Quaternionf(1, 0, 0, 0);
		offset_heading_rshin = Quaternionf(1, 0, 0, 0);
		offset_heading_lthigh = Quaternionf(1, 0, 0, 0);
		offset_heading_rthigh = Quaternionf(1, 0, 0, 0);
		offset_heading_waist = Quaternionf(1, 0, 0, 0);

		shin = 0.5;
		thigh = 0.4;
		back = 0.6;
		head = 0.2;
		hip_width = 0.3;
		shin_sensor = 0.2;
		thigh_sensor = 0.2;
		waist_sensor = 0.2;
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

	//Set positional parameters
	void setParam(float shin, float thigh, float back, float head, float hip_width, float shin_sensor, float thigh_sensor, float waist_sensor) {
		this->shin = shin;
		this->thigh = thigh;
		this->back = back;
		this->head = head;
		this->hip_width = hip_width;
		this->shin_sensor = shin_sensor;		// from knee
		this->thigh_sensor = thigh_sensor;		// from hip
		this->waist_sensor = waist_sensor;		// from hip
	}

	// Transform from IMU frame to driver frame (X right, Y up, Z back)
	void setDriverTransform(const Quaternionf& T_driver) {
		this->T_driver = T_driver;
	}

	// Transform from sensor frame to driver frame (X right, Y up, Z back)
	void setSensorTransform(const Quaternionf& T_lshin, const Quaternionf& T_rshin, const Quaternionf& T_lthigh, const Quaternionf& T_rthigh, const Quaternionf& T_waist) {
		this->T_lshin = T_lshin;
		this->T_rshin = T_rshin;
		this->T_lthigh = T_lthigh;
		this->T_rthigh = T_rthigh;
		this->T_waist = T_waist;
	}

	// Set offset quaternion
	void setOffset(const Quaternionf& Imu_lshin, const Quaternionf& Imu_rshin, const Quaternionf& Imu_lthigh, const Quaternionf& Imu_rthigh, const Quaternionf& Imu_waist, const Quaternionf& direction) {
		Quaternionf imu_lshin = convertFrame(Imu_lshin, T_lshin);
		Quaternionf imu_rshin = convertFrame(Imu_rshin, T_rshin);
		Quaternionf imu_lthigh = convertFrame(Imu_lthigh, T_lthigh);
		Quaternionf imu_rthigh = convertFrame(Imu_rthigh, T_rthigh);
		Quaternionf imu_waist = convertFrame(Imu_waist, T_waist);

		offset_level_lshin = findLevelOffset(imu_lshin);
		offset_level_rshin = findLevelOffset(imu_rshin);
		offset_level_lthigh = findLevelOffset(imu_lthigh);
		offset_level_rthigh = findLevelOffset(imu_rthigh);
		offset_level_waist = findLevelOffset(imu_waist);

		offset_heading_lshin = findHeadingOffset(imu_lshin, direction);
		offset_heading_rshin = findHeadingOffset(imu_rshin, direction);
		offset_heading_lthigh = findHeadingOffset(imu_lthigh, direction);
		offset_heading_rthigh = findHeadingOffset(imu_rthigh, direction);
		offset_heading_waist = findHeadingOffset(imu_waist, direction);
	}

	// Forward kinematics
	void update(const Quaternionf& Imu_lshin, const Quaternionf& Imu_rshin, const Quaternionf& Imu_lthigh, const Quaternionf& Imu_rthigh, const Quaternionf& Imu_waist, const Quaternionf& Q_hmd, const Vector3f& P_hmd) {
		Quaternionf imu_lshin = offset_heading_lshin * convertFrame(Imu_lshin, T_lshin) * offset_level_lshin;
		Quaternionf imu_rshin = offset_heading_rshin * convertFrame(Imu_rshin, T_rshin) * offset_level_rshin;
		Quaternionf imu_lthigh = offset_heading_lthigh * convertFrame(Imu_lthigh, T_lthigh) * offset_level_lthigh;
		Quaternionf imu_rthigh = offset_heading_rthigh * convertFrame(Imu_rthigh, T_rthigh) * offset_level_rthigh;
		Quaternionf imu_waist = offset_heading_waist * convertFrame(Imu_waist, T_waist) * offset_level_waist;
		
		Vector3f P_neck = P_hmd - (Q_hmd * Quaternionf(0, 0, head, 0) * Q_hmd.inverse()).vec();
		Vector3f P_chip = P_neck - (imu_waist * Quaternionf(0, 0, back, 0) * imu_waist.inverse()).vec();
		Vector3f P_lhip = P_chip + (imu_waist * Quaternionf(0, -hip_width / 2, 0, 0) * imu_waist.inverse()).vec();
		Vector3f P_rhip = P_chip + (imu_waist * Quaternionf(0, hip_width / 2, 0, 0) * imu_waist.inverse()).vec();
		Vector3f P_lknee = P_lhip + (imu_lthigh * Quaternionf(0, 0, -thigh, 0) * imu_lthigh.inverse()).vec();
		Vector3f P_rknee = P_rhip + (imu_rthigh * Quaternionf(0, 0, -thigh, 0) * imu_rthigh.inverse()).vec();
		Vector3f P_lfoot = P_lknee + (imu_lshin * Quaternionf(0, 0, -shin, 0) * imu_lshin.inverse()).vec();
		Vector3f P_rfoot = P_rknee + (imu_rshin * Quaternionf(0, 0, -shin, 0) * imu_rshin.inverse()).vec();

		P_waist = P_chip + (imu_waist * Quaternionf(0, 0, waist_sensor, 0) * imu_waist.inverse()).vec();
		Q_waist = imu_waist;
		if (P_waist.y() < 0) {
			P_waist.y() = 0;
		}

		P_lthigh = P_lhip + (imu_lthigh * Quaternionf(0, 0, -thigh_sensor, 0) * imu_lthigh.inverse()).vec();
		Q_lthigh = imu_lthigh;
		if (P_lthigh.y() < 0) {
			P_lthigh.y() = 0;
		}

		P_rthigh = P_rhip + (imu_rthigh * Quaternionf(0, 0, -thigh_sensor, 0) * imu_rthigh.inverse()).vec();
		Q_rthigh = imu_rthigh;
		if (P_rthigh.y() < 0) {
			P_rthigh.y() = 0;
		}

		P_lshin = P_lknee + (imu_lshin * Quaternionf(0, 0, -shin_sensor, 0) * imu_lshin.inverse()).vec();
		Q_lshin = imu_lshin;
		if (P_lshin.y() < 0) {
			P_lshin.y() = 0;
		}

		P_rshin = P_rknee + (imu_rshin * Quaternionf(0, 0, -shin_sensor, 0) * imu_rshin.inverse()).vec();
		Q_rshin = imu_rshin;
		if (P_rshin.y() < 0) {
			P_rshin.y() = 0;
		}
	}

	Quaternionf Q_lshin;
	Quaternionf Q_rshin;
	Quaternionf Q_lthigh;
	Quaternionf Q_rthigh;
	Quaternionf Q_waist;

	Vector3f P_lshin;
	Vector3f P_rshin;
	Vector3f P_lthigh;
	Vector3f P_rthigh;
	Vector3f P_waist;

private:

	// Convert original IMU reporting frame to driver reporting frame (X right, Y up, Z back)
	Quaternionf convertFrame(const Quaternionf& imu, const Quaternionf& T_imu_driver) {
		Quaternionf T_enu_imu = T_driver * T_imu_driver.inverse();
		Quaternionf imu_zero = T_enu_imu.inverse() * imu;	// Align imu value to driver frame
		return T_imu_driver.inverse() * imu_zero * T_imu_driver;	// Change reporting frame to driver frame
	}

	// Find leveled offset quaternion
	Quaternionf findLevelOffset(const Quaternionf& imu) {
		Matrix3f imu_mat(imu);
		float yaw = atan2f(-imu_mat(2, 0), imu_mat(0, 0));
		Quaternionf leveled_quat = euYXZ_to_quat(yaw, 0, 0);
		return imu.inverse() * leveled_quat;
	}

	// Find offset quaternion to specified heading
	Quaternionf findHeadingOffset(const Quaternionf& imu, const Quaternionf& direction) {
		Matrix3f imu_mat(imu);
		float yaw = atan2f(-imu_mat(2, 0), imu_mat(0, 0));
		Quaternionf heading_quat = euYXZ_to_quat(yaw, 0, 0);
		return heading_quat.inverse() * direction;
	}

	Quaternionf T_driver;

	Quaternionf T_lshin;
	Quaternionf T_rshin;
	Quaternionf T_lthigh;
	Quaternionf T_rthigh;
	Quaternionf T_waist;

	Quaternionf offset_level_lshin;
	Quaternionf offset_level_rshin;
	Quaternionf offset_level_lthigh;
	Quaternionf offset_level_rthigh;
	Quaternionf offset_level_waist;

	Quaternionf offset_heading_lshin;
	Quaternionf offset_heading_rshin;
	Quaternionf offset_heading_lthigh;
	Quaternionf offset_heading_rthigh;
	Quaternionf offset_heading_waist;

	float shin;
	float thigh;
	float back;
	float head;
	float hip_width;
	float shin_sensor;
	float thigh_sensor;
	float waist_sensor;
};