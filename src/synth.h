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
        tx[10].pulse(.125,.5);
        
        tx[11].setup(512);
        tx[11].squ();
        
        tx[12].setup(512);
        tx[12].btri();
        
        tx[13].setup(512);
        tx[13].dcy(.99);
        
        tx[14].setup(512);
        tx[14].dcy(-.99);
        
        tx[15].setup(512);
        tx[15].wqspline(100,1, 412,-1, 600);
        
        tx[16].setup(512);
        tx[16].wqspline(80,2.4, 162,-2.2, 600);
        
        tx[17].setup(512);
        tx[17].dcy(.975);
        
        tx[18].setup(512);
        tx[18].dcy(.975,(1.0/4.0));
        
        tx[19].setup(512);
        tx[19].dcy(.975,(3.0/8.0));
        
        tx[20].setup(512);
        tx[20].dcy(.975,(5.0/8.0));
        
        tx[21].setup(512);
        tx[21].dcy(.975,.5);
        
        tx[22].setup(512);
        tx[22].dcy(.975,(7.0/8.0));
        
        tx[22].setup(512);
        tx[22].dcy(.975,(1.0/8.0));
    }

    void inittlo()
    {
        ox[0].setup(0, 1.0, 0.0);
        for(int i=1;i<NTLO;i++)
        {
            ox[i].setup(i, 1.0, 1.0);
        }
    }

    void initvox()
    {
        // vox init
        v0=0;
        v1=0;
        v2=0;
        v3=0;
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
    void rndrtbl(float x,float y, float w,float h, int ti)
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

        ofDrawBitmapString(ofToString((char)(65+ti)), x-5,y+h/2);

        // x axis
        ofSetColor(255,88,0);
        ofDrawLine(x-w/2,y,x+w/2,y);
    }

    void rndrtbl(int ti)
    {
        rndrtbl(tblxywh[ti].x, tblxywh[ti].y, tblxywh[ti].w, tblxywh[ti].h, ti);
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

            rndrtbl(i);
        }
    }

    // tlo rendering
    void rndrtlo(float x, float y, float w, float h, int oi)
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
        ofDrawBitmapString(ofToString((char)(97+oi)), x-5,y+h/2);

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

    void rndrtlo(int oi)
    {
        rndrtlo(tloxywh[oi].x, tloxywh[oi].y, tloxywh[oi].w, tloxywh[oi].h, oi);
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

            rndrtlo(i);
        }
    }

    void rndrvox(float x, float y)
    {
        float r=33;
        float ym=4;
        float xm=5;

        ofSetColor(222,99,111);
        ofDrawLine(x,y-r, x,y+r);
        ofDrawLine(x-r,y, x+r,y);

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
    
    void panic()
    {
        ox[v0].seta(.0);
        ox[v1].seta(.0);
        ox[v2].seta(.0);
        ox[v3].seta(.0);
    }
    
    void resetptrs()
    {
        for(int i=0;i<NTLO;i++) ox[i].ptr=0.0;
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
