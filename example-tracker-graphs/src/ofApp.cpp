#include "ofApp.h"

void Graph::addSample(const glm::vec3& sample, int maxSamples) {
	data.push_back(sample);
	while ((int)data.size() > maxSamples) {
		data.pop_front();
	}
}

void Graph::computeScale() {
	if (data.empty()) return;

	if (scaleMode == ScaleMode::MinMax) {
		// Find global min/max across all channels
		float globalMin = data[0][0];
		float globalMax = data[0][0];
		for (const auto& sample : data) {
			for (int i = 0; i < 3; i++) {
				globalMin = std::min(globalMin, sample[i]);
				globalMax = std::max(globalMax, sample[i]);
			}
		}
		// Ensure some range
		if (globalMax - globalMin < 0.001f) {
			globalMin -= 0.5f;
			globalMax += 0.5f;
		}
		// Use same scale for all channels
		for (int i = 0; i < 3; i++) {
			minVal[i] = globalMin;
			maxVal[i] = globalMax;
		}
	} else {
		// Symmetric: find absolute max across all channels
		float absMax = 0.001f;
		for (const auto& sample : data) {
			for (int i = 0; i < 3; i++) {
				absMax = std::max(absMax, std::abs(sample[i]));
			}
		}
		for (int i = 0; i < 3; i++) {
			minVal[i] = -absMax;
			maxVal[i] = absMax;
		}
	}

	// Compute auto units for position graph
	if (useAutoUnits) {
		float range = maxVal[0] - minVal[0];
		if (range < 0.1f) {
			// Less than 10cm, show mm
			unitMultiplier = 1000.0f;
			unitSuffix = "mm";
		} else if (range < 1.0f) {
			// Less than 1m, show cm
			unitMultiplier = 100.0f;
			unitSuffix = "cm";
		} else {
			// 1m or more, show m
			unitMultiplier = 1.0f;
			unitSuffix = "m";
		}
	}
}

void Graph::draw(float x, float y, float w, float h) {
	float margin = 60;
	float graphX = x + margin;
	float graphY = y + 25;
	float graphW = w - margin - 10;
	float graphH = h - 35;

	// Background
	ofSetColor(40);
	ofDrawRectangle(graphX, graphY, graphW, graphH);

	if (data.empty()) {
		// Title without data
		ofSetColor(200);
		ofDrawBitmapString(baseTitle, x + 10, y + 18);
		return;
	}

	computeScale();

	// Title with dynamic units
	ofSetColor(200);
	string displayTitle = baseTitle;
	if (useAutoUnits) {
		displayTitle += " (" + unitSuffix + ")";
	}
	ofDrawBitmapString(displayTitle, x + 10, y + 18);

	// Draw tick marks and labels for each channel
	ofColor colors[3] = { ofColor::cyan, ofColor::magenta, ofColor::yellow };

	// Draw Y-axis ticks (use channel 0 for reference, but show all)
	int numTicks = 5;
	ofSetColor(150);
	for (int t = 0; t <= numTicks; t++) {
		float ty = graphY + graphH - (t / (float)numTicks) * graphH;
		ofDrawLine(graphX - 5, ty, graphX, ty);

		// Show values for each channel at this tick position
		for (int ch = 0; ch < 3; ch++) {
			float val = minVal[ch] + (t / (float)numTicks) * (maxVal[ch] - minVal[ch]);
			if (useAutoUnits) {
				val *= unitMultiplier;
			}
			ofSetColor(colors[ch]);
			string valStr = ofToString(val, 1);
			if (ch == 0) {
				ofDrawBitmapString(valStr, x + 2, ty + 4);
			}
		}
	}

	// Draw center line for symmetric mode
	if (scaleMode == ScaleMode::Symmetric) {
		ofSetColor(80);
		float centerY = graphY + graphH * 0.5f;
		ofDrawLine(graphX, centerY, graphX + graphW, centerY);
	}

	// Draw data lines
	for (int ch = 0; ch < 3; ch++) {
		ofSetColor(colors[ch]);
		ofNoFill();
		ofBeginShape();
		for (size_t i = 0; i < data.size(); i++) {
			float px = graphX + graphW - (data.size() - 1 - i);
			float normalized = (data[i][ch] - minVal[ch]) / (maxVal[ch] - minVal[ch]);
			float py = graphY + graphH - normalized * graphH;
			ofVertex(px, py);
		}
		ofEndShape();
	}
	ofFill();

	// Legend
	float legendX = graphX + graphW - 80;
	float legendY = graphY + 15;
	const char* labels[3] = { "X", "Y", "Z" };
	for (int ch = 0; ch < 3; ch++) {
		ofSetColor(colors[ch]);
		ofDrawBitmapString(labels[ch], legendX + ch * 25, legendY);
	}
}

void ofApp::setup() {
	ofSetFrameRate(120);
	ofSetVerticalSync(false);
	ofBackground(30);

	graphWidth = ofGetWidth() / 2;

	positionGraph.baseTitle = "Position";
	positionGraph.scaleMode = ScaleMode::MinMax;
	positionGraph.useAutoUnits = true;

	orientationGraph.baseTitle = "Orientation (deg)";
	orientationGraph.scaleMode = ScaleMode::MinMax;

	velocityGraph.baseTitle = "Velocity (m/s)";
	velocityGraph.scaleMode = ScaleMode::Symmetric;

	angularVelocityGraph.baseTitle = "Angular Velocity (rad/s)";
	angularVelocityGraph.scaleMode = ScaleMode::Symmetric;

	if (!tracker.setup()) {
		ofLogError() << "Failed to connect to Vive Tracker";
	}

	ofSetWindowTitle("Tracker Graphs [F=fullscreen C=clear Esc=quit]");
}

void ofApp::update() {
	tracker.update();

	if (!tracker.isTracking()) return;

	int maxSamples = graphWidth;

	glm::vec3 pos = tracker.getPosition();
	positionGraph.addSample(pos, maxSamples);

	glm::quat q = tracker.getOrientation();
	glm::vec3 euler = glm::degrees(glm::eulerAngles(q));
	orientationGraph.addSample(euler, maxSamples);

	glm::vec3 vel = tracker.getVelocity();
	velocityGraph.addSample(vel, maxSamples);

	glm::vec3 angVel = tracker.getAngularVelocity();
	angularVelocityGraph.addSample(angVel, maxSamples);
}

void ofApp::draw() {
	float halfW = ofGetWidth() * 0.5f;
	float halfH = ofGetHeight() * 0.5f;

	// Top-left: Position
	positionGraph.draw(0, 0, halfW, halfH);

	// Top-right: Velocity
	velocityGraph.draw(halfW, 0, halfW, halfH);

	// Bottom-left: Orientation
	orientationGraph.draw(0, halfH, halfW, halfH);

	// Bottom-right: Angular Velocity
	angularVelocityGraph.draw(halfW, halfH, halfW, halfH);

	// Status bar
	ofSetColor(150);
	string status = "Tracker: ";
	status += tracker.isConnected() ? (tracker.isTracking() ? "Tracking" : "Connected (not tracking)") : "Not connected";
	ofDrawBitmapString(status, 10, ofGetHeight() - 10);
}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_ESC) {
		ofExit();
	}

	if (key == 'f' || key == 'F') {
		ofToggleFullscreen();
	}

	if (key == 'c' || key == 'C') {
		positionGraph.data.clear();
		orientationGraph.data.clear();
		velocityGraph.data.clear();
		angularVelocityGraph.data.clear();
	}
}

void ofApp::windowResized(int w, int h) {
	if (w <= 0 || h <= 0) return;
	graphWidth = w / 2;
}
