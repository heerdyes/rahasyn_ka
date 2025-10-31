#pragma once

#include "ofMain.h"

#define TBL_MAX_N   (16384)

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

    void jya()
    {
        for(int i=0;i<sz;i++)
        {
            buf[i]=sin(i*2*PI/(float)sz);
        }
    }

    void birnd()
    {
        for(int i=0;i<sz;i++)
        {
            buf[i]=ofRandom(-1,1);
        }
    }

    void urnd()
    {
        for(int i=0;i<sz;i++)
        {
            buf[i]=ofRandom(0,1);
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
    float rateref; // reference rate
    float ampref;  // reference amp
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
        rateref=r0;
        ampref=a0;
    }

    void setra(float rr, float aa)
    {
        rateref=rr;
        ampref=aa;
    }

    void setr(float rr)
    {
        rateref=rr;
        rate=rr;
    }

    void seta(float aa)
    {
        ampref=aa;
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
        if(rtlo!=-1) rate=abs(rateref*ox[rtlo].samp(tx));
        if(atlo!=-1) amp=abs(ampref*ox[atlo].samp(tx));
    }

    float samp(tbl2 tx[])
    {
        return amp * tx[tid].buf[(int)round(ptr)];
    }
};
