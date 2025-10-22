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

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetWindowTitle("rahasyn_ka");
    ofBackground(0);

    scopectr=0;
    fnt.load("OCRA", 22);

    mgain=0.125;
    omode=0;

    inittbl();
    inittlo();

    soundsetup();
}

//--------------------------------------------------------------
void ofApp::update()
{}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(22,202,232);
    fnt.drawString("synthesizer esoteria", 50,50);

    float cy=ofGetHeight()/2;
    float cx=50;
    float ky=400;
    float kx=2;

    ofSetColor(255,144,11);
    for(int i=0;i<SCOP_SIZE;i++)
    {
        float xx=cx+i*kx;
        ofDrawLine(xx,cy,xx,cy-scope[i]*ky);
    }
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
    if(key==57344)
    {
        omode=0;
    }
    else if(key==57345)
    {
        omode=1;
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
