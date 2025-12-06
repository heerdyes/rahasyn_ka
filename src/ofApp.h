#pragma once

#include "ofMain.h"

#define HH          (ofGetHeight())
#define WW          (ofGetWidth())

#include "colors.h"
#include "fsm.h"
#include "uts.h"
#include "ftbl.h"
#include "synth.h"
#include "hwif.h"

#define SCOP_SIZE   (1024)

class ofApp : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();
        void exit();

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
        void kpevt(int key);
        void sourcerhka(string kafn);

        ofSoundStream soundStream;

        float scope[SCOP_SIZE];
        int scopectr;
        int ctr;
        int state;

        // fsm
        nd s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11;
        nd s12,s13,s14,s15,s16,s17,s18,s19,s20,s21;
        nd s22,s23,s24,s25,s26,s27;
        nd s28,s29,s30,s32,s33,s34,s35,s37;
        nd s31,s36; // unused

        syn z;
        int rbctr=0;
        string numtok;

        ut u;
        cstk S;
        lg L;
        
        // cameraman
        ofVideoGrabber vg;
        ofTexture vtx;
        ofPixels vfx;
		int camw;
		int camh;
		float camx=0;
		float camy=0;
		float vx=1;
		float vy=1;
		float vscale=1.0;
		bool isdvdss=true;
		int camclk=0;
		bool camon=false;
		
		float arg1,arg2;
		char bank='a';
		string cmdlog;
		
		// pico i/f
		rp2 pko;
};

