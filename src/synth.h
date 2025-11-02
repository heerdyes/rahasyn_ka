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

    // TODO: sound engine assist
    void threadedFunction()
    {
        while(isThreadRunning())
        {
            // perhaps table evolution
            // or recurrence/fx module
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

    void update(){}

    void updateNoLock(){}

    void inittbl()
    {
        // bipolar sawteeth
        tx[0].setup(512);
        tx[0].dramp();
        
        // adsyn
        tx[1].setup(512);
        tx[1].jya();
        
        tx[2].setup(1024);
        tx[2].dvijya(1,1, 2,1);
        
        tx[3].setup(1024);
        tx[3].trijya(1,2, 2,1, 4,1);
        
        // triangles
        tx[4].setup(512);
        tx[4].utri();

        // noise
        tx[5].setup(512);
        tx[5].birnd();
        
        tx[6].setup(512);
        tx[6].urnd();

        // wild pulses
        tx[7].setup(512);
        tx[7].pulse(.5);
        
        tx[8].setup(512);
        tx[8].pulse(.25);
        
        tx[9].setup(512);
        tx[9].pulse(.125);
        
        tx[10].setup(512);
        tx[10].pulse(.0625);
    }

    void inittlo()
    {
        for(int i=0;i<NTLO;i++)
        {
            ox[i].setup(i%16, 1.0, 1.0);
        }

        ox[16].setup(11, .75, 0.0);
    }

    void initvox()
    {
        // vox init
        v0=16; v1=16; v2=16; v3=16;
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

        ft.drawString(ofToString((char)(65+ti)), x-5,y+h/2);

        // x axis
        ofSetColor(255,88,0);
        ofDrawLine(x-w/2,y,x+w/2,y);
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

        // label
        ft.drawString(ofToString((char)(97+oi)), x-5,y+h/2);

        // x axis
        ofSetColor(255,88,0);
        ofDrawLine(x-w/2,y,x+w/2,y);

        float xp=x-w/2 + getoscptr(oi) * w / (float) tsz;
        ofDrawLine(xp,y+h/2,xp,y-h/2);

        // spline to mod tbl, if applicable
        int tbix=oref.tid;
        float xdst=tblxywh[tbix].x;
        float ydst=tblxywh[tbix].y;
        string sosc=ofToString((char)(97+oi));
        u.spline2(xdst,ydst, x,y, spltx,splty, 22, sosc+"t"+ofToString((char)(65+tbix)), ft);

        // display modulation patch cable splines
        if(oref.ratemodded())
        {
            int rtlo=oref.rtlo;
            float oxdst=tloxywh[rtlo].x;
            float oydst=tloxywh[rtlo].y;
            u.spline2(oxdst,oydst, x,y, splox,sploy, 22, sosc+"r"+ofToString((char)(97+rtlo)), ft);
        }
        if(oref.ampmodded())
        {
            int atlo=oref.atlo;
            float oxdst=tloxywh[atlo].x;
            float oydst=tloxywh[atlo].y;
            u.spline2(oxdst,oydst, x,y, splox,sploy, 22, sosc+"a"+ofToString((char)(97+atlo)), ft);
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

    void rndrvox(float x, float y, ofTrueTypeFont ff)
    {
        float r=33;
        float ym=4;
        float xm=5;

        ofDrawLine(x,y-r, x,y+r);
        ofDrawLine(x-r,y, x+r,y);

        ff.drawString(ofToString((char)(v0+97)), x+r,y+ym);
        ff.drawString(ofToString((char)(v1+97)), x-xm,y-r-ym);
        ff.drawString(ofToString((char)(v2+97)), x-r-2*xm,y+ym);
        ff.drawString(ofToString((char)(v3+97)), x-xm,y+r+2*ym);
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

    ut u; // TODO: make a singleton utility available for all classes
    float splox=WW/2, sploy=HH/2;
    float spltx=WW/2, splty=HH/2;
};
