#pragma once

class ofApp;
#include "ofMain.h"
#include "cube.h"

class Sequencer{
    public:
        Sequencer(ofApp* app, int tempo);

        void init();
//        void attach(ofApp app);
        void setBPM(int bpm);

        void updateSeq();
        void start(bool restart = false);
        void stop();
    protected:
    private:
        ofApp* app;
        int bpm = 120;
        bool run = false;
        unsigned long interval;
        unsigned long long nextBeat;
        unsigned long long lastBeat;
        int nextCube = 0;
};
