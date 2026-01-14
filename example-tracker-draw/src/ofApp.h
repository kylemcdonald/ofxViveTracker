#pragma once

#include "ofMain.h"
#include "ofxViveTracker.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void windowResized(int w, int h);

private:
	ofxViveTracker tracker;
	ofFbo canvas;

	// Plane calibration
	bool haveTL, haveTR, haveBL;
	glm::vec3 cornerTL, cornerTR, cornerBL;

	bool planeReady;
	glm::vec3 planeOrigin;
	glm::vec3 planeAxisX;
	glm::vec3 planeAxisY;
	float planeWidth;
	float planeHeight;

	// Drawing state
	bool hasPrev;
	glm::vec2 prevPos;

	void calibratePlane();
	glm::vec2 projectToPlane(const glm::vec3& pos);
	void clearCanvas();
	void updateTitle();
};
