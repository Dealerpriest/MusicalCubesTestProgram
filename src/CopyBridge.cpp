#include "CopyBridge.h"

ofEvent<CopyBridgeEventArgs> CopyBridge::copyingDone = ofEvent<CopyBridgeEventArgs>();

CopyBridge::CopyBridge(Cube* from, Cube* to)
{
    this->from = from;
    this->to = to;
    position = from->rectangle.getCenter().getMiddle((ofVec3f) to->rectangle.getCenter());
    circle.circle(position, radius);
}

CopyBridge::~CopyBridge()
{
    ofRemoveListener(ofEvents().mouseReleased, this, &CopyBridge::clicked);
}

void CopyBridge::setup(){
    ofAddListener(ofEvents().mouseReleased, this, &CopyBridge::clicked);
}

void CopyBridge::clicked(ofMouseEventArgs& args)
{
    if(ofVec2f(args.x, args.y).distance((ofVec2f) position) <radius){
        ofRemoveListener(ofEvents().mouseReleased, this, &CopyBridge::clicked);
        ofLog() << "bridge clicked";
        CopyBridgeEventArgs args;
        args.from = this->from->index;
        args.to = this->to->index;
        removeMe = true;
        ofNotifyEvent(copyingDone, args);
    }
}

bool CopyBridge::shouldRemoveBridge(CopyBridge& bridge)
{
    return bridge.removeMe;
}


void CopyBridge::draw()
{
    circle.draw();
}
