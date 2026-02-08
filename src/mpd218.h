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
            char x;
            mpdev.read(&x, 1);
            
            int b=x&0xff;
            int hi=b>>4;   // cmd
            int lo=b&0x0f; // channel
            
            // (de)bugging
            //char ln[20];
            //sprintf(ln, "[knobd] 0x%02x%02x", hi,lo);
            //ofLog()<<ofToString(ln);
            
            mde evt;
            if(hi==0x0f && lo==0x08) continue; // midi clock
            else if(hi==0x09) // note on
            {
                mpdev.read(&x, 1);
                int d1=x&0xff;
                mpdev.read(&x, 1);
                int d2=x&0xff;
                
                evt.setup(hi,lo, d1,d2);
                qref->nq(evt);
            }
            else if(hi==0x08) // note off
            {
                mpdev.read(&x, 1);
                int d1=x&0xff;
                mpdev.read(&x, 1);
                int d2=x&0xff;
                
                evt.setup(hi,lo, d1,d2);
                qref->nq(evt);
            }
            else if(hi==0x0b) // CC
            {
                mpdev.read(&x, 1);
                int d1=x&0xff;
                mpdev.read(&x, 1);
                int d2=x&0xff;
                
                evt.setup(hi,lo, d1,d2);
                qref->nq(evt);
            }
            else if(hi==0x0d) // aftertouch / pressure
            {
                mpdev.read(&x, 1);
                int d1=x&0xff;
                
                evt.setup(hi,lo, d1,0);
                qref->nq(evt);
                // currently not supported for osc rule
            }
        }
        mpdev.close();
    }
    
    std::ifstream mpdev;
    string state;
    string midf;
    iq *qref;
};

