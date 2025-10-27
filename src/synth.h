#pragma once

#include "ofMain.h"

#define SMPL_RATE   (44100)
#define BUFR_SIZE   (512)
#define SYNBUF_SZ   (512)
#define TBL_MAX_N   (16384)
#define NTBL        (26)
#define NTLO        (26)

class xywh
{
public:
    float x,y,w,h;

    xywh()
    {
        x=y=w=h=0;
    }
};

class tbl2
{
public:
    float buf[TBL_MAX_N];
    int sz;

    void setup(int n)
    {
        sz=n;
        dramp();
    }

    void dramp()
    {
        float m=2.0 / (float)sz;
        float smp=1.0;
        for(int i=0;i<sz;i++)
        {
            buf[i]=smp;
            smp-=m;
        }
    }

    void uramp()
    {
        float m=1.0 / (float)sz;
        float smp=0.0;
        for(int i=0;i<sz;i++)
        {
            buf[i]=smp;
            smp+=m;
        }
    }

    void pulse(float _pwm=.5)
    {
        float pwm=(_pwm<0 || _pwm>1)?.5:_pwm;
        for(int i=0;i<sz;i++)
        {
            if(i<pwm*sz) buf[i]=1.0;
            else buf[i]=0.0;
        }
    }

    void utri()
    {
        float m=2.0/(float)sz;

        // asc
        float ac=0.0;
        for(int i=0;i<sz/2;i++)
        {
            buf[i]=ac;
            ac+=m;
        }

        // desc
        ac=1.0;
        for(int i=sz/2;i<sz;i++)
        {
            buf[i]=ac;
            ac-=m;
        }
    }

    void bi2uni()
    {
        for(int i=0;i<sz;i++)
        {
            buf[i]=(1.0+buf[i])/2;
        }
    }
};

class tlo
{
public:
    int tid;
    float rate;
    float amp;
    // mod tlos
    int rtlo;
    int atlo;
    float ptr;

    tlo()
    {
        rtlo=-1;
        atlo=-1;
        ptr=0;
    }

    bool ratemodded() { return rtlo!=-1; }
    bool ampmodded() { return atlo!=-1; }

    tbl2 gettbl(tbl2 tx[])
    {
        return tx[tid];
    }

    int tblsz(tbl2 tx[])
    {
        return tx[tid].sz;
    }

    void setup(int tt, float r0, float a0)
    {
        tid=tt;
        rate=r0;
        amp=a0;
    }

    void setra(float rr, float aa)
    {
        rate=rr;
        amp=aa;
    }

    void incptr(tbl2 tx[])
    {
        ptr+=rate;
        if(ptr>tx[tid].sz-1) ptr-=tx[tid].sz-1;
    }

    void evolve(tbl2 tx[], tlo ox[])
    {
        incptr(tx);
        if(rtlo!=-1) rate=abs(ox[rtlo].samp(tx));
        if(atlo!=-1) amp=abs(ox[atlo].samp(tx));
    }

    float samp(tbl2 tx[])
    {
        return amp * tx[tid].buf[(int)round(ptr)];
    }
};

class syn: public ofThread
{
public:
    ~syn()
    {
        stop();
        waitForThread(false);
    }

    void setup()
    {
        mgain=0.125;

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

    // the sound engine
    void threadedFunction()
    {
        while(isThreadRunning())
        {
            // some parallel code
        }
    }

    void aevolve()
    {
        ox[v0].evolve(tx,ox);           // audio rate
        ox[v1].evolve(tx,ox);           // table lookup
        ox[v2].evolve(tx,ox);           // oscillator
        ox[v3].evolve(tx,ox);           // evolution
    }

    void kevolve()
    {
        // evolve the remaining kontrol rate oscs without looping
        int rem=ctr%NTLO;
        if(rem==v0 || rem==v1 || rem==v2 || rem==v3) {}
        else ox[rem].evolve(tx,ox);

        ctr++;
        if(ctr>444444) ctr=0;
    }

    float vsamp()
    {
        return mgain * (ox[v0].samp(tx) + ox[v1].samp(tx) + ox[v2].samp(tx) + ox[v3].samp(tx));
    }

    void update()
    {}

    void updateNoLock()
    {}

    void inittbl()
    {
        // bipolar sawteeth
        for(int i=0;i<NTBL/2;i++)
        {
            tx[i].setup(512);
            tx[i].dramp();
        }

        // unipolar triangles
        for(int i=NTBL/2;i<NTBL;i++)
        {
            tx[i].setup(512);
            tx[i].utri();
        }

        // a wild pulse
        tx[11].pulse(.1);
    }

    void inittlo()
    {
        for(int i=0;i<NTLO;i++)
        {
            ox[i].setup(i%8, 1.0, 1.0);
        }

        ox[8].setup(11, .75, 1.0);
        ox[9].setup(0, 1.5, 5.0);
    }

    void initvox()
    {
        // vox init
        v0=0; v1=1; v2=2; v3=3;
        ox[v0].setra(2.0, .5);
        ox[v1].setra(3.0, .0);
        ox[v2].setra(1.0, .0);
        ox[v3].setra(1.0, .0);
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
        return tx[ox[oi].tid].buf[j];
    }

    int getosctblsz(int oi)
    {
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

    // tbl rendering
    void rndrtbl(float x,float y, float w,float h, int ti, ofTrueTypeFont ft)
    {
        ofSetColor(23,202,232);
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

        // x axis
        ofDrawLine(x-w/2,y,x+w/2,y);

        ofSetColor(255,88,0);
        ft.drawString(ofToString((char)(97+ti)), x-5,y+h/2);
    }

    void rndrtbl(int ti, ofTrueTypeFont ft)
    {
        rndrtbl(tblxywh[ti].x, tblxywh[ti].y, tblxywh[ti].w, tblxywh[ti].h, ti, ft);
    }

    void rndrtbls(float x, float y, float r, ofTrueTypeFont ft)
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

            rndrtbl(i, ft);
        }
    }

    // tlo rendering
    void rndrtlo(float x, float y, float w, float h, int oi, ofTrueTypeFont ft)
    {
        tlo & oref=getosc(oi);
        ofSetColor(23,202,232);
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

        // x axis
        ofDrawLine(x-w/2,y,x+w/2,y);

        ofSetColor(255,88,0);
        float xp=x-w/2 + getoscptr(oi) * w / (float) tsz;
        ofDrawLine(xp,y+h/2,xp,y-h/2);

        ft.drawString(ofToString((char)(97+oi)), x-5,y+h/2);

        // spline to mod tbl, if applicable
        int tbix=oref.tid;
        float xdst=tblxywh[tbix].x;
        float ydst=tblxywh[tbix].y;
        u.spline2(x,y, xdst,ydst, WW/2,50, 18, "o:"+ofToString(tbix), ft);

        if(oref.ratemodded())
        {
            int rtlo=oref.rtlo;
            float oxdst=tloxywh[rtlo].x;
            float oydst=tloxywh[rtlo].y;
            u.spline2(x,y, oxdst,oydst, WW/2,HH/2+50, 18, "r:"+ofToString(oref.rtlo), ft);
        }
        if(oref.ampmodded())
        {
            int atlo=oref.atlo;
            float oxdst=tloxywh[atlo].x;
            float oydst=tloxywh[atlo].y;
            u.spline2(x,y, oxdst,oydst, WW/2,HH-50, 18, "a:"+ofToString(oref.atlo), ft);
        }
    }

    void rndrtlo(int oi, ofTrueTypeFont ft)
    {
        rndrtlo(tloxywh[oi].x, tloxywh[oi].y, tloxywh[oi].w, tloxywh[oi].h, oi, ft);
    }

    void rndrtlos(float x, float y, float r, ofTrueTypeFont ft)
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

            rndrtlo(i, ft);
        }
    }

    // vars
    int ctr = 0;
    float synbuf[SYNBUF_SZ];

    // table list
    tbl2 tx[NTBL];

    // oscillators
    tlo ox[NTLO];

    // 4 voices
    int v0, v1, v2, v3;
    float mgain;

    // tbl and tlo locations/sizes
    xywh tblxywh[NTBL];
    xywh tloxywh[NTLO];

    ut u;
};
