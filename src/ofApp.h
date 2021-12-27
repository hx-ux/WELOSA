#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxOscParameterSync.h"
#include "ofArtnetsender.h"
#include "ofxArtnet.h"
#include "ofxCsv.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);      
		void exit();
		
		//-> Programm Info
		void showSessionInfo();
		unsigned int showSessionInfoTrigger = 0;
		//<- Programm Info
		
        //->OSC
       ofxOscParameterSync sync;
	   //<-OSC

		//-->Future Realese
	   //ofParameterGroup saveTF;
	   //ofxPanel save;

        //->GUI	

		ofParameterGroup parameters;
        ofParameter <float> colorR;
        ofParameter<float> colorG;
        ofParameter<float> colorB;
        ofParameter<float> colorA;

        ofParameter<int>channelStart;
        ofParameter<int>channelLength;     
	
		ofParameter<int>effectIn;
		ofParameter<int>effectSpeed;
		ofParameter<float>effectIndex;

		ofParameterGroup parLenStart;
		ofParameterGroup parColors;
		ofParameterGroup parEffects;
			
		ofTrueTypeFont font;
		
		ofParameter<string> effectDesc;
		float currEffectIndex = 0;
		void effectIndexChanged(float&);
		//
		static const unsigned int effectPresetsLenght = 8;
		string effectPresetsNames[effectPresetsLenght] = {"Solid","RunUP::DOWN","Unfolding","Flash Black","Random Sparkle","Sinewave","Flash Inverted","Perlin Noise"};
		//
		ofParameter<ofColor> colorPicker;
		ofxPanel mainControls;
		ofxLabel test;
        //<-GUI

        //->Artnet
        std::vector<std::string> nodesDesc;
        std::vector<std::string>  nodesIP;
        vector<int> nodesMaxLen;
		ofColor currCol;
        //<-Artnet

        //Read CSV
		void csvScanCSV();
        bool foundNodes;
		const string csvPath = "nodes.csv";
        ofxCsv csv;
        unsigned int csvRows;
        //Read CSV	

		//->Hints
		void showHints();		
		//<-Hints		

		//-->file mgmt
		ofFileDialogResult fDSavePreset;
		ofFileDialogResult fDLoadPreset;
		ofFile file;
		bool checkFilePropOpen(ofFileDialogResult  f);
		bool checkFilePropSave(ofFileDialogResult  f);

		//-->Framerate
		string getFrameRate();
		//-->show presets saved as .xml
		void scanXMLPresets();
		void XMLPresetsDraw();
		void setXMLPreset(int key);
		int currentXMLPreset;
		bool foundXMLFiles = false;
		bool isXMLSelected = false;

		vector<ArtnetSender> artnetNodes;
		vector<string> xmlPresetList;
	

private:
	
};
