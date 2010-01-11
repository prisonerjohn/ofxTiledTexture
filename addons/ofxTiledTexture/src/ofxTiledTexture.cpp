#include "ofxTiledTexture.h"

//----------------------------------------------------------
ofxTiledTexture::ofxTiledTexture() {
    alphaMode = PLAIN;
    sizeMode = PLAIN;
    
    sizeOffset = 0;
    growing = true;
    seq = NULL;
    newSeq = false;
}

//----------------------------------------------------------
void ofxTiledTexture::allocate(int w, int h, int internalGlDataType) {
    allocate(w, h, internalGlDataType, ofGetUsingArbTex(), true);
}

//----------------------------------------------------------
void ofxTiledTexture::allocate(int w, int h, int internalGlDataType, bool smooth) {
    allocate(w, h, internalGlDataType, ofGetUsingArbTex(), smooth);
}
    
//----------------------------------------------------------
void ofxTiledTexture::allocate(int w, int h, int internalGlDataType, bool bUseARBExtention, bool smooth) {   
    
    //our graphics card might not support arb so we have to see if it is supported.
#ifndef TARGET_OF_IPHONE
    if (bUseARBExtention && GLEE_ARB_texture_rectangle){
        texData.tex_w = w;
        texData.tex_h = h;
        texData.tex_t = w;
        texData.tex_u = h;
        texData.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    } else 
#endif
	{
		//otherwise we need to calculate the next power of 2 for the requested dimensions
		//ie (320x240) becomes (512x256)
		texData.tex_w = ofNextPow2(w);
		texData.tex_h = ofNextPow2(h);
		texData.tex_t = 1.0f;
		texData.tex_u = 1.0f;
		texData.textureTarget = GL_TEXTURE_2D;
	}
    
	texData.glTypeInternal = internalGlDataType;
    
    // MEMO: todo, add more types
	switch(texData.glTypeInternal) {
#ifndef TARGET_OPENGLES	
		case GL_RGBA32F_ARB:
		case GL_RGBA16F_ARB:
			texData.glType		= GL_RGBA;
			texData.pixelType	= GL_FLOAT;
			break;
			
		case GL_RGB32F_ARB:
			texData.glType		= GL_RGB;
			texData.pixelType	= GL_FLOAT;
			break;
            
		case GL_LUMINANCE32F_ARB:
			texData.glType		= GL_LUMINANCE;
			texData.pixelType	= GL_FLOAT;
			break;
#endif			
			
		default:
			texData.glType		= GL_LUMINANCE;
			texData.pixelType	= GL_UNSIGNED_BYTE;
	}
    
	// attempt to free the previous bound texture, if we can:
	clear();
    
	glGenTextures(1, (GLuint *)&texData.textureID);   // could be more then one, but for now, just one
    
	glEnable(texData.textureTarget);
    
    glBindTexture(texData.textureTarget, (GLuint)texData.textureID);
#ifndef TARGET_OPENGLES
    // can't do this on OpenGL ES: on full-blown OpenGL, 
    // internalGlDataType and glDataType (GL_LUMINANCE below)
    // can be different; on ES they must be exactly the same.
    //		glTexImage2D(texData.textureTarget, 0, texData.glTypeInternal, (GLint)texData.tex_w, (GLint)texData.tex_h, 0, GL_LUMINANCE, PIXEL_TYPE, 0);  // init to black...
    glTexImage2D(texData.textureTarget, 0, texData.glTypeInternal, (GLint)texData.tex_w, (GLint)texData.tex_h, 0, texData.glType, texData.pixelType, 0);  // init to black...
#else
    glTexImage2D(texData.textureTarget, 0, texData.glTypeInternal, texData.tex_w, texData.tex_h, 0, texData.glTypeInternal, GL_UNSIGNED_BYTE, 0);
#endif
    
    glTexParameterf(texData.textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(texData.textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (smooth) {
        glTexParameterf(texData.textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(texData.textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameterf(texData.textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(texData.textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
	glDisable(texData.textureTarget);
    
	texData.width = w;
	texData.height = h;
    texData.bFlipTexture = false;
	texData.bAllocated = true;
}

//----------------------------------------------------------
void ofxTiledTexture::draw(float x, float y, float w, float h, float p, float q) {
    
	glEnable(texData.textureTarget);
    
	// bind the texture
	glBindTexture(texData.textureTarget, (GLuint)texData.textureID);
    
    GLint px0 = 0;		// up to you to get the aspect ratio right
    GLint py0 = 0;
    GLint px1 = (GLint)w;
    GLint py1 = (GLint)h;
    
    if (texData.bFlipTexture == true){
        GLint temp = (GLint)py0;
        py0 = py1;
        py1 = temp;
    }
    
    // for rect mode center, let's do this:
    if (ofGetRectMode() == OF_RECTMODE_CENTER){
        px0 = -w/2;
        py0 = -h/2;
        px1 = +w/2;
        py1 = +h/2;
    }
    
    //we translate our drawing points by our anchor point.
    //we still respect ofRectMode so if you have rect mode set to
    //OF_RECTMODE_CENTER your anchor will be relative to that.
    GLfloat anchorX;
    GLfloat anchorY;
    
    if(bAnchorIsPct){
        anchorX = anchor.x * w;
        anchorY = anchor.y * h;
    }else{
        anchorX = anchor.x;
        anchorY = anchor.y;
    }
    
    px0 -= anchorX;
    py0 -= anchorY;
    px1 -= anchorX;
    py1 -= anchorY;
    
    
    // -------------------------------------------------
    // complete hack to remove border artifacts.
    // slightly, slightly alters an image, scaling...
    // to remove the border.
    // we need a better solution for this, but
    // to constantly add a 2 pixel border on all uploaded images
    // is insane..
    
    GLfloat offsetw = 0;
    GLfloat offseth = 0;
    
    if (texData.textureTarget == GL_TEXTURE_2D /* && bTexHackEnabled */) {
        offsetw = 1.0f / (texData.tex_w);
        offseth = 1.0f / (texData.tex_h);
    }
    // -------------------------------------------------
    
    GLfloat tx0 = 0+offsetw;
    GLfloat ty0 = 0+offseth;
    GLfloat tx1 = texData.tex_t - offsetw;
    GLfloat ty1 = texData.tex_u - offseth;
    
	glPushMatrix(); 
	{
        glTranslatef(x,y,0.0f);
        
        if (sizeMode == SEQUENCE && newSeq) {
            // delete the old sequence, if any
            if (seq != NULL) {
                delete [] seq;
            }
            
            // create a new sequence
            numSeq = ofRandom(1, p*q);
            seq = new int[numSeq];
            for (int i=0; i < numSeq; i++) {
                seq[i] = ofRandom(0, p*q-1);
            }
            
            newSeq = false;
        }
        
        if (sizeMode == SMOOTH || sizeMode == SEQUENCE) {
            if (growing) {
                sizeOffset += 1;
            } else {
                sizeOffset -= 1;
            }
            
            if (sizeOffset > MAX_OFFSET) {
                sizeOffset = MAX_OFFSET;
                growing = false;
            } else if (sizeOffset < 0) {
                sizeOffset = 0;
                growing = true;
                newSeq = true;
            } 
        }
        
        float s = (px1-px0)/p;
        float t = (py1-py0)/q;
        for (int j = 0; j < q; j++) {
            for (int i = 0; i < p; i++) {
                if (alphaMode == RAINBOW) {
                    ofSetColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));
                } else if (alphaMode == WAVE) {
                    float a = fabsf(sinf(i/10.0f)*255);
                    ofSetColor(255, 255, 255, a);
                } else if (alphaMode == GRAD_WAVE) {
                    float d = sqrt(p);
                    float a = fabsf(sinf(i)*i/d)*255/d;
                    ofSetColor(255, 255, 255, a);
                } else if (alphaMode == GRAD) {
                    ofSetColor(255, 255, 255, i*255/p);
                }
                        
                if (sizeMode == CRAZY && (ofGetFrameNum()*i*j)%2 == 0) {
                    // flickering grid
                    sizeOffset = ofRandom(0, ofGetWidth()/p);   
                    glBegin(GL_QUADS);
                    glTexCoord2f(tx0,ty0);          glVertex3i(px0+i*s     -sizeOffset,  py0+j*t     -sizeOffset,  0);
                    glTexCoord2f(tx1,ty0);          glVertex3i(px0+(i+1)*s +sizeOffset,  py0+j*t     -sizeOffset,  0);
                    glTexCoord2f(tx1,ty1);          glVertex3i(px0+(i+1)*s +sizeOffset,  py0+(j+1)*t +sizeOffset,  0);
                    glTexCoord2f(tx0,ty1);          glVertex3i(px0+i*s     -sizeOffset,  py0+(j+1)*t +sizeOffset,  0);
                    glEnd();
                    
                } else if ((sizeMode == SMOOTH && (i+j)%2 == 0) || (sizeMode == SEQUENCE && isInSeq(j*p+i))) {
                    glBegin(GL_QUADS);
                    glTexCoord2f(tx0,ty0);          glVertex3i(px0+i*s     -sizeOffset,  py0+j*t     -sizeOffset,  0);
                    glTexCoord2f(tx1,ty0);          glVertex3i(px0+(i+1)*s +sizeOffset,  py0+j*t     -sizeOffset,  0);
                    glTexCoord2f(tx1,ty1);          glVertex3i(px0+(i+1)*s +sizeOffset,  py0+(j+1)*t +sizeOffset,  0);
                    glTexCoord2f(tx0,ty1);          glVertex3i(px0+i*s     -sizeOffset,  py0+(j+1)*t +sizeOffset,  0);
                    glEnd();
                    
                } else {
                    // normal grid
                    glBegin(GL_QUADS);
                    glTexCoord2f(tx0,ty0);          glVertex3i(px0+i*s,      py0+j*t,      0);
                    glTexCoord2f(tx1,ty0);			glVertex3i(px0+(i+1)*s,  py0+j*t,      0);
                    glTexCoord2f(tx1,ty1);			glVertex3i(px0+(i+1)*s,  py0+(j+1)*t,  0);
                    glTexCoord2f(tx0,ty1);			glVertex3i(px0+i*s,      py0+(j+1)*t,  0);
                    glEnd();
                }            
            }
        }
    }
    glPopMatrix();
    
	glDisable(texData.textureTarget);
}

//----------------------------------------------------------
bool ofxTiledTexture::isInSeq(int v) {
    for (int i=0; i < numSeq; i++) {
        if (seq[i] == v)
            return true;
    }
    return false;
}

//----------------------------------------------------------
void ofxTiledTexture::setAlphaMode(int m) {
    alphaMode = m;
}

//----------------------------------------------------------
void ofxTiledTexture::setSizeMode(int m) {
    sizeMode = m;
    if (sizeMode == SEQUENCE) {
        newSeq = true;
    }
}
