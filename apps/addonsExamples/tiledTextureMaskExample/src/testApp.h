#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxTiledTexture.h"

#define SCALAR 5.0f

const GLenum BLEND_MODES[] = { 
	GL_ZERO, 
	GL_ONE, 
	GL_SRC_COLOR, 
	GL_ONE_MINUS_SRC_COLOR, 
	GL_DST_COLOR, 
	GL_ONE_MINUS_DST_COLOR, 
	GL_SRC_ALPHA, 
	GL_ONE_MINUS_SRC_ALPHA, 
	GL_DST_ALPHA, 
	GL_ONE_MINUS_DST_ALPHA 
};

class testApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
    
        void keyPressed(int key);

        void reloadTextures();
		
    private:
        float           xTiles;
        float           yTiles;
        
        bool            linearSmooth;
    
        ofxTiledTexture tex;
        ofImage         bgImage;
    
        int  srcBlend;
        int  dstBlend;
    
        int  currAlphaMode;
        int  currSizeMode;
		
};

#endif
