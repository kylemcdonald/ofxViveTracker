#include "ofxViveTracker.h"

ofxViveTracker::ofxViveTracker()
	: vrSystem(nullptr)
	, trackerIndex(vr::k_unTrackedDeviceIndexInvalid)
	, connected(false)
	, tracking(false)
	, position(0.0f)
	, orientation(1.0f, 0.0f, 0.0f, 0.0f)
	, matrix(1.0f)
	, velocity(0.0f)
	, angularVelocity(0.0f) {
}

ofxViveTracker::~ofxViveTracker() {
	close();
}

bool ofxViveTracker::setup() {
	vr::EVRInitError err = vr::VRInitError_None;
	vrSystem = vr::VR_Init(&err, vr::VRApplication_Background);

	if (err != vr::VRInitError_None || !vrSystem) {
		ofLogError("ofxViveTracker") << "VR_Init failed: " << vr::VR_GetVRInitErrorAsEnglishDescription(err);
		return false;
	}

	if (!findTracker()) {
		ofLogError("ofxViveTracker") << "No GenericTracker found in SteamVR";
		vr::VR_Shutdown();
		vrSystem = nullptr;
		return false;
	}

	connected = true;
	ofLogNotice("ofxViveTracker") << "Connected to tracker at index " << trackerIndex;
	return true;
}

void ofxViveTracker::update() {
	if (!connected || !vrSystem) {
		tracking = false;
		return;
	}

	updatePose();
}

void ofxViveTracker::close() {
	if (vrSystem) {
		vr::VR_Shutdown();
		vrSystem = nullptr;
	}
	connected = false;
	tracking = false;
	trackerIndex = vr::k_unTrackedDeviceIndexInvalid;
}

bool ofxViveTracker::isConnected() const {
	return connected;
}

bool ofxViveTracker::isTracking() const {
	return tracking;
}

glm::vec3 ofxViveTracker::getPosition() const {
	return position;
}

glm::quat ofxViveTracker::getOrientation() const {
	return orientation;
}

glm::mat4 ofxViveTracker::getMatrix() const {
	return matrix;
}

glm::vec3 ofxViveTracker::getVelocity() const {
	return velocity;
}

glm::vec3 ofxViveTracker::getAngularVelocity() const {
	return angularVelocity;
}

bool ofxViveTracker::findTracker() {
	for (vr::TrackedDeviceIndex_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
		if (!vrSystem->IsTrackedDeviceConnected(i)) continue;
		if (vrSystem->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_GenericTracker) {
			trackerIndex = i;
			return true;
		}
	}
	return false;
}

void ofxViveTracker::updatePose() {
	vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
	vrSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, poses, vr::k_unMaxTrackedDeviceCount);

	const vr::TrackedDevicePose_t& p = poses[trackerIndex];
	tracking = p.bPoseIsValid;

	if (!tracking) {
		return;
	}

	matrix = convertMatrix(p.mDeviceToAbsoluteTracking);

	position.x = p.mDeviceToAbsoluteTracking.m[0][3];
	position.y = p.mDeviceToAbsoluteTracking.m[1][3];
	position.z = p.mDeviceToAbsoluteTracking.m[2][3];

	orientation = matrixToQuat(matrix);

	velocity.x = p.vVelocity.v[0];
	velocity.y = p.vVelocity.v[1];
	velocity.z = p.vVelocity.v[2];

	angularVelocity.x = p.vAngularVelocity.v[0];
	angularVelocity.y = p.vAngularVelocity.v[1];
	angularVelocity.z = p.vAngularVelocity.v[2];
}

glm::mat4 ofxViveTracker::convertMatrix(const vr::HmdMatrix34_t& mat) {
	return glm::mat4(
		mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0f,
		mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0f,
		mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0f,
		mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f
	);
}

glm::quat ofxViveTracker::matrixToQuat(const glm::mat4& mat) {
	return glm::quat_cast(mat);
}
