#include "ofxViveTracker.h"

#ifdef USE_LIBSURVIVE
#include <survive_api.h>
#include <thread>
#endif

ofxViveTracker::ofxViveTracker()
#ifdef USE_LIBSURVIVE
	: surviveCtx(nullptr)
	, trackerObject(nullptr)
#else
	: vrSystem(nullptr)
	, trackerIndex(vr::k_unTrackedDeviceIndexInvalid)
#endif
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
#ifdef USE_LIBSURVIVE
	if (surviveCtx) {
		survive_simple_close(surviveCtx);
		surviveCtx = nullptr;
	}
	connected = false;
	trackerObject = nullptr;
	trackerName.clear();

	// Initialize libsurvive with no arguments
	char* argv[] = { (char*)"ofxViveTracker", nullptr };
	surviveCtx = survive_simple_init(1, argv);

	if (!surviveCtx) {
		ofLogError("ofxViveTracker") << "Failed to initialize libsurvive";
		return false;
	}

	// Start the background tracking thread
	survive_simple_start_thread(surviveCtx);

	// Wait a moment for devices to be discovered
	float startTime = ofGetElapsedTimef();
	while (ofGetElapsedTimef() - startTime < 2.0f) {
		if (findTracker()) {
			connected = true;
			ofLogNotice("ofxViveTracker") << "Connected to tracker: " << trackerName;
			return true;
		}
		ofSleepMillis(100);
	}

	// No tracker found yet, but context is valid - can still poll
	ofLogWarning("ofxViveTracker") << "No tracker found during initial scan, will keep looking...";
	return false;

#else
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
#endif
}

void ofxViveTracker::update() {
	float now = ofGetElapsedTimef();

#ifdef USE_LIBSURVIVE
	if (!surviveCtx) {
		if (autoReconnect && (now - lastReconnectAttempt) >= reconnectInterval) {
			lastReconnectAttempt = now;
			tryConnect();
		}
		tracking = false;
		return;
	}

	// Check if libsurvive is still running
	if (!survive_simple_is_running(surviveCtx)) {
		ofLogNotice("ofxViveTracker") << "libsurvive stopped running";
		survive_simple_close(surviveCtx);
		surviveCtx = nullptr;
		connected = false;
		tracking = false;
		trackerObject = nullptr;
		trackerName.clear();
		return;
	}

	// Try to find a tracker if we don't have one
	if (!connected || !trackerObject) {
		if (autoReconnect && (now - lastReconnectAttempt) >= reconnectInterval) {
			lastReconnectAttempt = now;
			if (findTracker()) {
				connected = true;
				ofLogNotice("ofxViveTracker") << "Found tracker: " << trackerName;
			}
		}
		tracking = false;
		return;
	}

	updatePose();

#else
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
#endif
}

void ofxViveTracker::close() {
#ifdef USE_LIBSURVIVE
	if (surviveCtx) {
		// Store context and clear immediately to prevent further use
		SurviveSimpleContext* ctx = surviveCtx;
		surviveCtx = nullptr;
		trackerObject = nullptr;
		trackerName.clear();

		// Run close in detached thread to avoid blocking app exit
		// (survive_simple_close can hang waiting for USB I/O)
		std::thread([ctx]() {
			survive_simple_close(ctx);
		}).detach();
	}
	trackerObject = nullptr;
	trackerName.clear();
#else
	if (vrSystem) {
		vr::VR_Shutdown();
		vrSystem = nullptr;
	}
	trackerIndex = vr::k_unTrackedDeviceIndexInvalid;
#endif
	connected = false;
	tracking = false;
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

std::string ofxViveTracker::getTrackerName() const {
#ifdef USE_LIBSURVIVE
	return trackerName;
#else
	return "";
#endif
}

void ofxViveTracker::setAutoReconnect(bool enable) {
	autoReconnect = enable;
}

void ofxViveTracker::setReconnectInterval(float seconds) {
	reconnectInterval = seconds;
}

bool ofxViveTracker::findTracker() {
#ifdef USE_LIBSURVIVE
	if (!surviveCtx) return false;

	// Iterate through all objects to find a tracker
	for (const SurviveSimpleObject* obj = survive_simple_get_first_object(surviveCtx);
		 obj != nullptr;
		 obj = survive_simple_get_next_object(surviveCtx, obj)) {

		SurviveSimpleObject_type type = survive_simple_object_get_type(obj);
		SurviveSimpleSubobject_type subtype = survive_simple_object_get_subtype(obj);

		// Look for tracker devices
		if (type == SurviveSimpleObject_OBJECT) {
			if (subtype == SURVIVE_OBJECT_SUBTYPE_TRACKER ||
				subtype == SURVIVE_OBJECT_SUBTYPE_TRACKER_GEN2) {
				trackerObject = obj;
				trackerName = survive_simple_object_name(obj);
				return true;
			}
		}
	}
	return false;

#else
	for (vr::TrackedDeviceIndex_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
		if (!vrSystem->IsTrackedDeviceConnected(i)) continue;
		if (vrSystem->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_GenericTracker) {
			trackerIndex = i;
			return true;
		}
	}
	return false;
#endif
}

void ofxViveTracker::updatePose() {
#ifdef USE_LIBSURVIVE
	if (!surviveCtx || !trackerObject) {
		tracking = false;
		return;
	}

	SurvivePose pose;
	FLT timecode = survive_simple_object_get_latest_pose(trackerObject, &pose);

	// Check if we have a valid pose (timecode > 0 means we got data)
	if (timecode <= 0) {
		tracking = false;
		return;
	}

	tracking = true;

	// Coordinate transform from libsurvive to our system: (X, Y, Z) -> (X, Z, -Y)
	// This is a +90° rotation around X, combined with -90° body frame correction for orientation
	static const float c = 0.7071067811865476f;  // cos(45°) = sin(45°) = √2/2

	position = glm::vec3(pose.Pos[0], pose.Pos[2], -pose.Pos[1]);

	orientation.w = c * (pose.Rot[0] + pose.Rot[1]);
	orientation.x = c * (pose.Rot[1] - pose.Rot[0]);
	orientation.y = c * (pose.Rot[2] + pose.Rot[3]);
	orientation.z = c * (pose.Rot[3] - pose.Rot[2]);

	matrix = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation);

	SurviveVelocity vel;
	survive_simple_object_get_latest_velocity(trackerObject, &vel);

	velocity = glm::vec3(vel.Pos[0], vel.Pos[2], -vel.Pos[1]);
	angularVelocity = glm::vec3(vel.AxisAngleRot[0], vel.AxisAngleRot[2], -vel.AxisAngleRot[1]);

#else
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
#endif
}

#ifndef USE_LIBSURVIVE
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
#endif
