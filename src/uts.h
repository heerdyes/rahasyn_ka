#pragma once

#include "ofMain.h"

#define STK_MAX (64)
#define LOG_MAX (8)

class ut
{
public:
    // refer project splinear for more splines
    void spline2(float x1,float y1, float x2,float y2, float p,float q, int n, string st, ofTrueTypeFont ft)
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
            if(dst<2) ofSetColor(255,128,0);
            else ofSetColor(0,108,0);

            ofDrawLine(ox1,oy1, zx1,zy1);

            if(i==n/2)
            {
                ofSetColor(244,180,22);
                float shalf=ft.stringWidth(st)/2;
                ft.drawString(st, zx1-shalf, zy1-8);
            }

            ox1=zx1;
            oy1=zy1;
        }
    }

    void spline3(float x1,float y1, float x2,float y2, float p1,float q1, float p2,float q2, int n, string st, ofTrueTypeFont ft)
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
            if(dst<3) ofSetColor(255,128,0);
            else ofSetColor(0,255,0);

            ofDrawLine(ox1,oy1, zx1,zy1);

            if(i==n/2)
            {
                ofSetColor(23,202,232);
                float shalf=ft.stringWidth(st)/2;
                ft.drawString(st, zx1-shalf, zy1-8);
            }

            ox1=zx1;
            oy1=zy1;
        }
    }

    void edge2(nd& n0, nd& n1, float p,float q, string st, ofTrueTypeFont fz)
    {
        spline2(n0.x,n0.y, n1.x,n1.y, p,q, 22, st, fz);
    }

    void edge3(nd& n0, nd& n1, float p1,float q1, float p2,float q2, string st, ofTrueTypeFont fz)
    {
        spline3(n0.x,n0.y, n1.x,n1.y, p1,q1, p2,q2, 22, st, fz);
    }

    void update()
    {
        fctr++;
    }

    int fctr=0;
};

class cstk
{
public:
    char buf[STK_MAX];
    int sp;

    cstk()
    {
        sp=0;
        for(int i=0;i<STK_MAX;i++) buf[i]='\0';
    }

    void push(char a)
    {
        if(sp>=STK_MAX-1)
        {
            cout<<"[error] stack overflow!\n";
            return;
        }

        buf[sp]=a;
        sp++;
    }

    char pop()
    {
        if(sp<=0)
        {
            cout<<"[error] empty stack\n";
            return -1;
        }

        sp--;
        return buf[sp];
    }

    void rndr(float x, float y, ofTrueTypeFont ff)
    {
        float yy=y-10;
        ofSetColor(0,255,255);
        ofDrawLine(x-10,y, x+10,y);

        for(int i=0;i<sp;i++)
        {
            ff.drawString(ofToString(buf[i]), x,yy);
            yy-=14;
        }

        ofSetColor(255);
        ofDrawLine(x-10,yy, x+10,yy);
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

    void rndr(ofTrueTypeFont ff, float ht)
    {
        float yy=y;
        ofSetColor(55,242,66);
        for(int i=(pos+1)%LOG_MAX; i!=pos; i=(i+1)%LOG_MAX)
        {
            ofSetColor(55,242,66);
            ff.drawString(buf[i], x,yy);
            yy+=ht;
        }
    }

    void log(string s)
    {
        buf[pos]=s;
        pos=(pos+1)%LOG_MAX;
    }
};
