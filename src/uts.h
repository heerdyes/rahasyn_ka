#pragma once

#include "ofMain.h"

#define STK_MAX (64)
#define LOG_MAX (18)

#define SNHALF  (0.5 * 8)

class ut
{
public:
    // refer project splinear for more splines
    void spline2(float x1,float y1, float x2,float y2, float p,float q, int n, string st)
    {
        float mul=(1.0 / (float)n);
        float ox1=x1;
        float oy1=y1;

        for(int i=1;i<n+1;i++)
        {
            float d=mul*i;
            float zx1=x1*pow(1.0-d, 2) + 2*p*(1.0-d)*d + x2*pow(d, 2);
            float zy1=y1*pow(1.0-d, 2) + 2*q*(1.0-d)*d + y2*pow(d, 2);

            float dst=abs((fctr%n)-i);
            if(dst<2) CS2Y;
            else CS2TB;

            ofDrawLine(ox1,oy1, zx1,zy1);

            if(i==n/2)
            {
                CS2LBL;
                float shalf=st.length()*SNHALF;
                ofDrawBitmapString(st, zx1-shalf, zy1-8);
            }

            ox1=zx1;
            oy1=zy1;
        }
    }

    void spline3(float x1,float y1, float x2,float y2, float p1,float q1, float p2,float q2, int n, string st)
    {
        float mul=(1.0 / (float)n);
        float ox1=x1;
        float oy1=y1;

        for(int i=1;i<n+1;i++)
        {
            float d=mul*i;
            float zx1=x1*pow(1.0-d, 3) + 3*p1*pow(1.0-d, 2)*d + 3*p2*(1.0-d)*pow(d, 2) + x2*pow(d, 3);
            float zy1=y1*pow(1.0-d, 3) + 3*q1*pow(1.0-d, 2)*d + 3*q2*(1.0-d)*pow(d, 2) + y2*pow(d, 3);

            float dst=abs((fctr%n)-i);
            if(dst<3) CS3Y;
            else CS3TB;

            ofDrawLine(ox1,oy1, zx1,zy1);

            if(i==n/2)
            {
                CS3LBL;
                float shalf=st.length()*SNHALF;
                ofDrawBitmapString(st, zx1-shalf, zy1-8);
            }

            ox1=zx1;
            oy1=zy1;
        }
    }

    void edge2(nd& n0, nd& n1, float p,float q, string st)
    {
        spline2(n0.x,n0.y, n1.x,n1.y, p,q, 22, st);
    }

    void edge3(nd& n0, nd& n1, float p1,float q1, float p2,float q2, string st)
    {
        spline3(n0.x,n0.y, n1.x,n1.y, p1,q1, p2,q2, 22, st);
    }

    void update()
    {
        fctr++;
    }
    
    // tcmd format, styled like midi
    // cmd byte followed by 2 data bytes
    int tcmd_pack(int cmd, int d1, int d2)
    {
        return ((cmd&0xff)<<16) + ((d1&0xff)<<8) + (d2&0xff);
    }
    
    void tcmd_unpack(int pkt, int *cmd, int *d1, int *d2)
    {
        *d2=pkt&0xff;
        *d1=(pkt>>8)&0xff;
        *cmd=(pkt>>16)&0xff;
    }

    int fctr=0;
};

class cstk
{
public:
    int buf[STK_MAX];
    int sp;

    cstk()
    {
        sp=0;
        for(int i=0;i<STK_MAX;i++) buf[i]='0';
    }

    void push(int a)
    {
        if(sp>=STK_MAX-1)
        {
            cout<<"[error] stack overflow!\n";
            return;
        }

        buf[sp]=a;
        sp++;
    }

    int pop()
    {
        if(sp<=0)
        {
            cout<<"[error] empty stack\n";
            return -1;
        }

        sp--;
        return buf[sp];
    }

    void rndr(float x, float y, float hgap)
    {
        float yy=y-10;
        float xx=x-18;
        STKLBL;
        ofDrawLine(x-18,y, x+18,y);

        for(int i=0;i<sp;i++)
        {
            int k=buf[i];
            if(k==57355) ofDrawBitmapString("F12", xx,yy);
            else if(k==57344) ofDrawBitmapString("F1", xx,yy);
            else if(k==57345) ofDrawBitmapString("F2", xx,yy);
            else if(k==57346) ofDrawBitmapString("F3", xx,yy);
            else if(k==57347) ofDrawBitmapString("F4", xx,yy);
            else ofDrawBitmapString("["+ofToString(k)+"] "+ofToString((char)k), xx,yy);
            yy-=hgap;
        }

        yy-=8;
        STKLBL;
        ofDrawLine(x-18,yy, x+18,yy);
    }
};

class lg
{
public:
    float x,y;
    string buf[LOG_MAX];
    int pos;

    lg()
    {
        setup(770, HH-160);
    }

    void setup(float xx, float yy)
    {
        x=xx;
        y=yy;
        pos=0;
    }

    void rndr(float ht)
    {
        float yy=y;
        LGCOLOR;
        for(int i=(pos+1)%LOG_MAX; i!=pos; i=(i+1)%LOG_MAX)
        {
            ofDrawBitmapString(buf[i], x,yy);
            yy+=ht;
        }
    }

    void log(string s)
    {
        buf[pos]=s;
        pos=(pos+1)%LOG_MAX;
    }
};

