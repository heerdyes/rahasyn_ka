#include "ofApp.h"

void ofApp::initfsm()
{
    s0.setup(400,HH-44,0);
    s1.setup(480,HH-88,1);
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

    initfsm();

    z.setup();
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
    spline2(s1.x,s1.y, s2.x,s2.y, (s1.x+s2.x)/2-30,s2.y-20, 18, "a-z", fej);
    spline2(s2.x,s2.y, s3.x,s3.y, (s2.x+s3.x)/2+30,s3.y-20, 18, "a-z", fej);
    spline2(s3.x,s3.y, s0.x,s0.y, (s3.x+s0.x)/2-30,s0.y-20, 18, "a-z", fej);
}

void ofApp::rndrtlo(float x, float y, float w, float h, int oi)
{
    ofSetColor(23,202,232);
    int tsz=z.getosctblsz(oi);
    float xf=w / (float) tsz;
    float xx=x-w/2;
    float kk=(h/2)*z.getoscamp(oi);

    for(int i=0;i<tsz;i++)
    {
        float yy=y - kk*z.getosctblsamp(oi,i);
        ofDrawLine(xx,y,xx,yy);
        xx+=xf;
    }

    ofSetColor(255,88,0);
    float xp=x+z.getoscptr(oi) * w / (float) tsz;
    ofDrawLine(xp,y+h/2,xp,y-h/2);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(22,202,232);
    fnt.drawString("rahasynth_ka", 22,44);

    float cy=HH/2;
    float cgy=cy-100;
    float wd=800;
    float ht=400;
    float cx=WW/2-wd/2;
    float ky=ht/2;
    float kx=wd/SCOP_SIZE;

    ofSetColor(23,202,232);
    for(int i=0;i<SCOP_SIZE;i++)
    {
        float xx=cx+i*kx;
        ofDrawLine(xx,cgy,xx,cgy-scope[i]*ky);
    }

    // rndr 26 tlos
    float rr=300;
    float frac=2*PI/NTLO;
    float xmid=WW/2;
    for(int i=0;i<NTLO;i++)
    {
        float xq=xmid+1.6*rr*cos(i*frac);
        float yq=cgy-.9*rr*sin(i*frac);
        rndrtlo(xq,yq, 80,45, i);
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

        float lv=z.vsamp();

        if(lv>.99) lv=.99;         // protection
        else if(lv<-.99) lv=-.99;  // valves

        scope[scopectr]=lv;
        scopectr=(scopectr+1)%SCOP_SIZE;

        outbuf[lch]=lv;
        outbuf[rch]=lv;

        // below looks odd
        z.aevolve();
        z.kevolve();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    cout<<key<<endl;
    if(state==0)
    {
        if(key==57344)
        {
            // voice F1 -> v0
            state=1;
        }
    }
    else if(state==1)
    {
        if(key>=97 && key<=122)
        {
            z.v0=key-97;
            state=2;
        }
    }
    else if(state==2)
    {
        if(key>=97 && key<=122)
        {
            z.getosc(z.v0).rtlo=key-97;
            state=3;
        }
    }
    else if(state==3)
    {
        if(key>=97 && key<=122)
        {
            z.getosc(z.v0).atlo=key-97;
            state=0;
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
