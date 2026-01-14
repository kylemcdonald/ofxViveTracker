#pragma once

#include "ofMain.h"
#include <openvr.h>

class ofxViveTracker {
public:
	ofxViveTracker();
	~ofxViveTracker();

	bool setup();
	void update();
	void close();

	bool isConnected() const;
	bool isTracking() const;

	glm::vec3 getPosition() const;
	glm::quat getOrientation() const;
	glm::mat4 getMatrix() const;

	glm::vec3 getVelocity() const;
	glm::vec3 getAngularVelocity() const;

private:
	vr::IVRSystem* vrSystem;
	vr::TrackedDeviceIndex_t trackerIndex;
	vr::TrackedDevicePose_t pose;

	bool connected;
	bool tracking;

	glm::vec3 position;
	glm::quat orientation;
	glm::mat4 matrix;
	glm::vec3 velocity;
	glm::vec3 angularVelocity;

	bool findTracker();
	void updatePose();
	glm::mat4 convertMatrix(const vr::HmdMatrix34_t& mat);
	glm::quat matrixToQuat(const glm::mat4& mat);
};
