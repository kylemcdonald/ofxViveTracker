#pragma once

#include "ofMain.h"
#include "ofxViveTracker.h"
#include <deque>

enum class ScaleMode {
	MinMax,      // Position, Orientation: scale min/max separately
	Symmetric    // Velocity, Angular Velocity: scale to absolute max
};

struct Graph {
	string baseTitle;
	std::deque<glm::vec3> data;
	ScaleMode scaleMode;
	float minVal[3];
	float maxVal[3];
	bool useAutoUnits;
	float unitMultiplier;
	string unitSuffix;

	Graph() : scaleMode(ScaleMode::MinMax), useAutoUnits(false), unitMultiplier(1.0f) {
		for (int i = 0; i < 3; i++) {
			minVal[i] = 0;
			maxVal[i] = 0;
		}
	}

	void addSample(const glm::vec3& sample, int maxSamples);
	void computeScale();
	void draw(float x, float y, float w, float h);
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void windowResized(int w, int h);

private:
	ofxViveTracker tracker;

	Graph positionGraph;
	Graph orientationGraph;
	Graph velocityGraph;
	Graph angularVelocityGraph;

	int graphWidth;
};
