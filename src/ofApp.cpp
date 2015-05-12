#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    serial.listDevices();
    ofLog() << "connect To Arduino returned: " << connectToArduino();

    cubes = vector<Cube>(nrCubes, Cube());
    //place the cubes
    for(int i = 0; i < nrCubes; i++){
        cubes[i].setup(i, this);
    }
    updateCubes();

    ofAddListener(Cube::cubeTriggered, this, &ofApp::onCubeTriggered);
    ofAddListener(CopyBridge::copyingDone, this, &ofApp::onCopyingDone);

    seq = new Sequencer(this, 120);

    //GUI
    gui.setup();
    run.addListener(this, &ofApp::toggleSequencer);
    gui.add(run.setup("run sequencer", false));
    tempo.addListener(this, &ofApp::setTempo);
    gui.add(tempo.setup("tempo", 120, 80, 140));
}

void ofApp::exit(){
    serial.close();
    run.removeListener(this, &ofApp::toggleSequencer);
    tempo.removeListener(this, &ofApp::setTempo);
    ofRemoveListener(Cube::cubeTriggered, this, &ofApp::onCubeTriggered);
    ofRemoveListener(CopyBridge::copyingDone, this, &ofApp::onCopyingDone);
}

//--------------------------------------------------------------
void ofApp::update(){
    handleSerial();
    updateCubes();
    seq->updateSeq();
//    seq->run = run;
}

//--------------------------------------------------------------
void ofApp::draw(){
    drawCubes();
    drawCopying();
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
 return;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

void ofApp::setTempo(int &bpm){
    seq->setBPM(bpm);
}

void ofApp::toggleSequencer(bool &value){
        if(value){
            seq->start();
        }else{
            seq->stop();
        }
}

void ofApp::onCubeTriggered(vector <int>& args)
{
//    ofLog() << "cube object Triggered from sequencer";
    sendTrigger(args[0], args[1]);
}

void ofApp::onCopyingDone(CopyBridgeEventArgs& args)
{
    sendCopyFinished(args.from, args.to);
    ofRemove(bridges, CopyBridge::shouldRemoveBridge);
}


void ofApp::sendTrigger(uint8_t cube, uint8_t effect){
    uint8_t message[5] = {'#', '/', 0, 0, '\n'};
    message[2] = cube;
    message[3] = effect;
    serial.writeBytes(&message[0], 5);
    ofLog() << "Cube: " << (int) cube << " triggered " << (int) effect;
}

void ofApp::sendCubeOff(uint8_t cube)
{
    uint8_t message[4] = {'#', 92, 0, '\n'};
    message[2] = cube;
    serial.writeBytes(&message[0], 4);
}

void ofApp::sendRecordStart(uint8_t cube)
{
    uint8_t message[4] = {'#', '[', 0, '\n'};
    message[2] = cube;
    serial.writeBytes(&message[0], 4);
}

void ofApp::sendRecordStop(uint8_t cube)
{
    uint8_t message[4] = {'#', ']', 0, '\n'};
    message[2] = cube;
    serial.writeBytes(&message[0], 4);
}

void ofApp::sendRecordTimeout(uint8_t cube)
{
    uint8_t message[4] = {'#', '!', 0, '\n'};
    message[2] = cube;
    serial.writeBytes(&message[0], 4);
}

void ofApp::sendCopyFinished(uint8_t from, uint8_t to)
{
    uint8_t message[5] = {'#', '*', 0, 0, '\n'};
    message[2] = from;
    message[3] = to;
    serial.writeBytes(&message[0], 5);
}

bool ofApp::connectToArduino(){
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    float stamp = ofGetElapsedTimef();
    for(int i = 0; i < deviceList.size(); i++){
        if(serial.setup(i, baudrate)){
            while(ofGetElapsedTimef() < stamp + i*2 +2){}//Spend two seconds checking each port
            ofLog() << "checking port: " << i;
            if(serial.readByte() == 'A'){
                ofLog() << "received an A!";
                serial.writeByte('A');
                return true;
            }
            serial.flush();
            serial.close();
        }
    }

    return false;

}

void ofApp::handleSerial()
{
    //Have we received any stuff?????? If so, do cool shit!
  if(serial.available() >= 2){
    if(serial.readByte() == '#'){
      uint8_t newLine;
//      ofLog() << "Command received";
      uint8_t command = readChar();
      if(command == 't'){
          unsigned char c = readChar();
          string message;
          while(c != '\n'){
              message.push_back(c);
              c = readChar();
          }
          ofLog() <<"message: " << message.c_str();
          return;
      }
      uint8_t affectedCube = readChar();

      if(command == '/'){//Cube is triggered
        uint8_t effect = readChar();
        newLine = readChar();
        if(newLine == '\n'){
            cubes[affectedCube].active = true;
            cubes[affectedCube].currentEffect = effect;
            ofLog() << "IR on Cube " << (int) affectedCube << "triggered";
        }
      }else if(command == 92){//Cube turned off
        newLine = readChar();
        if(newLine == '\n'){
            cubes[affectedCube].active = false;
            cubes[affectedCube].currentEffect = 0;
            sendCubeOff(affectedCube);
            ofLog() << " cube turned off :" << (int) affectedCube;
        }
      }else if(command == '['){//Recording requested
        newLine = readChar();
        if(newLine == '\n'){
//            run = false;
            seq->stop();
            cubes[affectedCube].recordRequested = true;
//            cubes[affectedCube].recording = true;
//            sendRecordStart(affectedCube);
            ofLog() << "Recording requested on cube " << (int) affectedCube;
        }
      }else if(command == '*'){//Copy request
        uint8_t affectedCube2 = readChar();
        newLine = readChar();
        if(newLine == '\n'){
            createBride(affectedCube, affectedCube2);
            ofLog() << "Copying requested between " << (int) affectedCube << " and " << (int) affectedCube2;
        }
      }else if(command == '>'){//Start request
        if(affectedCube == '\n'){
            seq->start();
        }
      }
    }
  }
}

uint8_t ofApp::readChar()
{
 // Blocks until another byte is available on serial port
  while (serial.available() < 1) { } // Block
  return serial.readByte();
}


void ofApp::updateCubes(){
    positionCubes();
}

void ofApp::positionCubes()
{
    int margin = 50;
    int width = ofGetWidth();
    width -= 2*margin;
    int interval = width / (nrCubes/nrOfCubeRows);
    int startPosX = margin + interval/4;
    int startPosY = margin;

    for(int i = 0; i < nrCubes; i++){
        cubes[i].rectangle.set(startPosX+((i%nrOfCubeRows)*interval), startPosY+(i/nrOfCubeRows)*interval, interval/2, interval/2);
        //cubes[i].draw();
    }
}

void ofApp::createBride(int from, int to)
{
    seq->stop();
    bridges.push_back(CopyBridge(&cubes[from], &cubes[to]));
    bridges.back().setup();
//    ofAddListener(CopyBridge::copyingDone, this, &ofApp::onCopyingDone);
//    cubes[from].rectangle.getCenter();
//    cubes[from].copying = true;
//    cubes[from].copyingTo = to;
}

void ofApp::drawCopying()
{
    for(int i = 0; i < bridges.size(); i++){
        bridges[i].draw();
    }
}


void ofApp::drawCubes(){
    for(int i = 0; i < nrCubes; i++){
        if(cubes[i].recordRequested){
            ofSetColor(255,255,0);
        }else if(cubes[i].recording){
            ofSetColor(255,0,0);
        }else if(cubes[i].active){
            ofSetColor(cubes[i].color);
        }else{
            ofSetColor(240, 240, 240);
        }
        ofRect(cubes[i].rectangle);
    }
}

