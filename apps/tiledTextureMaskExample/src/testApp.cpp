#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
    linearSmooth = false;
    reloadTextures();
    
    srcBlend = 8;
    dstBlend = 9;
    
    currAlphaMode = 0;
    currSizeMode = 0;
}

//--------------------------------------------------------------
void testApp::update() {
}

//--------------------------------------------------------------
void testApp::draw() {
    xTiles = ofGetWidth()/float(ABS(mouseX)+1);
    xTiles = xTiles*(ofGetWidth()/SCALAR);
    xTiles = xTiles/ofGetWidth();
    
    yTiles = ofGetHeight()/float(ABS(mouseY)+1);
    yTiles = yTiles*(ofGetWidth()/SCALAR);
    yTiles = yTiles/ofGetWidth();
    
    // draw the tiles
    tex.draw(0, 0, ofGetWidth(), ofGetHeight(), xTiles, yTiles);
    
    glEnable(GL_BLEND);
    {
        glBlendFunc(BLEND_MODES[srcBlend], BLEND_MODES[dstBlend]);
        // draw the background image
        bgImage.draw(0, 0, ofGetWidth(), ofGetHeight()); 
    }
    glDisable(GL_BLEND);
    
    // print some instructions
    ofSetColor(0);
    ofFill();
    ofRect(20, ofGetHeight() - 20 - 80, 355, 85);
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("[s] toggles LINEAR SMOOTH\n[f] cycles through SIZE MODE\n[up/down] cycles through SRC BLEND MODE\n[left/right] cycles through DST BLEND MODE\n[</>] cycles through ALPHA MODE", 
                       30, ofGetHeight() - 20 - 60);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
    switch (key) {
        case 's':
            linearSmooth = !linearSmooth;
        case ' ':
            reloadTextures();
            break;
        
        case 'f':
            currSizeMode = (currSizeMode+1)%4;
            tex.setSizeMode(currSizeMode);
            break;
        
        case OF_KEY_UP:
            srcBlend = (srcBlend+1)%10;
            break;
        case OF_KEY_DOWN:
            srcBlend = (srcBlend-1)%10;
            break;
            
        case OF_KEY_RIGHT:
            dstBlend = (dstBlend+1)%10;
            break;
        case OF_KEY_LEFT:
            dstBlend = (dstBlend-1)%10;
            break;
            
        case ',':
        case '<':
            currAlphaMode = (currAlphaMode-1)%5;
            tex.setAlphaMode(currAlphaMode);
            break;
        case '.':
        case '>':
            currAlphaMode = (currAlphaMode+1)%5;
            tex.setAlphaMode(currAlphaMode);
            break;
    }
    
    printf("src:%d\t\tdst:%d\t\talpha:%d\t\tsize:%d\n", srcBlend, dstBlend, currAlphaMode, currSizeMode);
}

//--------------------------------------------------------------
void testApp::reloadTextures() {
    // load pattern
    ofImage patternImage;
    patternImage.loadImage("pattern.png");
    tex.allocate(patternImage.width, patternImage.height, GL_RGBA, linearSmooth); 
    tex.loadData(patternImage.getPixels(), patternImage.width, patternImage.height, GL_RGBA); 
    
    // load image
    bgImage.loadImage("background.jpg");
}