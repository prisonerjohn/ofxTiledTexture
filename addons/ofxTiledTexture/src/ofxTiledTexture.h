#ifndef _TILED_TEXTURE_H_
#define _TILED_TEXTURE_H_

#define PLAIN       0
#define RAINBOW     1
#define WAVE        2
#define GRAD_WAVE   3
#define GRAD        4

#define CRAZY       1
#define SMOOTH      2
#define SEQUENCE    3

#define MAX_OFFSET  16

#include "ofMain.h"

class ofxTiledTexture : public ofTexture {
    private:
        int     alphaMode;
        
        int     sizeMode;
        int     sizeOffset;
        bool    growing;
    
        int*    seq;
        int     numSeq;
        bool    newSeq;
        bool    isInSeq(int v);
    
	public:
        // constructor
                ofxTiledTexture();
    
        // overrides
        void    allocate(int w, int h, int internalGlDataType);
        void    allocate(int w, int h, int internalGlDataType, bool smooth);
        void    allocate(int w, int h, int internalGlDataType, bool bUseARBExtention, bool smooth);
        void    draw(float x, float y, float w, float h, float p, float q);   
        void    draw(float x, float y, float w, float h, float p, float q, int r, int g, int b, int alphaMin, int alphaMax, bool flicker, int sizeEase);
    
        // newbies
        void    setAlphaMode(int m);
        void    setSizeMode(int m);
    
}; 

#endif
