#include "sequencer.h"
#include "ofApp.h"

Sequencer::Sequencer(ofApp* app, int tempo = 120)
{
    this->app = app;
    bpm = tempo;
    interval = 1000*60 / bpm;

    lastBeat = lastBeat = ofGetElapsedTimeMillis();
}

void Sequencer::init()
{
//    this->cubes = cubes;
}

void Sequencer::setBPM(int bpm)
{
    interval = 1000*60 / bpm;
    nextBeat = lastBeat + interval;
}

void Sequencer::updateSeq()
{
    if(!run){
        return;
    }
    unsigned long long now = ofGetElapsedTimeMillis();
    if(now >= nextBeat){
        lastBeat = nextBeat;
        while(now >= nextBeat){
            nextBeat += interval;
        }

        app->cubes[nextCube].receiveBeat();
        nextCube++;
        nextCube %= app->cubes.size();
    }
}

void Sequencer::start(bool restart)
{
    run = true;
    if(restart){
        nextCube = 0;
    }
}

void Sequencer::stop()
{
    run = false;
}
