#pragma once

#include "ofMain.h"

#define SMPL_RATE   (44100)
#define BUFR_SIZE   (512)
#define SCOP_SIZE   (512)
#define TBL_MAX_N   (16384)

class tbl
{
public:
    float buf[TBL_MAX_N];
    int sz;
    float ptr;

    tbl()
    {
        ptr=0.0;
    }

    void setup(int n)
    {
        if(n>TBL_MAX_N || n<0) sz=TBL_MAX_N;
        else sz=n;
    }

    float sampnow() { return buf[(int)round(ptr)]; }

    void mvptr(float r)
    {
        float _ptr=ptr+r;
        if(_ptr>sz-1) _ptr-=sz-1;
        ptr=_ptr;
    }

    void uramp()
    {
        float m=1.0 / (float)sz;
        float smp=0.0;
        for(int i=0;i<sz;i++)
        {
            buf[i]=smp;
            smp+=m;
        }
    }

    void dramp()
    {
        float m=2.0 / (float)sz;
        float smp=1.0;
        for(int i=0;i<sz;i++)
        {
            buf[i]=smp;
            smp-=m;
        }
    }

    void pulse(float _pwm=.5)
    {
        float pwm=(_pwm<0 || _pwm>1)?.5:_pwm;
        for(int i=0;i<sz;i++)
        {
            if(i<pwm*sz) buf[i]=1.0;
            else buf[i]=0.0;
        }
    }

    void bi2uni()
    {
        for(int i=0;i<sz;i++)
        {
            buf[i]=(1.0+buf[i])/2;
        }
    }
};

class tlo
{
public:
    tbl *t;
    float rate;
    float amp;

    tlo(){}

    void setup(tbl *tt, float r0, float a0)
    {
        t=tt;
        rate=r0;
        amp=a0;
    }

    void evolve() { t->mvptr(rate); }

    float samp() { return amp*t->sampnow(); }
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

        void inittbl();
        void inittlo();

        ofSoundStream soundStream;
        ofTrueTypeFont fnt;

        float scope[SCOP_SIZE];
        int scopectr;
        int octr;
        float mgain;

        // tables
        tbl *ta;
        tbl *tb;
        tbl *tc;

        // oscillators
        tlo o;
        tlo oa;
        tlo ort;
        int omode;
};
