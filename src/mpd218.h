#pragma once

#include "ofMain.h"

class mpdctl: public ofThread
{
public:
    ~mpdctl()
    {
        stopThread();
        waitForThread(false);
    }
    
    void setup(string mf, iq *qq)
    {
        qref=qq;
        midf=mf;
        mpdev.open(midf, std::ios::binary | std::ios::in);
        
        if(mpdev.fail())
        {
            state="failed while opening: "+midf;
            cout<<"[FATAL] "+state+"\n";
        }
        else
        {
            state="opened "+midf;
            startThread();
        }
    }
    
    void threadedFunction()
    {
        while(isThreadRunning())
        {
            // midi computer
            char x;
            mpdev.read(&x, 1);
            int b=x&0xff;
            qref->nq(b);
        }
        mpdev.close();
    }
    
    std::ifstream mpdev;
    string state;
    string midf;
    iq *qref;
};

