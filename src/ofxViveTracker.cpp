#include "ofxViveTracker.h"

ofxViveTracker::ofxViveTracker()
	: vrSystem(nullptr)
	, trackerIndex(vr::k_unTrackedDeviceIndexInvalid)
	, connected(false)
	, tracking(false)
	, autoReconnect(true)
	, reconnectInterval(2.0f)
	, lastReconnectAttempt(-10.0f)
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
	lastReconnectAttempt = ofGetElapsedTimef();
	return tryConnect();
}

bool ofxViveTracker::tryConnect() {
	if (vrSystem) {
		vr::VR_Shutdown();
		vrSystem = nullptr;
	}
	connected = false;

	vr::EVRInitError err = vr::VRInitError_None;
	vrSystem = vr::VR_Init(&err, vr::VRApplication_Background);

	if (err != vr::VRInitError_None || !vrSystem) {
		vrSystem = nullptr;
		return false;
	}

	if (!findTracker()) {
		vr::VR_Shutdown();
		vrSystem = nullptr;
		return false;
	}

	connected = true;
	ofLogNotice("ofxViveTracker") << "Connected to tracker at index " << trackerIndex;
	return true;
}

void ofxViveTracker::update() {
	float now = ofGetElapsedTimef();

	// Case 1: Not connected to SteamVR at all
	if (!vrSystem) {
		if (autoReconnect && (now - lastReconnectAttempt) >= reconnectInterval) {
			lastReconnectAttempt = now;
			tryConnect();
		}
		tracking = false;
		return;
	}

	// Poll for VR events to detect SteamVR shutdown
	vr::VREvent_t event;
	while (vrSystem->PollNextEvent(&event, sizeof(event))) {
		if (event.eventType == vr::VREvent_Quit) {
			ofLogNotice("ofxViveTracker") << "SteamVR is shutting down";
			vr::VR_Shutdown();
			vrSystem = nullptr;
			connected = false;
			tracking = false;
			trackerIndex = vr::k_unTrackedDeviceIndexInvalid;
			return;
		}
	}

	// Case 2: Connected to SteamVR but no tracker found
	if (!connected) {
		if (autoReconnect && (now - lastReconnectAttempt) >= reconnectInterval) {
			lastReconnectAttempt = now;
			if (findTracker()) {
				connected = true;
				ofLogNotice("ofxViveTracker") << "Reconnected to tracker at index " << trackerIndex;
			}
		}
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

void ofxViveTracker::setAutoReconnect(bool enable) {
	autoReconnect = enable;
}

void ofxViveTracker::setReconnectInterval(float seconds) {
	reconnectInterval = seconds;
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

	// Check if device disconnected
	if (!p.bDeviceIsConnected) {
		ofLogWarning("ofxViveTracker") << "Tracker disconnected";
		connected = false;
		tracking = false;
		trackerIndex = vr::k_unTrackedDeviceIndexInvalid;
		return;
	}

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
