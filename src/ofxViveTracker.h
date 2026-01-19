#pragma once

#include "ofMain.h"

// Backend selection:
// - macOS: Always uses libsurvive
// - Windows: Default is OpenVR. Define OFX_VIVE_USE_LIBSURVIVE to use libsurvive instead.
//
// To use libsurvive on Windows:
// 1. Add OFX_VIVE_USE_LIBSURVIVE to your project's preprocessor definitions
// 2. Link against survive.lib instead of openvr_api.lib
// 3. Copy libsurvive.dll and plugins/ folder to your bin/ directory

#if defined(__APPLE__) || defined(OFX_VIVE_USE_LIBSURVIVE)
#define USE_LIBSURVIVE
#endif

#ifdef USE_LIBSURVIVE
// Forward declarations for libsurvive
struct SurviveSimpleContext;
struct SurviveSimpleObject;
#else
#include <openvr.h>
#endif

class ofxViveTracker {
public:
	ofxViveTracker();
	~ofxViveTracker();

	bool setup();
	void update();
	void close();

	bool isConnected() const;
	bool isTracking() const;

	void setAutoReconnect(bool enable);
	void setReconnectInterval(float seconds);

	glm::vec3 getPosition() const;
	glm::quat getOrientation() const;
	glm::mat4 getMatrix() const;

	glm::vec3 getVelocity() const;
	glm::vec3 getAngularVelocity() const;

	// Get the name/serial of the connected tracker (libsurvive only)
	std::string getTrackerName() const;

private:
#ifdef USE_LIBSURVIVE
	SurviveSimpleContext* surviveCtx;
	const SurviveSimpleObject* trackerObject;
	std::string trackerName;
#else
	vr::IVRSystem* vrSystem;
	vr::TrackedDeviceIndex_t trackerIndex;
	vr::TrackedDevicePose_t pose;

	glm::mat4 convertMatrix(const vr::HmdMatrix34_t& mat);
	glm::quat matrixToQuat(const glm::mat4& mat);
#endif

	bool connected;
	bool tracking;
	bool autoReconnect;
	float reconnectInterval;
	float lastReconnectAttempt;

	glm::vec3 position;
	glm::quat orientation;
	glm::mat4 matrix;
	glm::vec3 velocity;
	glm::vec3 angularVelocity;

	bool findTracker();
	bool tryConnect();
	void updatePose();
};
