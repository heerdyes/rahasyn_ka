#pragma once

#include "ofMain.h"

#define HH          (ofGetHeight())
#define WW          (ofGetWidth())

#include "fsm.h"
#include "uts.h"
#include "ftbl.h"
#include "synth.h"

#define SCOP_SIZE   (1024)

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

        void initfsm();
        void rndrfsm();

        ofSoundStream soundStream;

        float scope[SCOP_SIZE];
        int scopectr;
        int ctr;
        int state;

        // fsm
        nd s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11;

        syn z;
        int rbctr=0;
        string numtok;

        ut u;
        cstk S;
        lg L;
        
        ofVideoGrabber vidGrabber;
		int camWidth;
		int camHeight;
		float camx=0;
		float camy=0;
		float vx=1;
		float vy=1;
		float vscale=.5;
		bool isdvdss=false;
};

