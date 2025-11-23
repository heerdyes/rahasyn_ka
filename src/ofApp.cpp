#include "ofApp.h"

void ofApp::initfsm()
{
    s0.setup(290,HH-88-44, 0);
    s1.setup(180,HH-88-44, 1);
    s2.setup(480,HH-156, 2);
    s3.setup(420,HH-264, 3);
    s4.setup(420,HH-44, 4);
    s5.setup(640,HH-280, 5);
    s6.setup(660,HH-88-88, 6);
    s7.setup(640,HH-44, 7);
    s8.setup(200,HH-281, 8);
    s9.setup(120,HH-44, 9);
    s10.setup(64,HH-99, 10);
    s11.setup(81,HH-191, 11);
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetWindowTitle("rahasyn_ka");
    ofBackground(0);

    scopectr=0;
    ctr=0;
    state=0;

    initfsm();

    z.setup();
    L.setup(1380,HH-303);
    
    soundsetup();
    
    if(camon)
    {
        // try to grab at this size
	    camw = 320;
	    camh = 240;
	    
	    vg.setVerbose(true);
	    vg.setup(camw,camh);
	    
	    vfx.allocate(camw, camh, OF_PIXELS_RGB);
	    vtx.allocate(vfx);
	}
	ofSetVerticalSync(true);
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
    if(camon && ctr%12==0) camclk++;
    
    if(camon && isdvdss)
    {
        vg.update();
        
        camx+=vx;
        camy+=vy;
        if(camx<0 || camx>WW-camw*vscale) vx=-vx;
        if(camy<0 || camy>HH-camh*vscale) vy=-vy;
        
        if(vg.isFrameNew())
        {
		    ofPixels & pixels = vg.getPixels();
		    for(size_t i = 0; i < pixels.size(); i++)
		    {
			    // pixel funk
			    int b=pixels[i]&0xff;
			    int g=(pixels[i]>>8)&0xff;
			    int r=(pixels[i]>>16)&0xff;
			    
			    int fr=r;
			    int fg=g;
			    int fb=b+camclk;
			    
			    vfx[i]=((fr&0xff)<<16)+((fg&0xff)<<8)+(fb&0xff);
		    }
		    
		    //load the fxd pixels
		    vtx.loadData(vfx);
	    }
	}
}

void ofApp::rndrfsm()
{
    // edges first
    u.edge2(s0,s1, s0.x-30,s1.y-40, "F[1-4]");
    u.edge2(s1,s0, s0.x-30,s1.y+30, "a-z");
    u.edge2(s0,s2, s2.x-30,s0.y-20, "a-z");
    u.edge2(s2,s3, s2.x,s3.y, "r");
    u.edge2(s2,s4, s2.x,s4.y, "a");
    u.edge2(s3,s0, s0.x,s3.y, "a-z");
    u.edge2(s4,s0, s0.x,s4.y, "a-z");
    u.edge2(s3,s5, s5.x,s3.y, "0-9");
    u.edge2(s3,s6, s5.x,s5.y, "<.>");
    u.edge3(s5,s5, s5.x+60,s5.y-30, s5.x+60,s5.y+30, "0-9");
    u.edge2(s5,s6, s5.x+20,s6.y, "<.>");
    u.edge2(s6,s7, s7.x+30,s7.y-20, "0-9");
    u.edge3(s7,s7, s7.x+50,s7.y-30, s7.x+50,s7.y+30, "0-9");
    u.edge2(s4,s5, s6.x,s6.y, "0-9");
    u.edge2(s4,s6, s7.x,s7.y, "<.>");
    u.edge2(s7,s0, s2.x,s2.y+30, "\\n");
    u.edge2(s2,s8, s3.x,s3.y+30, "t");
    u.edge2(s8,s0, s1.x,s1.y-30, "A-Z");
    u.edge2(s0,s9, s0.x-30,s9.y+30, "F12");
    u.edge2(s9,s10, s9.x,s10.y, "<.>");
    u.edge2(s10,s11, s10.x-44,s11.y+33, "0-9");
    u.edge3(s11,s11, s11.x-20,s11.y-20,s11.x+20,s11.y-20, "0-9");
    u.edge2(s11,s0, s11.x+20,s0.y, "\\n");

    // then nodes, to prevent edge lines reaching the center
    s0.rndr(state);
    s1.rndr(state);
    s2.rndr(state);
    s3.rndr(state);
    s4.rndr(state);
    s5.rndr(state);
    s6.rndr(state);
    s7.rndr(state);
    s8.rndr(state);
    s9.rndr(state);
    s10.rndr(state);
    s11.rndr(state);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // video plsss
    ofSetColor(255);
	if(camon && isdvdss) vtx.draw(camx,camy, camw*vscale,camh*vscale);
	
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

    z.rndrtlos(WW/2+550, HH/2-170, 330);
    z.rndrtbls(WW/2-550, HH/2-170, 330);

    // render finite state machine
    rndrfsm();
    // transcript
    L.rndr(18);
    // stack and numtok
    S.rndr(WW/2,HH/2+200, 18);
    ofDrawBitmapString(numtok, WW/2-numtok.length()*SNHALF,HH/2+200+30);
    // vox
    z.rndrvox(WW/2,HH/2+200+108);
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

        // a-rate and k-rate are unified?
        z.evolve();
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
    
    if(key==96)
    {
        isdvdss=!isdvdss;
        L.log("// screensaver cam: "+ofToString(isdvdss));
    }
    
    if(ckey==' ')
    {
        // panic and zero out gains of each vox
        z.panic();
        L.log("!! all vox tlo gains set to zero !!");
        return;
    }
    
    if(key==57362) // home
    {
        z.resetptrs();
        L.log("!! tlo ptrs syncd to 0 !!");
        return;
    }

    if(state==0)
    {
        // F[1-4] -> v[0-3]
        if(key==57344)
        {
            S.push(key);
            state=1;
        }
        else if(key==57345)
        {
            S.push(key);
            state=1;
        }
        else if(key==57346)
        {
            S.push(key);
            state=1;
        }
        else if(key==57347)
        {
            S.push(key);
            state=1;
        }
        // [a-z] -> oscil
        else if(key>=97 && key<=122)
        {
            S.push(key);
            state=2;
        }
        // F12 -> master volume
        else if(key==57355)
        {
            S.push(key);
            state=9;
        }
    }
    else if(state==1)
    {
        if(key>=97 && key<=122)
        {
            int fkey=S.pop();
            int vcur=fkey-57344;
            
            if(vcur==0)      z.v0=key-97;
            else if(vcur==1) z.v1=key-97;
            else if(vcur==2) z.v2=key-97;
            else if(vcur==3) z.v3=key-97;

            L.log("// v"+ofToString(vcur)+" -> "+ofToString(ckey));
            state=0;
        }
    }
    else if(state==2)
    {
        if(ckey=='r')
        {
            S.push(key);
            state=3;
        }
        else if(ckey=='a')
        {
            S.push(key);
            state=4;
        }
        else if(ckey=='t')
        {
            S.push(key);
            state=8;
        }
    }
    else if(state==3)
    {
        if(key>=97 && key<=122)
        {
            S.pop(); // discard top
            int oc=S.pop();

            int oci=oc-97;
            L.log("// "+ofToString((char)oc)+".rate <- "+ofToString(ckey));

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
            int oc=S.pop();

            int oci=oc-97;
            L.log("// "+ofToString((char)oc)+".rtlo = -1");

            z.ox[oci].rtlo=-1;
            state=0;
        }
    }
    else if(state==4)
    {
        if(key>=97 && key<=122)
        {
            S.pop(); // discard top
            int oc=S.pop();

            int oci=oc-97;
            L.log("// "+ofToString((char)oc)+".amp <- "+ofToString(ckey));

            z.ox[oci].atlo=key-97;
            state=0;
        }
        else if(key==45) // -
        {
            S.pop(); // discard top
            int oc=S.pop();

            int oci=oc-97;
            L.log("// "+ofToString((char)oc)+".atlo = -1");

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
            char attr=(char)S.pop();
            int oci=S.pop();
            int oix=oci-97;

            if(attr=='r')
            {
                z.ox[oix].setr(ofToFloat(numtok));
                L.log("// "+ofToString((char)oci)+".rate = "+numtok);
            }
            else if(attr=='a')
            {
                z.ox[oix].seta(ofToFloat(numtok));
                L.log("// "+ofToString((char)oci)+".amp = "+numtok);
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
            int oc=S.pop();
            int oci=oc-97;

            L.log("// "+ofToString((char)oc)+".table -> "+ofToString(ckey));
            z.ox[oci].tid=key-65;

            state=0;
        }
    }
    else if(state==9)
    {
        if(key==46) // .
        {
            numtok+=".";
            state=10;
        }
    }
    else if(state==10)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=11;
        }
    }
    else if(state==11)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=11;
        }
        else if(key==13)
        {
            z.mgain=ofToFloat(numtok);
            L.log("!! master volume = "+ofToString(z.mgain));
            
            S.pop();
            numtok.clear();
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

