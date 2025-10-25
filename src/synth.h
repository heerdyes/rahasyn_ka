#pragma once

#include "ofMain.h"

#define SMPL_RATE   (44100)
#define BUFR_SIZE   (512)
#define SYNBUF_SZ   (512)
#define TBL_MAX_N   (16384)
#define NTBL        (26)
#define NTLO        (26)

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
        // bipolar tables
        for(int i=0;i<NTBL;i++)
        {
            tx[i].setup(512);
            tx[i].dramp();
        }

        tx[11].pulse(.1);
    }

    void inittlo()
    {
        for(int i=0;i<NTLO;i++)
        {
            ox[i].setup(0, 1.0, 1.0);
        }

        ox[4].setup(11, .75, 1.0);
        ox[5].setup(0, 1.5, 5.0);
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
};
