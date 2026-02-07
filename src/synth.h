#pragma once

#include "ofMain.h"

#define SMPL_RATE   (44100)
#define BUFR_SIZE   (512)
#define SYNBUF_SZ   (512)
#define NTBL        (26)
#define NTLO        (26)

class syn: public ofThread
{
public:
    ~syn()
    {
        stop();
        waitForThread(false);
    }

    void setup(iq *qq)
    {
        qref=qq;
        mgain=0.125;
        mdx=-1;

        inittbl();
        inittlo();
        initvox();

        start();
    }

    void start()
    {
        startThread();
    }

    void stop()
    {
        stopThread();
    }
    
    // sound engine assist
    void threadedFunction()
    {
        while(isThreadRunning())
        {
            // midimon
            if(!qref->mt())
            {
                int b=qref->dq();
                if(b==153)
                {
                    if(mdx>=0) ox[mdx].trigger();
                }
            }
            
            // tcmd computer
            for(int i=0;i<NTBL;i++)
            {
                int cmd,d1,d2,d3;
                u.tcmd_unpack(tcmd[i], &cmd, &d1, &d2, &d3);

                if(cmd==2) // bipolar random
                {
                    float vv=ofRandom(-1,1);
                    if(d1==0 || d2==0) tx[i].sampwr(tclk, vv);
                    else
                    {
                        // triggered randomness causes timbral changes
                        if(ctr%(d1*d2)==0) tx[i].sampwr(tclk, vv);
                    }
                }
                else if(cmd==1) // unipolar random
                {
                    float vv=ofRandom(0,1);
                    if(d1==0 || d2==0) tx[i].sampwr(tclk, vv);
                    else
                    {
                        // control refresh rate of randomness
                        if(ctr%(d1*d2)==0) tx[i].sampwr(tclk, vv);
                    }
                }
                else if(cmd==8) // wavetable
                {
                    if(ox[d3].tid>=0)
                    {
                        float mixgen=ox[d3].samp(tx);
                        float mix1=ofMap(mixgen, -1,1, 1,0);
                        float mix2=1-mix1;
                        
                        tx[i].sampwr(tclk, mix1*tx[d1].samprd(tclk) + mix2*tx[d2].samprd(tclk));
                    }
                }
            }
            
            tclk=(tclk+1)%TBL_MAX_N;
        }
    }
    
    void evolve()
    {
        // no more a-rate k-rate separation
        if(v0>=0) ox[v0].evolve(tx,ox);
        if(v1>=0) ox[v1].evolve(tx,ox);           // table lookup
        if(v2>=0) ox[v2].evolve(tx,ox);           // oscillator
        if(v3>=0) ox[v3].evolve(tx,ox);           // evolution
        
        // evolve the remaining oscs without looping
        int rem=ctr%NTLO;
        if(rem==v0 || rem==v1 || rem==v2 || rem==v3) {}
        else ox[rem].evolve(tx,ox);

        ctr++;
        if(ctr>8888888) ctr=0;
    }

    float vsamp()
    {
        float vs0=v0==-1?0:ox[v0].samp(tx);
        float vs1=v1==-1?0:ox[v1].samp(tx);
        float vs2=v2==-1?0:ox[v2].samp(tx);
        float vs3=v3==-1?0:ox[v3].samp(tx);
        return mgain * (vs0+vs1+vs2+vs3);
    }

    void update(){}

    void updateNoLock(){}

    void inittbl()
    {
        // standard table size alloc
        for(int i=0;i<NTBL;i++) tx[i].setup(TBL_STD_N);
        
        // bipolar sawteeth
        tx[0].dramp();
        
        // adsyn
        tx[1].jya();
        tx[2].dvijya(1,1, 2,1);
        tx[3].setup(512);
        tx[3].trijya(1,2, 2,1, 4,1);
        
        // triangles
        tx[4].utri();
        tx[4].uni2bi();

        // noise
        tx[5].birnd();
        tcmd[5]=u.tcmd_pack(2, 2, 2, 0);

        // wild pulses
        tx[6].pulse(.09);
        tx[7].pulse(.33);
        tx[8].pulse(.25);
        tx[9].pulse(.125);
        tx[10].pulse(.125,.5);
        tx[11].squ();
        tx[12].btri();
        
        tx[13].dcy(.975);
        tx[13].uni2bi();
        
        tx[14].dcy(-.98);
        tx[15].wqspline(10,1, 150,-1, 500);
        tx[16].wqspline(80,2.4, 162,-2.2, 500);
        tx[17].wqspline(10,3.6, 250,-3.4, 500);
        
        tx[18].dcy(.96);
        tx[18].dcy(.96,.6666);
        tx[18].uni2bi();
        
        tx[19].dcy(.96);
        tx[19].dcy(.96,.3333);
        tx[19].uni2bi();
        
        tx[20].dcy(.95);
        tx[20].dcy(.975,.25);
        tx[20].uni2bi();
        
        tx[21].setup(1024);
        tx[21].trijya(.5,1, 1,3, 3,1);
        tx[22].setup(1024);
        tx[22].trijya(1,1, 3,3, 5,2);
        tx[23].setup(1024);
        tx[23].trijya(1,1, 4,3, 7,1);
        tx[24].setup(1024);
        tx[24].trijya(.75,1, 5,4, 2,1);
        tx[25].setup(512);
        tx[25].wqspline(130,3.6, 256,-3.0, 700);
    }

    void inittlo()
    {
        if(prepatch)
        {
            for(int i=0;i<NTLO;i++)
            {
                ox[i].setup(i, 1.0, 1.0);
            }
        }
    }

    void initvox()
    {
        // vox init
        v0=-1;
        v1=-1;
        v2=-1;
        v3=-1;
        // rgb oscils
        bo=8;
        ro=5;
        go=6;
    }

    void draw(){}

    tlo & getosc(int oi)
    {
        return ox[oi];
    }

    int gettblsz(int ti)
    {
        return tx[ti].sz;
    }

    float getosctblsamp(int oi, int j)
    {
        if(ox[oi].tid==-1) return 0.0;
        return tx[ox[oi].tid].buf[j];
    }

    int getosctblsz(int oi)
    {
        if(ox[oi].tid==-1) return 0;
        return tx[ox[oi].tid].sz;
    }

    float getoscptr(int oi)
    {
        return ox[oi].ptr;
    }

    float getoscamp(int oi)
    {
        return ox[oi].amp;
    }
    
    float getbo() { return abs(ox[bo].amp); }
    float getgo() { return abs(ox[go].amp); }
    float getro() { return abs(ox[ro].amp); }

    // tbl rendering
    void rndrtbl(float x,float y, float w,float h, int ti, float cx,float cy)
    {
        TRBLU;
        int tsz=gettblsz(ti);
        float xf=w / (float) tsz;
        float xx=x-w/2;
        float kk=h/2;

        for(int i=0;i<tsz;i++)
        {
            float yy=y - kk*tx[ti].buf[i];
            ofDrawLine(xx,y,xx,yy);
            xx+=xf;
        }

        // label is radially aligned
        float rd=.18;
        float lx=(1-rd)*x + rd*cx;
        float ly=(1-rd)*y + rd*cy;
        TRBLK;
        ofDrawEllipse(lx,ly-5,20,20);
        TRBLU;
        ofDrawBitmapString(ofToString((char)(65+ti)), lx-3,ly);

        // x axis
        AXISX;
        ofDrawLine(x-w/2,y,x+w/2,y);
    }

    void rndrtbl(int ti, float cx,float cy)
    {
        rndrtbl(tblxywh[ti].x, tblxywh[ti].y, tblxywh[ti].w, tblxywh[ti].h, ti, cx,cy);
    }

    void rndrtbls(float x, float y, float r)
    {
        // rndr 26 tbls
        float frac=2*PI/NTBL;
        for(int i=0;i<NTBL;i++)
        {
            float xq=x+r*cos(i*frac);
            float yq=y-r*sin(i*frac);

            tblxywh[i].x=xq;
            tblxywh[i].y=yq;
            tblxywh[i].w=72;
            tblxywh[i].h=40;

            rndrtbl(i, x,y);
            if(tcmd[i]!=0)
            {
                int cmd,d1,d2,d3;
                u.tcmd_unpack(tcmd[i], &cmd, &d1, &d2, &d3);
                
                if(cmd==8) // wavetable
                {
                    string pt="~"+ofToString((char)(65+i));
                    u.spline2(tblxywh[d1].x,tblxywh[d1].y, tblxywh[i].x,tblxywh[i].y, splwx,splwy, 22, pt+"1");
                    u.spline2(tblxywh[d2].x,tblxywh[d2].y, tblxywh[i].x,tblxywh[i].y, splwx,splwy, 22, pt+"2");
                    u.spline2(tloxywh[d3].x,tloxywh[d3].y, tblxywh[i].x,tblxywh[i].y, spltx,splty, 22, pt+"o");
                }
            }
        }
    }

    // tlo rendering
    void rndrtlo(float x, float y, float w, float h, int oi, float cx,float cy)
    {
        tlo & oref=getosc(oi);
        // label display
        float rd1=.18;
        float lx=(1-rd1)*x + rd1*cx;
        float ly=(1-rd1)*y + rd1*cy;
        // rate / amp display
        float rd2=.30;
        float lx2=(1-rd2)*x + rd2*cx;
        float ly2=(1-rd2)*y + rd2*cy;
        // rate / amp mod amt display
        float rd3=.45;
        float lx3=(1-rd3)*x + rd3*cx;
        float ly3=(1-rd3)*y + rd3*cy;
        
        TRBLU;
        if(oref.tid==-1)
        {
            ofDrawLine(x-w/2,y,x+w/2,y);
            // label is radially aligned
            string slbl=ofToString((char)(97+oi));
            
            if(oi==v0 || oi==v1 || oi==v2 || oi==v3) TRRED;
            else TRBLU;
            
            ofDrawBitmapString(slbl, lx,ly);
            ofDrawBitmapString(ofToString(oref.rate,3), lx2-18,ly2-10);
            ofDrawBitmapString(ofToString(oref.amp,3), lx2-18,ly2+10);
            
            ofDrawBitmapString(ofToString(oref.rmag,2), lx3-14,ly3-6);
            ofDrawBitmapString(ofToString(oref.amag,2), lx3-14,ly3+6);
            
            // signal trigability
            if(oref.trigo)
            {
                TRRED;
                ofDrawBitmapString("T", lx-7,ly-7);
            }
            return;
        }
        
        int tsz=getosctblsz(oi);
        float xf=w / (float) tsz;
        float xx=x-w/2;
        float kk=(h/2)*getoscamp(oi);

        for(int i=0;i<tsz;i++)
        {
            float yy=y - kk*getosctblsamp(oi,i);
            ofDrawLine(xx,y,xx,yy);
            xx+=xf;
        }

        // label is radially aligned
        string slbl=ofToString((char)(97+oi));
        TRBLK;
        if(oi==v0 || oi==v1 || oi==v2 || oi==v3) TRRED;
        else TRBLU;
        ofDrawBitmapString(slbl, lx,ly);
        ofDrawBitmapString(ofToString(oref.rate,3), lx2-18,ly2-10);
        ofDrawBitmapString(ofToString(oref.amp,3), lx2-18,ly2+10);
        // rate and amp mod amts
        ofDrawBitmapString(ofToString(oref.rmag,2), lx3-14,ly3-6);
        ofDrawBitmapString(ofToString(oref.amag,2), lx3-14,ly3+6);
        // signal trigability
        if(oref.trigo)
        {
            TRRED;
            ofDrawBitmapString("T", lx-7,ly-7);
        }

        // x axis
        AXISX;
        ofDrawLine(x-w/2,y,x+w/2,y);

        float xp=x-w/2 + getoscptr(oi) * w / (float) tsz;
        ofDrawLine(xp,y+h/2,xp,y-h/2);

        // spline to mod tbl, if applicable
        int tbix=oref.tid;
        float xdst=tblxywh[tbix].x;
        float ydst=tblxywh[tbix].y;
        string sosc=ofToString((char)(97+oi));
        u.spline2(xdst,ydst, x,y, spltx,splty, 22, sosc+"t"+ofToString((char)(65+tbix)));

        // display modulation patch cable splines
        if(oref.ratemodded())
        {
            int rtlo=oref.rtlo;
            float oxdst=tloxywh[rtlo].x;
            float oydst=tloxywh[rtlo].y;
            u.spline2(oxdst,oydst, x,y, splox,sploy, 22, sosc+"r"+ofToString((char)(97+rtlo)));
        }
        if(oref.ampmodded())
        {
            int atlo=oref.atlo;
            float oxdst=tloxywh[atlo].x;
            float oydst=tloxywh[atlo].y;
            u.spline2(oxdst,oydst, x,y, splox,sploy, 22, sosc+"a"+ofToString((char)(97+atlo)));
        }
    }

    void rndrtlo(int oi, float cx,float cy)
    {
        rndrtlo(tloxywh[oi].x, tloxywh[oi].y, tloxywh[oi].w, tloxywh[oi].h, oi, cx,cy);
    }

    void rndrtlos(float x, float y, float r)
    {
        // rndr 26 tlos
        float frac=2*PI/NTLO;
        for(int i=0;i<NTLO;i++)
        {
            float xq=x+r*cos(i*frac);
            float yq=y-r*sin(i*frac);

            tloxywh[i].x=xq;
            tloxywh[i].y=yq;
            tloxywh[i].w=72;
            tloxywh[i].h=40;

            rndrtlo(i, x,y);
        }
    }

    void rndrvox(float x, float y)
    {
        float r=33;
        float ym=4;
        float xm=5;

        VXXHAIR;
        ofDrawLine(x,y-r, x,y+r);
        ofDrawLine(x-r,y, x+r,y);

        BRCYAN;
        ofDrawBitmapString(ofToString((char)(v0+97)), x+r,y+ym);
        ofDrawBitmapString(ofToString((char)(v1+97)), x-xm,y-r-ym);
        ofDrawBitmapString(ofToString((char)(v2+97)), x-r-2*xm,y+ym);
        ofDrawBitmapString(ofToString((char)(v3+97)), x-xm,y+r+2*ym);
    }

    void updatesplo(float xx, float yy)
    {
        splox=xx;
        sploy=yy;
    }

    void updatesplt(float xx, float yy)
    {
        spltx=xx;
        splty=yy;
    }
    
    void updatesplw(float xx, float yy)
    {
        splwx=xx;
        splwy=yy;
    }
    
    void panic()
    {
        if(v0>=0) ox[v0].seta(.0);
        if(v1>=0) ox[v1].seta(.0);
        if(v2>=0) ox[v2].seta(.0);
        if(v3>=0) ox[v3].seta(.0);
    }
    
    void resetptrs()
    {
        for(int i=0;i<NTLO;i++)
        {
            if(!ox[i].trigo) ox[i].ptr=0.0;
        }
    }

    // vars
    int ctr = 0;
    unsigned int tclk = 0;
    float synbuf[SYNBUF_SZ];

    // table list
    tbl2 tx[NTBL];

    // oscillators
    tlo ox[NTLO];
    
    // midi reactor
    int mdx;
    
    // threaded table commands in tcmd format (refer uts.h)
    unsigned int tcmd[NTBL];

    // 4 voices
    int v0, v1, v2, v3;
    float mgain;
    // oscils for rgb channels
    int bo, ro, go;

    // tbl and tlo locations/sizes
    xywh tblxywh[NTBL];
    xywh tloxywh[NTLO];

    ut u; // TODO: make a singleton utility available for all classes
    float splox=3*WW/4, sploy=HH/2-64;
    float spltx=WW/2, splty=HH/2;
    float splwx=WW/4, splwy=HH/2-64;
    
    bool prepatch=false;
    
    // midi q
    iq *qref;
};

