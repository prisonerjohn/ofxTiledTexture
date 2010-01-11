#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
    video.initGrabber(160, 120);
    tex.allocate(video.width, video.height, GL_RGB); 
}

//--------------------------------------------------------------
void testApp::update() {
    video.grabFrame();
	if (video.isFrameNew()) {
		tex.loadData(video.getPixels(), video.width, video.height, GL_RGB);
    }
}

//--------------------------------------------------------------
void testApp::draw() {
    tex.draw(0, 0, ofGetWidth(), ofGetHeight(), ofGetWidth()/float(ABS(mouseX)+1), ofGetHeight()/float(ABS(mouseY)+1));
}