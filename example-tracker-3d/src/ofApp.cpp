#include "ofApp.h"

void ofApp::setup() {
	ofSetFrameRate(120);
	ofSetVerticalSync(false);
	ofBackground(50);
	ofEnableDepthTest();

	// Set up camera - position it to see the tracking volume
	cam.setDistance(4.0f);
	cam.setNearClip(0.01f);
	cam.setFarClip(100.0f);

	if (!tracker.setup()) {
		ofLogError() << "Failed to connect to Vive Tracker";
	}

	ofSetWindowTitle("Tracker 3D [F=fullscreen C=clear trail Esc=quit]");
}

void ofApp::update() {
	tracker.update();

	if (tracker.isTracking()) {
		glm::vec3 pos = tracker.getPosition();
		trail.push_back(pos);
		while (trail.size() > maxTrailPoints) {
			trail.pop_front();
		}
	}
}

void ofApp::draw() {
	cam.begin();

	drawTrackingVolume();
	drawBaseStation();
	drawTrail();
	drawTracker();

	cam.end();

	// Draw status text
	ofDisableDepthTest();
	ofSetColor(200);
	string status = "Tracker: ";
	status += tracker.isConnected() ? (tracker.isTracking() ? "Tracking" : "Connected (not tracking)") : "Not connected";
	ofDrawBitmapString(status, 20, 30);

	if (tracker.isTracking()) {
		glm::vec3 pos = tracker.getPosition();
		ofDrawBitmapString("Position: " + ofToString(pos.x, 3) + ", " + ofToString(pos.y, 3) + ", " + ofToString(pos.z, 3), 20, 50);
	}
	ofEnableDepthTest();
}

void ofApp::drawBaseStation() {
	// Small black cube at origin representing base station
	ofSetColor(0);
	ofDrawBox(0, 0, 0, 0.05f, 0.05f, 0.05f);
}

void ofApp::drawTrackingVolume() {
	// Draw 2x2x2m wireframe cube centered at origin
	ofSetColor(100);
	ofNoFill();
	ofDrawBox(0, 1.0f, 0, 2.0f, 2.0f, 2.0f);
	ofFill();

	// Draw ground grid
	ofSetColor(60);
	float gridSize = 2.0f;
	float gridStep = 0.25f;
	for (float x = -gridSize / 2; x <= gridSize / 2; x += gridStep) {
		ofDrawLine(x, 0, -gridSize / 2, x, 0, gridSize / 2);
	}
	for (float z = -gridSize / 2; z <= gridSize / 2; z += gridStep) {
		ofDrawLine(-gridSize / 2, 0, z, gridSize / 2, 0, z);
	}

	// Draw axis lines at origin
	ofSetLineWidth(2);
	ofSetColor(255, 0, 0);
	ofDrawLine(0, 0, 0, 0.3f, 0, 0); // X - red
	ofSetColor(0, 255, 0);
	ofDrawLine(0, 0, 0, 0, 0.3f, 0); // Y - green
	ofSetColor(0, 0, 255);
	ofDrawLine(0, 0, 0, 0, 0, 0.3f); // Z - blue
	ofSetLineWidth(1);
}

void ofApp::drawTracker() {
	if (!tracker.isTracking()) return;

	glm::vec3 pos = tracker.getPosition();
	glm::mat4 mat = tracker.getMatrix();

	ofPushMatrix();

	// Apply position and orientation
	ofMultMatrix(mat);

	// Draw tracker as flattened cube (80x80x40mm) - Y is the short axis
	ofSetColor(0, 200, 255);
	ofDrawBox(0, 0, 0, 0.08f, 0.04f, 0.08f);

	// Draw local axes on tracker
	ofSetLineWidth(2);
	ofSetColor(255, 0, 0);
	ofDrawLine(0, 0, 0, 0.1f, 0, 0); // X
	ofSetColor(0, 255, 0);
	ofDrawLine(0, 0, 0, 0, 0.1f, 0); // Y
	ofSetColor(0, 0, 255);
	ofDrawLine(0, 0, 0, 0, 0, 0.1f); // Z
	ofSetLineWidth(1);

	ofPopMatrix();
}

void ofApp::drawTrail() {
	if (trail.size() < 2) return;

	ofSetColor(255, 200, 0);
	ofSetLineWidth(2);
	ofNoFill();
	ofBeginShape();
	for (const auto& pt : trail) {
		ofVertex(pt);
	}
	ofEndShape();
	ofSetLineWidth(1);
	ofFill();
}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_ESC) {
		ofExit();
	}

	if (key == 'f' || key == 'F') {
		ofToggleFullscreen();
	}

	if (key == 'c' || key == 'C') {
		trail.clear();
	}
}
