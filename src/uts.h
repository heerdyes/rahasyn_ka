#pragma once

#include "ofMain.h"

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
            if(dst<3) ofSetColor(255,128,0);
            else ofSetColor(0,255,0);

            ofDrawLine(ox1,oy1, zx1,zy1);

            if(i==n/2)
            {
                ofSetColor(23,202,232);
                ft.drawString(st, zx1, zy1-8);
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
                ft.drawString(st, zx1, zy1-8);
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
