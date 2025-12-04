#pragma once

#include "ofMain.h"

class rp2
{
public:
    // pico i/f
	ofSerial ser;
	string sln;
	int kn0;
	
	int baud;
	string dev;
	char state;
	
	void setup()
	{
	    state='0';
	    baud=115200;
	    dev="/dev/ttyACM0";
	    ser.setup(dev, 115200);
	}
	
	void eatbyte(int b)
	{
	    char cb=(char)b;
        if(cb=='\n')
        {
            kn0=ofToInt(sln);
            sln.clear();
        }
        else
        {
            sln+=ofToString(cb);
        }
	}
	
	void listen(lg & L)
    {
        int b=ser.readByte();
        if(state=='0')
        {
            if(b==OF_SERIAL_NO_DATA) state='w';
            else if(b==OF_SERIAL_ERROR)
            {
                state='e';
                L.log("!! pico error !!");
            }
            else
            {
                state='r';
                eatbyte(b);
            }
        }
        else if(state=='w')
        {
            if(b==OF_SERIAL_NO_DATA){}
            else if(b==OF_SERIAL_ERROR)
            {
                state='e';
                L.log("!! pico error !!");
            }
            else
            {
                state='r';
                eatbyte(b);
            }
        }
        else if(state=='e')
        {
            if(b==OF_SERIAL_NO_DATA) state='w';
            else if(b==OF_SERIAL_ERROR){}
            else
            {
                state='r';
                eatbyte(b);
            }
        }
        else if(state=='r')
        {
            if(b==OF_SERIAL_NO_DATA) state='w';
            else if(b==OF_SERIAL_ERROR)
            {
                state='e';
                L.log("!! pico error !!");
            }
            else eatbyte(b);
        }
    }
    
    void quit()
    {
        ser.close();
    }
};

