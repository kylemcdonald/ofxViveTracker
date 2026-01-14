#include "ofApp.h"

void ofApp::setup() {
	ofSetFrameRate(120);
	ofSetVerticalSync(false);
	ofBackground(255);

	canvas.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	clearCanvas();

	haveTL = haveTR = haveBL = false;
	planeReady = false;
	hasPrev = false;

	if (!tracker.setup()) {
		ofLogError() << "Failed to connect to Vive Tracker";
	}

	updateTitle();
}

void ofApp::update() {
	tracker.update();

	if (!tracker.isTracking()) {
		hasPrev = false;
		return;
	}

	glm::vec3 pos = tracker.getPosition();
	glm::vec2 screenPos;

	if (planeReady) {
		screenPos = projectToPlane(pos);
	} else {
		// Fallback: use X/Y directly with simple scaling
		float scale = 900.0f;
		screenPos.x = ofGetWidth() * 0.5f - pos.x * scale;
		screenPos.y = ofGetHeight() * 0.5f - pos.y * scale;
	}

	if (hasPrev) {
		canvas.begin();
		ofSetColor(0);
		ofSetLineWidth(3);
		ofDrawLine(prevPos, screenPos);
		canvas.end();
	}

	prevPos = screenPos;
	hasPrev = true;
}

void ofApp::draw() {
	ofSetColor(255);
	canvas.draw(0, 0);

	// Draw status
	ofSetColor(100);
	string status = "Tracker: ";
	status += tracker.isConnected() ? (tracker.isTracking() ? "Tracking" : "Connected (not tracking)") : "Not connected";
	ofDrawBitmapString(status, 20, 30);

	if (tracker.isTracking()) {
		glm::vec3 pos = tracker.getPosition();
		ofDrawBitmapString("Position: " + ofToString(pos.x, 3) + ", " + ofToString(pos.y, 3) + ", " + ofToString(pos.z, 3), 20, 50);
	}
}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_ESC) {
		ofExit();
	}

	if (key == 'c' || key == 'C') {
		clearCanvas();
	}

	if (key == 'f' || key == 'F') {
		ofToggleFullscreen();
	}

	if ((key == '1' || key == '2' || key == '3') && tracker.isTracking()) {
		glm::vec3 pos = tracker.getPosition();

		if (key == '1') {
			cornerTL = pos;
			haveTL = true;
		} else if (key == '2') {
			cornerTR = pos;
			haveTR = true;
		} else if (key == '3') {
			cornerBL = pos;
			haveBL = true;
		}

		if (haveTL && haveTR && haveBL) {
			calibratePlane();
		}

		updateTitle();
	}
}

void ofApp::calibratePlane() {
	planeOrigin = cornerTL;

	// X axis: TL to TR
	planeAxisX = glm::normalize(cornerTR - cornerTL);

	// Y axis: TL to BL, made perpendicular to X via Gram-Schmidt
	glm::vec3 yRaw = cornerBL - cornerTL;
	yRaw = yRaw - planeAxisX * glm::dot(yRaw, planeAxisX);
	planeAxisY = glm::normalize(yRaw);

	// Physical dimensions
	planeWidth = glm::dot(cornerTR - cornerTL, planeAxisX);
	planeHeight = glm::dot(cornerBL - cornerTL, planeAxisY);

	// Require minimum 5cm x 5cm
	planeReady = (planeWidth > 0.05f && planeHeight > 0.05f);

	hasPrev = false;
}

glm::vec2 ofApp::projectToPlane(const glm::vec3& pos) {
	glm::vec3 rel = pos - planeOrigin;

	float u = glm::dot(rel, planeAxisX);
	float v = glm::dot(rel, planeAxisY);

	float px = (u / planeWidth) * ofGetWidth();
	float py = (v / planeHeight) * ofGetHeight();

	return glm::vec2(px, py);
}

void ofApp::clearCanvas() {
	canvas.begin();
	ofClear(255, 255, 255, 255);
	canvas.end();
	hasPrev = false;
}

void ofApp::updateTitle() {
	string title = "Tracker Draw [TL:" + ofToString(haveTL ? 1 : 0);
	title += " TR:" + ofToString(haveTR ? 1 : 0);
	title += " BL:" + ofToString(haveBL ? 1 : 0);
	title += " ready:" + ofToString(planeReady ? 1 : 0);
	title += "] (1=TL 2=TR 3=BL C=clear Esc=quit)";
	ofSetWindowTitle(title);
}

void ofApp::windowResized(int w, int h) {
	if (w <= 0 || h <= 0) return;

	canvas.allocate(w, h, GL_RGBA);
	clearCanvas();
}
