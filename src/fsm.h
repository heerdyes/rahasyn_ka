#pragma once

#include "ofMain.h"

class xywh
{
public:
    float x,y,w,h;
    
    void setup(float xx,float yy, float ww,float hh)
    {
        x=xx; y=yy;
        w=ww; h=hh;
    }

    xywh()
    {
        setup(0,0,0,0);
    }
};

class nd
{
public:
    float x,y;
    int st;
    ofColor c;
    float w;

    void setup(float xx,float yy,int s)
    {
        x=xx;
        y=yy;
        st=s;
        w=30;
        c.set(255,108,0);
    }

    void rndr(int fstate)
    {
        if(fstate==st) ofSetColor(c);
        else ofSetColor(23,202,232);

        ofDrawEllipse(x,y,w,w);
        ofSetColor(0);
        ofDrawEllipse(x,y,w-5,w-5);
        
        ofSetColor(23,202,232);
        ofDrawBitmapString(ofToString(st,2,'0'), x-8,y+5);
    }
};

