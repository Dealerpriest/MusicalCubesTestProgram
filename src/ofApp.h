#pragma once

//class Sequencer;
//class Cube;
//class CopyBridge;
#include "ofxGui.h"
#include "ofMain.h"
#include "Cube.h"
#include "Sequencer.h"
#include "CopyBridge.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void exit();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void onCubeTriggered(vector <int> &args);
		void onCopyingDone(CopyBridgeEventArgs& args);
		void toggleSequencer(bool &value);

		void sendTrigger(uint8_t cube, uint8_t effect);
		void sendCubeOff(uint8_t cube);
		void sendRecordStart(uint8_t cube);
		void sendRecordStop(uint8_t cube);
		void sendRecordTimeout(uint8_t cube);
		void sendCopyFinished(uint8_t from, uint8_t to);

		bool connectToArduino();
		void handleSerial();
		uint8_t readChar();
		void updateCubes();
		void positionCubes();
		void drawCubes();
		void drawCopying();

		void createBride(int from, int to);

        const int baudrate = 9600;
		ofSerial serial;

		static const int nrCubes = 2;
		vector<Cube> cubes;
		vector<CopyBridge> bridges;

		Sequencer* seq;

		ofxPanel gui;
		ofxToggle run;
};
