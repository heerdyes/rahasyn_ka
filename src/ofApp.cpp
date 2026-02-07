#include "ofApp.h"

void ofApp::initfsm()
{
    s0.setup(570,HH-174, 0);
    s1.setup(421,HH-65, 1);
    s2.setup(760,HH-236, 2);
    s3.setup(750,HH-314, 3);
    s4.setup(780,HH-124, 4);
    s5.setup(860,HH-280, 5);
    s6.setup(900,HH-170, 6);
    s7.setup(822,HH-184, 7);
    s8.setup(710,HH-277, 8);
    s9.setup(460,HH-44, 9);
    s10.setup(302,HH-44, 10);
    s11.setup(382,HH-74, 11);
    //
    s12.setup(300,HH-220, 12);
    s13.setup(172,HH-205, 13);
    s14.setup(115,HH-250, 14);
    s15.setup(38,HH-276, 15);
    s16.setup(32,HH-340, 16);
    s17.setup(99,HH-380, 17);
    s18.setup(180,HH-350, 18);
    s19.setup(200,HH-266, 19);
    s20.setup(120,HH-316, 20);
    s21.setup(68,HH-220, 21);
    //
    s22.setup(200,HH-138, 22);
    s23.setup(120,HH-138, 23);
    s24.setup(60,HH-118, 24);
    s25.setup(100,HH-48, 25);
    s26.setup(260,HH-300, 26);
    s27.setup(340,HH-280, 27);
    //
    s28.setup(550,HH-323, 28);
    s30.setup(560,HH-44, 30);
    s29.setup(840,HH-323, 29);
    s32.setup(760,HH-383, 32);
    s33.setup(700,HH-343, 33);
    s34.setup(650,HH-53, 34);
    s35.setup(740,HH-60, 35);
    s37.setup(700,HH-103, 37);
    //
    s31.setup(390,HH-250, 31);
}

void ofApp::exit()
{
    pko.quit();
    
    cout<<endl<<"[session.rhka]"<<endl;
    cout<<cmdlog<<endl<<endl;
    
    if(persist_cmdlog)
    {
        ofBuffer snbuf(cmdlog.c_str(), cmdlog.size());
        ofBufferToFile("session.rhka", snbuf);
    }
}

void ofApp::loadconf()
{
    conf.load("settings.xml");
    mpdmidf=conf.getValue("raha:midi","");
    ofLog()<<"#conf# setting mpdmidf: "<<mpdmidf;
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
    z.setup(&mpdq);
    L.setup(1380,HH-303);
    pko.setup();
    soundsetup();
    //
    loadconf();
    mpd.setup(mpdmidf, &mpdq);
    
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
	
	L.log("  / --------- --------- \\");
	L.log("||  greetings soundseer  ||");
	L.log("  \\ --------- --------- /");
	L.log("");
	L.log("[mpd] "+mpd.state);
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
	
	// listen to h/w i/f if connected
	if(pko.state!='e')
	{
	    pko.listen(L);
	    // map the master gain
    	z.mgain=ofMap(pko.kn0, 0,4096, .0,.999);
    }
    
    // wave pencil
    if(state==31)
    {
        if(S.sp==2)
        {
            int txia=S.buf[0];
            int txi=txia-65;
            
            int tndx=(int)ofMap(mouseX, 0,WW, 0,z.tx[txi].sz);
            float vv=ofMap(mouseY, 0,HH, 1,-1);
            
            z.tx[txi].sampwr(tndx, vv);
        }
    }
}

void ofApp::rndrfsm()
{
    // edges first
    u.edge2(s0,s1, (s0.x+s1.x)/2,s0.y+22, "F[1-4]");
    u.edge2(s1,s0, (s0.x+s1.x)/2,s1.y-11, "-|[a-z]");
    u.edge2(s0,s2, (s0.x+s2.x)/2+20,(s0.y+s2.y)/2-20, "[a-z]");
    u.edge2(s2,s0, (s0.x+s2.x)/2,(s0.y+s2.y)/2+36, ".|\\n");
    u.edge2(s2,s3, (s2.x+s3.x)/2+22,(s2.y+s3.y)/2+11, "r");
    u.edge2(s2,s4, s2.x,s4.y, "a");
    u.edge2(s3,s0, (s0.x+s3.x)/2,(s0.y+s3.y)/2-22, "-|[a-z]");
    u.edge2(s4,s0, (s0.x+s4.x)/2+33,(s0.y+s4.y)/2, "-|[a-z]");
    u.edge2(s3,s5, (s3.x+s5.x)/2,(s3.y+s5.y)/2+11, "[0-9]");
    u.edge2(s3,s6, (s3.x+s6.x)/2+27,(s3.y+s6.y)/2-7, "<.>");
    u.edge3(s5,s5, s5.x+60,s5.y-30, s5.x+60,s5.y+30, "[0-9]");
    u.edge2(s5,s6, (s5.x+s6.x)/2+22,(s5.y+s6.y)/2-22, "<.>");
    u.edge2(s6,s7, s7.x+30,s7.y-20, "[0-9]");
    u.edge3(s7,s7, s7.x+80,s7.y, s7.x,s7.y+80, "[0-9]");
    u.edge2(s4,s5, (s4.x+s5.x)/2-22,(s4.y+s5.y)/2-22, "[0-9]");
    u.edge2(s4,s6, (s4.x+s6.x)/2+22,(s4.y+s6.y)/2+22, "<.>");
    u.edge2(s7,s0, (s7.x+s0.x)/2+44,(s7.y+s0.y)/2+4, "\\n");
    u.edge2(s2,s8, s2.x-8,s8.y, "t");
    u.edge2(s8,s0, (s0.x+s8.x)/2+22,(s0.y+s8.y)/2, "-|[A-Z]");
    u.edge2(s0,s9, (s0.x+s9.x)/2-33,(s0.y+s9.y)/2+55, "F12");
    u.edge2(s9,s10, (s9.x+s10.x)/2,s10.y+22, "<.>");
    u.edge2(s10,s11, (s10.x+s11.x)/2-22,s11.y+33, "[0-9]");
    u.edge3(s11,s11, s11.x-40,s11.y-40,s11.x+40,s11.y-40, "[0-9]");
    u.edge2(s11,s0, (s11.x+s0.x)/2,s0.y, "\\n");
    u.edge2(s0,s12, s12.x,(s0.y+s12.y)/2, "[A-Z]");
    u.edge2(s12,s0, s0.x-44,(s0.y+s12.y)/2-20, "[wzjub]");
    u.edge2(s12,s13, (s12.x+s13.x)/2,s12.y+22, "d");
    u.edge2(s12,s26, s12.x-18,s12.y-10, "r");
    u.edge2(s13,s14, (s13.x+s14.x)/2+8,s14.y+22, "<.>");
    u.edge2(s13,s21, (s13.x+s21.x)/2,s21.y+28, "<->");
    u.edge2(s21,s14, (s14.x+s21.x)/2,s21.y, "<.>");
    u.edge2(s14,s15, (s14.x+s15.x)/2,s14.y, "[0-9]");
    u.edge3(s15,s15, s15.x-55,s15.y+55, s15.x+11,s15.y+55, "[0-9]");
    u.edge2(s15,s16, s15.x-20,s15.y, "<,>");
    u.edge2(s15,s0, s15.x-138,s0.y+0, "\\n");
    u.edge2(s16,s17, (s16.x+s17.x)/2,s17.y, "<.>");
    u.edge2(s17,s18, (s17.x+s18.x)/2,s18.y, "[0-9]");
    u.edge3(s18,s18, s18.x-22,s18.y-44, s18.x+36,s18.y-44, "[0-9]");
    u.edge2(s18,s0, (s18.x+s0.x)/2+128,(s0.y+s18.y)/2-128, "\\n");
    u.edge2(s12,s19, (s12.x+s19.x)/2,s12.y-20, "n");
    u.edge2(s19,s20, (s19.x+s20.x)/2,s19.y, "[1-9]");
    u.edge3(s20,s20, s20.x-55,s20.y+27, s20.x-55,s20.y-27, "[0-9]");
    u.edge2(s20,s0, (s20.x+s0.x)/2+128,(s20.y+s0.y)/2-128, "\\n");
    u.edge2(s0,s22, (s0.x+s22.x)/2,s22.y, "<[>");
    u.edge2(s22,s23, (s22.x+s23.x)/2,s22.y, "[A-Z]");
    u.edge2(s23,s24, (s23.x+s24.x)/2-20,s24.y-20, "[A-Z]");
    u.edge2(s23,s0, s23.x-33,s23.y+132, "<->");
    u.edge2(s24,s25, (s24.x+s25.x)/2-30,s25.y, "[A-Z]");
    u.edge2(s25,s0, (s25.x+s0.x)/2-150,(s25.y+s0.y)/2, "[a-z]");
    u.edge2(s26,s27, (s26.x+s27.x)/2,s27.y+10, "[1-9]");
    u.edge3(s27,s27, s27.x-50,s27.y-36, s27.x+50,s27.y-36, "[0-9]");
    u.edge2(s27,s0, (s27.x+s0.x)/2+22,(s27.y+s0.y)/2-55, "\\n");
    u.edge2(s0,s28, (s0.x+s28.x)/2-44,(s0.y+s28.y)/2+33, "</>");
    u.edge2(s28,s0, (s28.x+s0.x)/2+44,(s28.y+s0.y)/2-33, "[0-9a-z]");
    u.edge2(s0,s30, (s0.x+s30.x)/2-44,(s0.y+s30.y)/2+33, "<\\>");
    u.edge2(s30,s0, (s0.x+s30.x)/2+44,(s0.y+s30.y)/2-33, "[a-z]");
    u.edge2(s3,s29, (s3.x+s29.x)/2,(s3.y+s29.y)/2-11, "<=>");
    u.edge2(s29,s32, (s29.x+s32.x)/2+22,(s29.y+s32.y)/2-22, "<.>");
    u.edge2(s29,s0, (s29.x+s0.x)/2+55,(s29.y+s0.y)/2-178, "1");
    u.edge2(s32,s33, (s32.x+s33.x)/2-22,(s32.y+s33.y)/2+11, "[0-9]");
    u.edge3(s33,s33, s33.x-44,s33.y+44, s33.x-44,s33.y-44, "[0-9]");
    u.edge2(s33,s0, (s33.x+s0.x)/2-22,(s33.y+s0.y)/2-11, "\\n");
    u.edge2(s4,s37, (s4.x+s37.x)/2+22,(s4.y+s37.y)/2, "<=>");
    u.edge2(s37,s35, (s35.x+s37.x)/2,(s35.y+s37.y)/2+22, "<.>");
    u.edge2(s37,s0, (s0.x+s37.x)/2+22,(s0.y+s37.y)/2, "1");
    u.edge2(s35,s34, (s35.x+s34.x)/2,(s35.y+s34.y)/2+22, "[0-9]");
    u.edge3(s34,s34, s34.x-55,s34.y+55, s34.x-55,s34.y-33, "[0-9]");
    u.edge2(s34,s0, (s34.x+s0.x)/2+33,(s34.y+s0.y)/2-33, "\\n");
    u.edge2(s12,s31, (s12.x+s31.x)/2,(s12.y+s31.y)/2-22, "m");
    u.edge2(s31,s0, (s0.x+s31.x)/2+22,(s0.y+s31.y)/2-22, "\\n");

    // then nodes, to prevent edge lines reaching the center
    s0.rndr(state); s1.rndr(state); s2.rndr(state); s3.rndr(state);
    s4.rndr(state); s5.rndr(state); s6.rndr(state); s7.rndr(state);
    s8.rndr(state); s9.rndr(state); s10.rndr(state); s11.rndr(state);
    s12.rndr(state); s13.rndr(state); s14.rndr(state); s15.rndr(state);
    s16.rndr(state); s17.rndr(state); s18.rndr(state); s19.rndr(state);
    s20.rndr(state); s21.rndr(state); s22.rndr(state); s23.rndr(state);
    s24.rndr(state); s25.rndr(state); s26.rndr(state); s27.rndr(state);
    s28.rndr(state); s29.rndr(state); s30.rndr(state); s32.rndr(state);
    s33.rndr(state); s34.rndr(state); s35.rndr(state); s37.rndr(state);
    s31.rndr(state);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // video plsss
    WWHITE;
	if(camon && isdvdss) vtx.draw(camx,camy, camw*vscale,camh*vscale);
	
    TRBLU;
    float cy=HH/2;
    float cgy=cy-132;
    float wd=1600;
    float ht=550;
    float cx=WW/2-wd/2;
    float ky=ht/2;
    float kx=wd/SCOP_SIZE;

    // sound waveform
    SCOPEN;
    for(int i=0;i<SCOP_SIZE;i++)
    {
        float xx=cx+i*kx;
        ofDrawLine(xx,cgy,xx,cgy-scope[i]*ky);
    }
    
    // rndr q
    mpdq.rndr(212,11);

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
    
    // pico chat
    string pkomsg=ofToString(pko.state)+"::"+ofToString(pko.kn0,0,4,'0');
    ofDrawBitmapString(pkomsg, WW/2-SNHALF*pkomsg.length(),32);
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

void ofApp::sourcerhka(string kafn)
{
    ofBuffer cmdsq=ofBufferFromFile(kafn);
    string scmd=cmdsq.getText();
    
    for(size_t i=0;i<scmd.length();i++)
    {
        int ki=0;
        char tc=scmd[i];
        
        if(tc=='\n') ki=13;
        else if(tc=='<')
        {
            string fntok;
            for(;;)
            {
                i++;
                if(i>=scmd.length()) break;
                if(scmd[i]=='>') break;
                fntok+=scmd[i];
            }
            
            if(fntok=="F1") ki=57344;
            else if(fntok=="F2") ki=57345;
            else if(fntok=="F3") ki=57346;
            else if(fntok=="F4") ki=57347;
            else if(fntok=="F12") ki=57355;
        }
        else ki=(int)scmd[i];
        
        //cout<<"[autokey] "<<ki<<endl;
        kpevt(ki);
    }
}

void ofApp::kpevt(int key)
{
    char ckey=(char)key;
    cout<<key<<endl;
    
    if(key==3682)
    {
        z.updatesplt(mouseX, mouseY);
    }
    if(key==3683)
    {
        z.updatesplo(mouseX, mouseY);
    }
    if(key==3684)
    {
        z.updatesplw(mouseX, mouseY);
    }
    
    if(key==96)
    {
        cmdlog+=ofToString(ckey);
        isdvdss=!isdvdss;
        L.log("// screensaver cam: "+ofToString(isdvdss));
    }
    
    if(ckey==' ')
    {
        cmdlog+=ofToString(ckey);
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
            cmdlog+="<F1>";
        }
        else if(key==57345)
        {
            S.push(key);
            state=1;
            cmdlog+="<F2>";
        }
        else if(key==57346)
        {
            S.push(key);
            state=1;
            cmdlog+="<F3>";
        }
        else if(key==57347)
        {
            S.push(key);
            state=1;
            cmdlog+="<F4>";
        }
        // [a-z] -> oscil
        else if(key>=97 && key<=122)
        {
            S.push(key);
            state=2;
            cmdlog+=ofToString(ckey);
        }
        // F12 -> master volume
        else if(key==57355)
        {
            S.push(key);
            state=9;
            cmdlog+="<F12>";
        }
        // [A-Z] table
        else if(key>=65 && key<=90)
        {
            S.push(key);
            state=12;
            cmdlog+=ofToString(ckey);
        }
        else if(key==91) // [
        {
            S.push(key);
            state=22;
            cmdlog+=ofToString(ckey);
        }
        else if(key==47) // /
        {
            S.push(key);
            state=28;
            cmdlog+=ofToString(ckey);
        }
        else if(key==92) /* \\ */
        {
            S.push(key);
            state=30;
            cmdlog+=ofToString(ckey);
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
            cmdlog+=ofToString(ckey);
        }
        else if(key==45)
        {
            int fkey=S.pop();
            int vcur=fkey-57344;
            
            if(vcur==0)      z.v0=-1;
            else if(vcur==1) z.v1=-1;
            else if(vcur==2) z.v2=-1;
            else if(vcur==3) z.v3=-1;
            
            L.log("// v"+ofToString(vcur)+" -> -1");
            state=0;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==2)
    {
        if(ckey=='r')
        {
            S.push(key);
            state=3;
            cmdlog+=ofToString(ckey);
        }
        else if(ckey=='a')
        {
            S.push(key);
            state=4;
            cmdlog+=ofToString(ckey);
        }
        else if(ckey=='t')
        {
            S.push(key);
            state=8;
            cmdlog+=ofToString(ckey);
        }
        else if(ckey=='.')
        {
            int oc=S.pop();
            int oci=oc-97;
            
            z.ox[oci].trigo=!z.ox[oci].trigo;
            L.log("// "+ofToString((char)oc)+".trigo = "+ofToString(z.ox[oci].trigo));

            state=0;
            cmdlog+=ofToString(ckey);
        }
        else if(key==13)
        {
            int oc=S.pop();
            int oci=oc-97;
            L.log("// "+ofToString((char)oc)+".trig() !");

            z.ox[oci].trigger();
            state=0;
            cmdlog+=ofToString(ckey);
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
            cmdlog+=ofToString(ckey);
        }
        else if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=5;
            cmdlog+=ofToString(ckey);
        }
        else if(key==46) // .
        {
            numtok+=".";
            state=6;
            cmdlog+=ofToString(ckey);
        }
        else if(key==45) // -
        {
            S.pop(); // discard top
            int oc=S.pop();

            int oci=oc-97;
            L.log("// "+ofToString((char)oc)+".rtlo = -1");

            z.ox[oci].rtlo=-1;
            state=0;
            cmdlog+=ofToString(ckey);
        }
        else if(key==61) // =
        {
            S.push(key);
            state=29;
            cmdlog+=ofToString(ckey);
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
            cmdlog+=ofToString(ckey);
        }
        else if(key==45) // -
        {
            S.pop(); // discard top
            int oc=S.pop();

            int oci=oc-97;
            L.log("// "+ofToString((char)oc)+".atlo = -1");

            z.ox[oci].atlo=-1;
            state=0;
            cmdlog+=ofToString(ckey);
        }
        else if(key==46) // .
        {
            numtok+=".";
            state=6;
            cmdlog+=ofToString(ckey);
        }
        else if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=5;
            cmdlog+=ofToString(ckey);
        }
        else if(key==61) // =
        {
            S.push(key);
            state=37;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==5)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=5;
            cmdlog+=ofToString(ckey);
        }
        else if(key==46) // .
        {
            numtok+=".";
            state=6;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==6)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=7;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==7)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=7;
            cmdlog+=ofToString(ckey);
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
            cmdlog+="\n";
        }
    }
    else if(state==8)
    {
        if(key>=65 && key<=90)
        {
            S.pop(); // discard 't'
            int oc=S.pop();
            int oci=oc-97;

            z.ox[oci].tid=key-65;
            L.log("[ox] "+ofToString((char)oc)+".table -> "+ofToString(ckey));
            state=0;
            cmdlog+=ofToString(ckey);
        }
        else if(key==45) // -
        {
            S.pop(); // discard 't'
            int oc=S.pop();
            int oci=oc-97;

            z.ox[oci].tid=-1;
            L.log("[ox] "+ofToString((char)oc)+".table -> null");
            state=0;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==9)
    {
        if(key==46) // .
        {
            numtok+=".";
            state=10;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==10)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=11;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==11)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=11;
            cmdlog+=ofToString(ckey);
        }
        else if(key==13)
        {
            z.mgain=ofToFloat(numtok);
            L.log("!! master volume = "+ofToString(z.mgain));
            
            S.pop();
            numtok.clear();
            state=0;
            cmdlog+="\n";
        }
    }
    else if(state==12)
    {
        int tkey,tix;
        char ctx;
        if(key==119) // w
        {
            tkey=S.pop();
            tix=tkey-65;
            ctx=(char)tkey;
            z.tx[tix].btri();
            L.log("// tbl "+ofToString(ctx)+" is triangled");
            state=0;
            cmdlog+=ofToString(ckey);
        }
        else if(key==122) // z
        {
            tkey=S.pop();
            tix=tkey-65;
            ctx=(char)tkey;
            z.tx[tix].dramp();
            L.log("// tbl "+ofToString(ctx)+" is ramped");
            state=0;
            cmdlog+=ofToString(ckey);
        }
        else if(key==106) // j
        {
            tkey=S.pop();
            tix=tkey-65;
            ctx=(char)tkey;
            z.tx[tix].jya();
            L.log("// tbl "+ofToString(ctx)+" is sined");
            state=0;
            cmdlog+=ofToString(ckey);
        }
        else if(key==117) // u
        {
            tkey=S.pop();
            tix=tkey-65;
            ctx=(char)tkey;
            z.tx[tix].bi2uni();
            L.log("[tx] "+ofToString(ctx)+" is unipolarized");
            state=0;
            cmdlog+=ofToString(ckey);
        }
        else if(key==98) // b
        {
            tkey=S.pop();
            tix=tkey-65;
            ctx=(char)tkey;
            z.tx[tix].uni2bi();
            L.log("[tx] "+ofToString(ctx)+" is bipolarized");
            state=0;
            cmdlog+=ofToString(ckey);
        }
        else if(key==100) // d
        {
            S.push(key);
            state=13;
            cmdlog+=ofToString(ckey);
        }
        else if(key==110) // n
        {
            S.push(key);
            state=19;
            cmdlog+=ofToString(ckey);
        }
        else if(key==114) // r
        {
            S.push(key);
            state=26;
            cmdlog+=ofToString(ckey);
        }
        else if(key==109) // m
        {
            S.push(key);
            state=31;
            cmdlog+="m";
            L.log("# wave mouse stupidly to change waveform");
        }
    }
    else if(state==13)
    {
        if(key==46)
        {
            numtok+=".";
            state=14;
            cmdlog+=ofToString(ckey);
        }
        else if(key==45) // -
        {
            numtok+="-";
            state=21;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==14)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=15;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==15)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=15;
            cmdlog+=ofToString(ckey);
        }
        else if(key==44)
        {
            arg1=ofToFloat(numtok);
            L.log("// decay = "+ofToString(arg1));
            numtok.clear();
            state=16;
            cmdlog+=ofToString(ckey);
        }
        else if(key==13)
        {
            arg1=ofToFloat(numtok);
            L.log("// decay = "+ofToString(arg1));
            numtok.clear();
            S.pop(); // chuck d
            
            int tkey=S.pop();
            char ct=(char)tkey;
            int ti=tkey-65;
            z.tx[ti].dcy(arg1);
            
            L.log("[tx] "+ofToString(ct)+" decayed");
            state=0;
            cmdlog+="\n";
        }
    }
    else if(state==16)
    {
        if(key==46)
        {
            numtok+=".";
            state=17;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==17)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=18;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==18)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=18;
            cmdlog+=ofToString(ckey);
        }
        else if(key==13)
        {
            arg2=ofToFloat(numtok);
            L.log("// phase = "+ofToString(arg2));
            numtok.clear();
            S.pop(); // chuck d
            
            int tkey=S.pop();
            char ct=(char)tkey;
            int ti=tkey-65;
            z.tx[ti].dcy(arg1, arg2);
            
            L.log("[tx] "+ofToString(ct)+" decayed");
            state=0;
            cmdlog+="\n";
        }
    }
    else if(state==19)
    {
        if(key>=49 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=20;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==20)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=20;
            cmdlog+=ofToString(ckey);
        }
        else if(key==13)
        {
            S.pop();
            int tkey=S.pop();
            char ct=(char)tkey;
            int ti=tkey-65;
            
            z.tx[ti].resize(ofToInt(numtok));
            L.log("[tx] "+ofToString(ct)+" resized to "+numtok);
            numtok.clear();
            state=0;
            cmdlog+="\n";
        }
    }
    else if(state==21)
    {
        if(key==46)
        {
            numtok+=".";
            state=14;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==22)
    {
        if(key>=65 && key<=90)
        {
            S.push(key);
            state=23;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==23)
    {
        if(key>=65 && key<=90)
        {
            S.push(key);
            state=24;
            cmdlog+=ofToString(ckey);
        }
        else if(key==45)
        {
            int td=S.pop();
            int tdid=td-65;
            S.pop();
            
            z.tcmd[tdid]=0;
            L.log("[wt] "+ofToString((char)td)+" wavetable deassoc");
            state=0;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==24)
    {
        if(key>=65 && key<=90)
        {
            S.push(key);
            state=25;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==25)
    {
        if(key>=97 && key<=122)
        {
            int t2=S.pop();
            int t1=S.pop();
            int td=S.pop();
            S.pop(); // discard [
            
            int t2id=t2-65;
            int t1id=t1-65;
            int tdid=td-65;
            int moid=key-97;
            
            z.tcmd[tdid]=u.tcmd_pack(8, t1id, t2id, moid);
            
            string std=ofToString((char)td);
            string skey=ofToString((char)key);
            string st1=ofToString((char)t1);
            string st2=ofToString((char)t2);
            L.log("(wt "+std+" "+skey+" "+st1+" "+st2+")");
            
            state=0;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==26)
    {
        if(key>=49 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=27;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==27)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ofToString(ckey);
            state=27;
            cmdlog+=ofToString(ckey);
        }
        else if(key==13)
        {
            S.pop(); // discard r
            int tk=S.pop();
            int tkid=tk-65;
            
            int nt=ofToInt(numtok);
            nt=nt%256;
            numtok.clear();
            
            z.tcmd[tkid]=u.tcmd_pack(2, nt, nt, 0);
            L.log("[birnd] "+ofToString((char)tk)+" delay = "+ofToString(nt*nt));
            
            state=0;
            cmdlog+="\n";
        }
    }
    else if(state==28)
    {
        if((key>=48 && key<=57) || (key>=97 && key<=122))
        {
            numtok+=ofToString(ckey);
            S.pop();
            cmdlog+=ofToString(ckey);
            
            string kafn(ofToString(bank)+numtok+".rhka");
            L.log("-> sourcing program: "+kafn);
            
            numtok.clear();
            state=0;
            sourcerhka(kafn);
        }
    }
    else if(state==30)
    {
        if(key>=97 && key<=122)
        {
            S.pop();
            bank=ckey;
            L.log("# source bank: "+ofToString(bank));
            state=0;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==29)
    {
        if(key==46) // .
        {
            numtok+=ckey;
            state=32;
            cmdlog+=ofToString(ckey);
        }
        else if(key==49) // 1
        {
            S.pop(); S.pop(); // pop = and r
            int oia=S.pop();
            char oc=(char)oia;
            int oi=oia-97;
            
            z.ox[oi].rmag=1.0;
            L.log("// "+ofToString(oc)+".rmag = 1.0");
            
            state=0;
            cmdlog+="1";
        }
    }
    else if(state==32)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ckey;
            state=33;
            cmdlog+=ofToString(ckey);
        }
    }
    else if(state==33)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ckey;
            state=33;
            cmdlog+=ofToString(ckey);
        }
        else if(key==13)
        {
            S.pop(); S.pop(); // pop = and r
            int oia=S.pop();
            char oc=(char)oia;
            int oi=oia-97;
            
            float vv=ofToFloat(numtok);
            z.ox[oi].rmag=vv;
            L.log("// "+ofToString(oc)+".rmag = "+ofToString(vv));
            
            numtok.clear();
            state=0;
            cmdlog+="\n";
        }
    }
    else if(state==37)
    {
        if(key==46) // .
        {
            numtok+=ckey;
            state=35;
            cmdlog+=ckey;
        }
        else if(key==49) // 1
        {
            S.pop(); S.pop();
            int oia=S.pop();
            char oic=(char)oia;
            int oi=oia-97;
            
            z.ox[oi].amag=1.0;
            L.log("// "+ofToString(oic)+".amag = 1.0");
            
            state=0;
            cmdlog+="1";
        }
    }
    else if(state==35)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ckey;
            state=34;
            cmdlog+=ckey;
        }
    }
    else if(state==34)
    {
        if(key>=48 && key<=57)
        {
            numtok+=ckey;
            state=34;
            cmdlog+=ckey;
        }
        else if(key==13)
        {
            S.pop(); S.pop();
            int oia=S.pop();
            char oic=(char)oia;
            int oi=oia-97;
            
            z.ox[oi].amag=ofToFloat(numtok);
            L.log("// "+ofToString(oic)+".amag = "+numtok);
            numtok.clear();
            
            state=0;
            cmdlog+="\n";
        }
    }
    else if(state==31)
    {
        if(key==13)
        {
            state=0;
            S.pop(); S.pop();
            cmdlog+="\n";
            L.log("# wave pencil ended");
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    kpevt(key);
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

