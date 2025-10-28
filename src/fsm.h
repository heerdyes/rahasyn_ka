#pragma once

#include "ofMain.h"

class xywh
{
public:
    float x,y,w,h;

    xywh()
    {
        x=y=w=h=0;
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

    void rndr(ofTrueTypeFont f0, int fstate)
    {
        if(fstate==st) ofSetColor(c);
        else ofSetColor(23,202,232);

        ofDrawEllipse(x,y,w,w);
        ofSetColor(0);
        f0.drawString(ofToString(st,2,'0'), x-10,y+5);
    }
};
