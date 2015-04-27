#pragma once
#include "ofMain.h"
#include "Cube.h"
//#include "CopyBridgeEvent.h"
class CopyBridgeEventArgs;

class CopyBridge
{
    public:
        /** Default constructor */
        CopyBridge(Cube* from, Cube* to);
        /** Default destructor */
        ~CopyBridge();
        void setup();
        void clicked(ofMouseEventArgs& args);
        void draw();
        bool static shouldRemoveBridge(CopyBridge& bridge);

        static ofEvent<CopyBridgeEventArgs> copyingDone;

        bool removeMe = false;

    protected:
    private:
        static const int radius = 70;
        Cube* from;
        Cube* to;
        ofPoint position;
        ofPath circle;
};


class CopyBridgeEventArgs : public ofEventArgs {

public:

    int from;
    int to;
    CopyBridge* sender;


    CopyBridgeEventArgs() {
//        from = NULL;
//        to    = NULL;
//        sender = NULL;
    }
};

