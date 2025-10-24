#pragma once

#include "ofMain.h"
#include <atomic>

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
    int ptr;

    void setup(int n)
    {
        sz=n;
        ptr=0;
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

    void incptr()
    {
        ptr=(ptr+1)%sz;
    }

    float samp()
    {
        return buf[ptr];
    }
};

class tbl
{
public:
    float buf[TBL_MAX_N];
    int sz;
    float ptr;

    tbl()
    {
        ptr=0.0;
    }

    void setup(int n)
    {
        if(n>TBL_MAX_N || n<0) sz=TBL_MAX_N;
        else sz=n;
    }

    float sampnow() { return buf[(int)round(ptr)]; }

    void mvptr(float r)
    {
        float _ptr=ptr+r;
        if(_ptr>sz-1) _ptr-=sz-1;
        ptr=_ptr;
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

    tlo()
    {
        rtlo=-1;
        atlo=-1;
    }

    void setup(int tt, float r0, float a0)
    {
        tid=tt;
        rate=r0;
        amp=a0;
    }

    void evolve(tbl tx[], tlo ox[])
    {
        tx[tid].mvptr(rate);
        if(rtlo!=-1) rate=ox[rtlo].samp(tx);
        if(atlo!=-1) amp=ox[atlo].samp(tx);
    }

    float samp(tbl tx[]) { return amp*tx[tid].sampnow(); }
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
            if(ctr2%1==0)
            {
                ox[v0].evolve(tx,ox);           // audio rate
                ox[v1].evolve(tx,ox);           // table lookup
                ox[v2].evolve(tx,ox);           // oscillator
                ox[v3].evolve(tx,ox);           // evolution

                // evolve the remaining kontrol rate oscs without looping
                int rem=ctr%NTLO;
                if(rem==v0 || rem==v1 || rem==v2 || rem==v3) {}
                else ox[rem].evolve(tx,ox);
            }

            ctr++;
            if(ctr%4==0) ctr2++;

            if(ctr>444444) ctr=0;
            if(ctr2>444444) ctr2=0;
        }
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
    }

    void inittlo()
    {
        for(int i=0;i<NTLO;i++)
        {
            ox[i].setup(0, 1.0, 1.0);
        }
    }

    void initvox()
    {
        // vox init
        v0=0; v1=1; v2=2; v3=3;
        ox[v0].rate=.25;
        ox[v1].amp=.0;
        ox[v2].amp=.0;
        ox[v3].amp=.0;
    }

    /// This drawing function cannot be called from the thread itself because
    /// it includes OpenGL calls
    void draw(){}

    // vars
    int ctr = 0;
    int ctr2 = 0;
    float synbuf[SYNBUF_SZ];

    // table list
    tbl tx[NTBL];

    // oscillators
    tlo ox[NTLO];

    // 4 voices
    int v0, v1, v2, v3;
    float mgain;
};
