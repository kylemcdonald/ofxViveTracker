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

	void drawBaseStation();
	void drawTrackingVolume();
	void drawTracker();
	void drawTrail();
};
