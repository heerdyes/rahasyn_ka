#include "ofApp.h"

void ofApp::initfsm()
{
    s0.setup(400,HH-44,0);
    s1.setup(480,HH-88,1);
    s2.setup(550,HH-88,2);
    s20.setup(600,HH-144,20);
    s21.setup(680,HH-144,21);
    s22.setup(720,HH-88,22);
    s3.setup(600,HH-44,3);
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetWindowTitle("rahasyn_ka");
    ofBackground(0);

    scopectr=0;
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
    if(ctr%4==0)
    {
        u.update();
        z.u.update();
    }
}

void ofApp::rndrfsm()
{
    u.edge2(s0,s1, s0.x-30,s1.y-40, "F1", fej);
    u.edge2(s1,s2, s1.x-30,s2.y-20, "a-z", fej);
    u.edge2(s2,s3, s2.x+30,s3.y-20, "a-z", fej);
    u.edge2(s3,s0, (s3.x+s0.x)/2,s0.y+20, "a-z", fej);

    u.edge2(s2,s20, (s2.x+s20.x)/2-40,s2.y-60, "0-9", fej);
    u.edge3(s20,s20, s20.x-60,s20.y-60, s20.x+40,s20.y-60, "0-9", fej);
    u.edge2(s20,s21, s20.x+50,s21.y-40, ".", fej);
    u.edge3(s22,s22, s22.x+10,s22.y-50, s22.x+70,s22.y, "0-9", fej);
    u.edge2(s21,s22, s21.x+30,s22.y-30, "0-9", fej);
    u.edge2(s2,s21, s2.x+50,s21.y+50, ".", fej);
    u.edge2(s22,s3, s3.x+30,s3.y+40, "\\n", fej);

    s0.rndr(fej, state);
    s1.rndr(fej, state);
    s2.rndr(fej, state);
    s20.rndr(fej, state);
    s21.rndr(fej, state);
    s22.rndr(fej, state);
    s3.rndr(fej, state);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(22,202,232);
    fnt.drawString("rahasynth_ka", 22,44);

    float cy=HH/2;
    float cgy=cy-100;
    float wd=1280;
    float ht=440;
    float cx=WW/2-wd/2;
    float ky=ht/2;
    float kx=wd/SCOP_SIZE;

    // sound waveform
    ofSetColor(23,202,232);
    for(int i=0;i<SCOP_SIZE;i++)
    {
        float xx=cx+i*kx;
        ofDrawLine(xx,cgy,xx,cgy-scope[i]*ky);
    }

    z.rndrtlos(WW/2+400, HH/2-100, 330, fej);
    z.rndrtbls(WW/2-400, HH/2-100, 330, fej);

    // render finite state machine
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
    char ckey=(char)key;

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
        else if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=20;
        }
        else if(ckey=='.')
        {
            numtok+=ofToString(ckey);
            state=21;
        }
    }
    else if(state==20)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=20;
        }
        else if(ckey=='.')
        {
            numtok+=ofToString(ckey);
            state=21;
        }
    }
    else if(state==21)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=22;
        }
    }
    else if(state==22)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=22;
        }
        else if(key==13) // enter key
        {
            tlo & _o=z.getosc(z.v0);
            _o.rate=ofToFloat(numtok);
            _o.rtlo=-1;

            numtok.clear();
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
