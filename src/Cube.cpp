#include "cube.h"
#include "ofApp.h"

ofEvent<vector <int>> Cube::cubeTriggered = ofEvent<vector <int>>();

Cube::Cube()
{
    currentEffect = 0;
    this->app = app;

    //Doesnt work. probably because vector init makes copies instead of calling constructor
    //ctor
}

Cube::~Cube(){
    ofRemoveListener(ofEvents().mouseReleased, this, &Cube::clicked);
}

void Cube::setup(int index, ofApp* app)
{
    this->app = app;
    this->index = index;
    ofAddListener(ofEvents().mouseReleased, this, &Cube::clicked);
}

void Cube::clicked(ofMouseEventArgs& args)
{
//    ofLog() << "callback called for cube: " << this->index;
//    ofLog() << "cords: " << args.x << ", " << args.y;
//    ofLog() << rectangle.getLeft() << "  " << rectangle.getRight() << ",  " << rectangle.getTop() << "  " << rectangle.getBottom();

    if(rectangle.inside(args.x, args.y)){
//        ofLog() << "cube " << this->index << "clicked";
        if(recordRequested){
            recordRequested = false;
            if(args.button == 0){
                recording = true;
                app->sendRecordStart(index);
            }else{
                app->sendRecordTimeout(index);
                app->seq->start();
            }
        }else if(recording){
            recording = false;
            app->sendRecordStop(index);
            app->seq->start();
        }else if(active){
            color = ofColor(0,0,0);
            active = false;
            app->sendCubeOff(index);
        }else{
            int clickedPoint = ofMap(args.y - rectangle.getY(), 0, rectangle.getHeight(), 0, 255, true);
            currentEffect = clickedPoint;
            active = true;

        }
    }
}

void Cube::draw(){
    color = ofApp::getRainbowColor(currentEffect);
    if(recordRequested){
        ofSetColor(255,255,0);
    }else if(recording){
        ofSetColor(255,0,0);
    }else if(active){
        ofSetColor(color);
    }else{
        ofSetColor(240, 240, 240);
    }
    ofRect(rectangle);
    ofSetColor(0,0,0);
    ofDrawBitmapString(ofToString((int)currentEffect), rectangle.getPosition());

}

void Cube::receiveBeat()
{
    if(active){
        //ofLog() << "trigger cube: " << index;
        vector <int> args = vector<int>(2, 0);
        args[0] = index;
        args[1] = currentEffect;
        ofNotifyEvent(cubeTriggered, args);
    }
}


