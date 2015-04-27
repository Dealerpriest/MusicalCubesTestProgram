#pragma once

class ofApp;
#include "ofMain.h"
//#include "sequencer.h"


class Cube
{
    public:
        /** Default constructor */
        Cube();
        ~Cube();

        static ofEvent<vector <int>> cubeTriggered;

        //functions
        void setup(int index, ofApp* app);
        void clicked(ofMouseEventArgs& args);
        void receiveBeat();

        //variables
        int index;
        uint8_t currentEffect;
        bool active = false;
        bool recordRequested = false;
        bool recording = false;
//        bool copying = false;
//        int copyingTo;
//        ofCircle copyCircle;
        ofRectangle rectangle;
        ofColor color;


    protected:
    private:
        ofApp* app;
};
