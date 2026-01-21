#pragma once

#include "ofMain.h"
#include "ofxViveTracker.h"
#include <deque>

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);

private:
	ofxViveTracker tracker;
	ofEasyCam cam;

	std::deque<glm::vec3> trail;
	static const int maxTrailPoints = 100;

	glm::vec3 positionOffset{0.0f};  // Offset to center tracker in visualization

	void drawBaseStation();
	void drawTrackingVolume();
	void drawTracker();
	void drawTrail();
};
