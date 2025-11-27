#pragma once

#include "ofMain.h"

#define TBL_MAX_N   (16384)
#define TBL_STD_N   (256)

class tbl2
{
public:
    float buf[TBL_MAX_N];
    int sz=TBL_STD_N;
    
    void wipe()
    {
        for (int i = 0; i < sz; i += 1)
        {
            buf[i]=0.0;
        }
    }

    void setup(int n)
    {
        resize(n);
        wipe();
    }
    
    void resize(int n)
    {
        if(n<=1) sz=2;
        else if(n>=TBL_MAX_N) sz=TBL_MAX_N;
        else sz=n;
    }
    
    void nrmlyz()
    {
        float max=0;
        // get abs max
        for(int i=0;i<sz;i++) if(abs(buf[i])>max) max=abs(buf[i]);
        // normalize
        for(int i=0;i<sz;i++) buf[i]/=max;
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

    // additive synthesizers
    void jya()
    {
        for(int i=0;i<sz;i++)
        {
            buf[i]=sin(i*2*PI/(float)sz);
        }
    }
    
    void dvijya(float f1,float w1, float f2,float w2)
    {
        float k=2*PI/(float)sz;
        
        for(int i=0;i<sz;i++)
        {
            buf[i]=w1*sin(i*k*f1) + w2*sin(i*k*f2);
        }
        
        nrmlyz();
    }
    
    void trijya(float f1,float w1, float f2,float w2, float f3,float w3)
    {
        float k=2*PI/(float)sz;
        
        for(int i=0;i<sz;i++)
        {
            buf[i]=w1*sin(i*k*f1) + w2*sin(i*k*f2) + w3*sin(i*k*f3);
        }
        
        nrmlyz();
    }
    
    void squ(float pwm=.5)
    {
        for (int i = 0; i < sz; i += 1)
        {
            buf[i]=(i<pwm*sz)?1.0:-1.0;
        }
    }
    
    void btri()
    {
        float acc=0;
        float k=4.0/sz;
        for (int i = 0; i < sz; i += 1)
        {
            buf[i]=acc;
            acc+=k;
            if(abs(acc)>1)
            {
                acc=acc>0?1.0:-1.0;
                k=-k;
            }
        }
    }
    
    void dcy(float gp=.5, float ph=.0)
    {
        float acc=1.0;
        int begat=ph<0?0:ph>=1?sz-1:(int)(ph*sz);
        
        for (int i = begat; i < sz; i += 1)
        {
            buf[i]=acc;
            acc*=gp;
        }
    }

    // noise synthesizers
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

    // pulsators
    void pulse(float _pwd=.5, float _beg=.0)
    {
        int ipwd=(_pwd<=0 || _pwd>=1)?.5:(int)(_pwd*sz);
        int ibeg=(_beg<=0 || _beg>=1)?.0:(int)(_beg*sz);
        int iend=ibeg+ipwd;
        
        for(int i=0;i<sz;i++)
        {
            if(i>=ibeg && i<=iend) buf[i]=1.0;
            else buf[i]=0.0;
        }
    }
    
    // splinewaves
    void wqspline(float p1,float q1, float p2,float q2, int n)
    {
        float x1=0,y1=0;
        float x2=sz-1,y2=0;
        float mul=(1.0 / (float)n);
        
        for(int i=1;i<n+1;i++)
        {
            float d=mul*i;
            float zx1=x1*pow(1.0-d, 3) + 3*p1*pow(1.0-d, 2)*d + 3*p2*(1.0-d)*pow(d, 2) + x2*pow(d, 3);
            float zy1=y1*pow(1.0-d, 3) + 3*q1*pow(1.0-d, 2)*d + 3*q2*(1.0-d)*pow(d, 2) + y2*pow(d, 3);
            
            int ix=zx1<0?0:zx1>sz-1?sz-1:(int)zx1;
            buf[ix]=zy1;
        }
        
        nrmlyz();
    }

    // triangulars
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
    
    void uni2bi()
    {
        for(int i=0;i<sz;i++)
        {
            buf[i]=2*abs(buf[i])-1;
        }
    }
    
    float samprd(int p)
    {
        return buf[p%sz];
    }
    
    void sampwr(int p, float v)
    {
        buf[p%sz]=v;
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
        rate=rateref=1.0;
        amp=ampref=1.0;
        tid=-1; // -1 causes system wide ramifications
    }

    bool ratemodded() { return rtlo!=-1; }
    bool ampmodded() { return atlo!=-1; }

    int tblsz(tbl2 tx[])
    {
        if(tid==-1) return 0;
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
        if(tid==-1) return;
        ptr+=rate;
        while(ptr>tx[tid].sz-1) ptr-=tx[tid].sz-1;
    }

    void evolve(tbl2 tx[], tlo ox[])
    {
        incptr(tx);
        if(rtlo!=-1) rate=abs(rateref*ox[rtlo].samp(tx));
        if(atlo!=-1) amp=abs(ampref*ox[atlo].samp(tx));
    }

    float samp(tbl2 tx[])
    {
        if(tid==-1) return 0.0;
        return amp * tx[tid].buf[(int)round(ptr)];
    }
};

