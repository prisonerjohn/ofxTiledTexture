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
    glDisable(GL_BLEND);
    
    xTiles = ofGetWidth()/float(ABS(mouseX)+1);
    xTiles = xTiles*(ofGetWidth()/SCALAR);
    xTiles = xTiles/ofGetWidth();
    
    yTiles = ofGetHeight()/float(ABS(mouseY)+1);
    yTiles = yTiles*(ofGetWidth()/SCALAR);
    yTiles = yTiles/ofGetWidth();
    
    tex.draw(0, 0, ofGetWidth(), ofGetHeight(), xTiles, yTiles);
    
    glEnable(GL_BLEND);
    glBlendFunc(BLEND_MODES[srcBlend], BLEND_MODES[dstBlend]);
    bgImage.draw(0, 0, ofGetWidth(), ofGetHeight());    
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