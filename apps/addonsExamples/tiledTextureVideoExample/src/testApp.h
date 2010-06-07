#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxTiledTexture.h"

class testApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		
    private:
        ofxTiledTexture tex;
        ofVideoGrabber  video;
		
};

#endif
