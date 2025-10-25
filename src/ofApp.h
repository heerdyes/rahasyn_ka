#pragma once

#include "ofMain.h"
#include "synth.h"

#define SCOP_SIZE   (512)
#define HH          (ofGetHeight())
#define WW          (ofGetWidth())

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
        f0.drawString(ofToString(st), x-7,y+8);
    }
};

class ofApp : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseEntered(int x, int y);
        void mouseExited(int x, int y);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);

        void audioOut(ofSoundBuffer & outbuf);

        void soundsetup()
        {
            soundStream.printDeviceList();
            ofSoundStreamSettings settings;

            auto devices = soundStream.getMatchingDevices("default");
            if(!devices.empty())
            {
                settings.setOutDevice(devices[0]);
            }

            settings.setOutListener(this);
            settings.sampleRate = SMPL_RATE;
            settings.numOutputChannels = 2;
            settings.bufferSize = BUFR_SIZE;
            soundStream.setup(settings);
        }

        // refer project splinear for more splines
        void spline2(float x1,float y1, float x2,float y2, float p,float q, int n, string st, ofTrueTypeFont ft)
        {
            float mul=(1.0 / (float)n);
            float ox1=x1;
            float oy1=y1;

            for(int i=1;i<n+1;i++)
            {
                float d=mul*i;
                float zx1=x1*pow(1.0-d, 2) + 2*p*(1.0-d)*d + x2*pow(d, 2);
                float zy1=y1*pow(1.0-d, 2) + 2*q*(1.0-d)*d + y2*pow(d, 2);

                float dst=abs((fctr%n)-i);
                if(dst<3) ofSetColor(255,128,0);
                else ofSetColor(0,255,0);

                ofDrawLine(ox1,oy1, zx1,zy1);

                if(i==n/2)
                {
                    ofSetColor(23,202,232);
                    ft.drawString(st, zx1, zy1-8);
                }

                ox1=zx1;
                oy1=zy1;
            }
        }

        void initfsm();
        void rndrfsm();

        // table rendering
        void rndrtlo(float x, float y, float w, float h, int oi);

        ofSoundStream soundStream;
        ofTrueTypeFont fnt;
        ofTrueTypeFont fej;

        float scope[SCOP_SIZE];
        int scopectr;
        int ctr;
        int fctr;
        int state;

        // fsm
        nd s0,s1,s2,s3;

        syn z;
        int rbctr=0;
};
