#include "ofApp.h"

void ofApp::inittbl()
{
    // bipolar tables
    ta=new tbl();
    ta->setup(128);
    ta->dramp();

    // unipolar tables
    tb=new tbl();
    tb->setup(64);
    tb->pulse(.5);

    tc=new tbl();
    tc->setup(64);
    tc->uramp();
}

void ofApp::inittlo()
{
    o.setup(ta, 1.0, 1.0);
    oa.setup(tb, 1.0, 1.0);
    ort.setup(tc, 1.0, 4.0);
}

void ofApp::initfsm()
{
    s0.setup(400,HH-44,0);
    s1.setup(480,HH-44,1);
    s2.setup(550,HH-88,2);
    s3.setup(600,HH-44,3);
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetWindowTitle("rahasyn_ka");
    ofBackground(0);

    scopectr=0;
    fctr=0;
    ctr=0;
    state=0;

    fnt.load("OCRA", 16);
    fej.load("OCRA", 10);

    mgain=0.125;

    inittbl();
    inittlo();
    initfsm();

    soundsetup();
}

//--------------------------------------------------------------
void ofApp::update()
{
    ctr++;
    if(ctr%4==0) fctr++;
}

void ofApp::rndrfsm()
{
    s0.rndr(fnt, state);
    s1.rndr(fnt, state);
    s2.rndr(fnt, state);
    s3.rndr(fnt, state);

    spline2(s0.x,s0.y, s1.x,s1.y, (s0.x+s1.x)/2,s1.y-40, 18, "F1", fej);
    spline2(s1.x,s1.y, s2.x,s2.y, (s1.x+s2.x)/2-30,s2.y-20, 18, "F2", fej);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(22,202,232);
    fnt.drawString("rahasynth_ka", 22,44);

    float cy=HH/2;
    float cx=50;
    float ky=400;
    float kx=2;

    ofSetColor(23,202,232);
    for(int i=0;i<SCOP_SIZE;i++)
    {
        float xx=cx+i*kx;
        ofDrawLine(xx,cy,xx,cy-scope[i]*ky);
    }

    rndrfsm();
}

// ----------------------------------------- //
void ofApp::audioOut(ofSoundBuffer & outbuf)
{
    for (size_t i = 0; i < outbuf.getNumFrames(); i++)
    {
        int ni=i*outbuf.getNumChannels();
        int lch=ni;
        int rch=ni+1;

        float asig=o.samp();  // modded tlo signal

        float lv=mgain*asig;       // synth signal
        if(lv>.99) lv=.99;         // protection
        else if(lv<-.99) lv=-.99;  // valves

        scope[scopectr]=lv;
        scopectr=(scopectr+1)%SCOP_SIZE;

        outbuf[lch]=lv;
        outbuf[rch]=lv;

        o.evolve();              // audio rate osc
    }

    o.rate=ort.samp();
    o.amp=oa.samp();
    oa.evolve(); // kontrol
    ort.evolve(); // rate osc
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    cout<<key<<endl;
    if(state==0)
    {
        if(key==57344)
        {
            state=1;
        }
    }
    else if(state==1)
    {
        if(key==97)
        {
            state=2;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
