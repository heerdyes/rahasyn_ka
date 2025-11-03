#include "ofApp.h"

void ofApp::initfsm()
{
    s0.setup(290,HH-88-44, 0);
    s1.setup(120,HH-88-44, 1);
    s2.setup(480,HH-156, 2);
    s3.setup(420,HH-264, 3);
    s4.setup(420,HH-44, 4);
    s5.setup(640,HH-280, 5);
    s6.setup(660,HH-88-88, 6);
    s7.setup(640,HH-44, 7);
    s8.setup(200,HH-281, 8);
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetWindowTitle("rahasyn_ka");
    ofBackground(0);

    scopectr=0;
    ctr=0;
    state=0;

    string sf("OcrA.ttf");
    fnt.load(sf, 16);
    fej.load(sf, 11);

    initfsm();

    z.setup();
    L.setup(1380,HH-303);
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
    // edges first
    u.edge2(s0,s1, s0.x-30,s1.y-40, "F[1-4]", fej);
    u.edge2(s1,s0, s0.x-30,s1.y+30, "a-z", fej);
    u.edge2(s0,s2, s2.x-30,s0.y-20, "a-z", fej);
    u.edge2(s2,s3, s2.x,s3.y, "r", fej);
    u.edge2(s2,s4, s2.x,s4.y, "a", fej);
    u.edge2(s3,s0, s0.x,s3.y, "a-z", fej);
    u.edge2(s4,s0, s0.x,s4.y, "a-z", fej);
    u.edge2(s3,s5, s5.x,s3.y, "0-9", fej);
    u.edge2(s3,s6, s5.x,s5.y, "<.>", fej);
    u.edge3(s5,s5, s5.x+60,s5.y-30, s5.x+60,s5.y+30, "0-9", fej);
    u.edge2(s5,s6, s5.x+20,s6.y, "<.>", fej);
    u.edge2(s6,s7, s7.x+30,s7.y-20, "0-9", fej);
    u.edge3(s7,s7, s7.x+50,s7.y-30, s7.x+50,s7.y+30, "0-9", fej);
    u.edge2(s4,s5, s6.x,s6.y, "0-9", fej);
    u.edge2(s4,s6, s7.x,s7.y, "<.>", fej);
    u.edge2(s7,s0, s2.x,s2.y+30, "\\n", fej);
    u.edge2(s2,s8, s3.x,s3.y+30, "t", fej);
    u.edge2(s8,s0, s1.x,s1.y-30, "A-Z", fej);

    // then nodes, to prevent edge lines reaching the center
    s0.rndr(fej, state);
    s1.rndr(fej, state);
    s2.rndr(fej, state);
    s3.rndr(fej, state);
    s4.rndr(fej, state);
    s5.rndr(fej, state);
    s6.rndr(fej, state);
    s7.rndr(fej, state);
    s8.rndr(fej, state);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(22,202,232);

    float cy=HH/2;
    float cgy=cy-100;
    float wd=1280;
    float ht=440;
    float cx=WW/2-wd/2;
    float ky=ht/2;
    float kx=wd/SCOP_SIZE;

    // sound waveform
    ofSetColor(255,202,77);
    for(int i=0;i<SCOP_SIZE;i++)
    {
        float xx=cx+i*kx;
        ofDrawLine(xx,cgy,xx,cgy-scope[i]*ky);
    }

    z.rndrtlos(WW/2+550, HH/2-170, 330, fej);
    z.rndrtbls(WW/2-550, HH/2-170, 330, fej);

    // render finite state machine
    rndrfsm();
    // transcript
    L.rndr(fnt, 18);
    // stack and numtok
    S.rndr(WW/2,HH/2+200, fnt,18);
    fnt.drawString(numtok, WW/2-fnt.stringWidth(numtok)/2,HH/2+200+30);
    // vox
    z.rndrvox(WW/2,HH/2+200+108, fnt);
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

    if(key==3682)
    {
        z.updatesplt(mouseX, mouseY);
    }
    if(key==3683)
    {
        z.updatesplo(mouseX, mouseY);
    }

    if(state==0)
    {
        // F[1-4] -> v[0-3]
        if(key==57344)
        {
            vcur=0;
            state=1;
        }
        else if(key==57345)
        {
            vcur=1;
            state=1;
        }
        else if(key==57346)
        {
            vcur=2;
            state=1;
        }
        else if(key==57347)
        {
            vcur=3;
            state=1;
        }
        // [a-z] -> oscil
        else if(key>=97 && key<=122)
        {
            S.push(ckey);
            state=2;
        }
    }
    else if(state==1)
    {
        if(key>=97 && key<=122)
        {
            if(vcur==0)      z.v0=key-97;
            else if(vcur==1) z.v1=key-97;
            else if(vcur==2) z.v2=key-97;
            else if(vcur==3) z.v3=key-97;

            L.log("[vox] v"+ofToString(vcur)+" -> "+ofToString(ckey));
            state=0;
        }
    }
    else if(state==2)
    {
        if(ckey=='r')
        {
            S.push(ckey);
            state=3;
        }
        else if(ckey=='a')
        {
            S.push(ckey);
            state=4;
        }
        else if(ckey=='t')
        {
            S.push(ckey);
            state=8;
        }
    }
    else if(state==3)
    {
        if(key>=97 && key<=122)
        {
            S.pop(); // discard top
            char oc=S.pop();

            int oci=((int)oc)-97;
            L.log("[m] "+ofToString(oc)+".rate <- "+ofToString(ckey));

            z.ox[oci].rtlo=key-97;
            state=0;
        }
        else if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=5;
        }
        else if(key==46) // .
        {
            numtok+=".";
            state=6;
        }
        else if(key==45) // -
        {
            S.pop(); // discard top
            char oc=S.pop();

            int oci=((int)oc)-97;
            L.log("[m] "+ofToString(oc)+".rtlo = -1");

            z.ox[oci].rtlo=-1;
            state=0;
        }
    }
    else if(state==4)
    {
        if(key>=97 && key<=122)
        {
            S.pop(); // discard top
            char oc=S.pop();

            int oci=((int)oc)-97;
            L.log("[m] "+ofToString(oc)+".amp <- "+ofToString(ckey));

            z.ox[oci].atlo=key-97;
            state=0;
        }
        else if(key==45) // -
        {
            S.pop(); // discard top
            char oc=S.pop();

            int oci=((int)oc)-97;
            L.log("[m] "+ofToString(oc)+".atlo = -1");

            z.ox[oci].atlo=-1;
            state=0;
        }
        else if(key==46) // .
        {
            numtok+=".";
            state=6;
        }
        else if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=5;
        }
    }
    else if(state==5)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=5;
        }
        else if(key==46) // .
        {
            numtok+=".";
            state=6;
        }
    }
    else if(state==6)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=7;
        }
    }
    else if(state==7)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=7;
        }
        else if(key==13)
        {
            char attr=S.pop();
            char oci=S.pop();
            int oix=((int)oci)-97;

            if(attr=='r')
            {
                z.ox[oix].setr(ofToFloat(numtok));
                L.log("[r] "+ofToString(oci)+".rate = "+numtok);
            }
            else if(attr=='a')
            {
                z.ox[oix].seta(ofToFloat(numtok));
                L.log("[a] "+ofToString(oci)+".amp = "+numtok);
            }

            numtok.clear();
            state=0;
        }
    }
    else if(state==8)
    {
        if(key>=65 && key<=90)
        {
            S.pop(); // discard 't'
            char oc=S.pop();
            int oci=((int)oc)-97;

            L.log("[t] "+ofToString(oc)+".table -> "+ofToString(ckey));
            z.ox[oci].tid=key-65;

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
