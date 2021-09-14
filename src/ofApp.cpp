/*
WINDOWS BRANCH 
TODO !!!
+ remove array after testing
+ the lenght is given twice to the class
*/
#include "ofApp.h"
string tempLastMsg = "";
//GENERAL
#define GLOBALFRAMERATE 60
#define BRANCH_TYPE "WINDOWS"
#define VERSION_NUMBER "0.0.4"
#define ENABLE_VSYNC TRUE
#define TTF_FONT TRUE

//OSC
#define OSC_IN_PORT 6666
#define OSC_OUT_PORT 6667
#define OSC_IP "localhost"


vector<ArtnetSender> newSG;



//--------------------------------------------------------------
void ofApp::setup(){


ofSetLogLevel(OF_LOG_VERBOSE);
ofSetLogLevel(OF_LOG_FATAL_ERROR);
ofLogToConsole();

const string csvPath = "nodes.csv";

if(csv.load(ofToDataPath(csvPath)))
{
foundNodes=true;
csvRows=csv.getNumRows();
newSG.resize(csvRows);
ofLog(OF_LOG_NOTICE, "CSV-File found with "+ofToString(csvRows)+"nodes!");

for(int i = 0; i < csvRows; i++) 
{
	
		   _name.push_back(csv[i][0]);
		   _ip.push_back(csv[i][1]);
		   _length.push_back(ofToInt((csv[i][2])));
}

}
 else
{
	ofLog(OF_LOG_FATAL_ERROR, "File not found ");
}

	ofSetFrameRate(GLOBALFRAMERATE);
	ofSetVerticalSync(ENABLE_VSYNC);
	ofEnableAntiAliasing();
	
	ofBackground(ofColor::black);   
		
	//do not change the name, its a part of the osc message
	parameters.setName("parameters");

	mainControls.setHeaderBackgroundColor(ofColor::black);	//
	

	//Experimental:: choose the begin and end of your universe 

	/*parameters.add(parLenStart);
	parLenStart.setName("Start/End Channel");
	parameters.add(channelStart.set("channelStart",1,0,60));
	channelStart.setParent(parLenStart);
	parameters.add(channelLength.set("channelLength",60,0,100));
	channelLength.setParent(parLenStart);
	*/


	parameters.add(parColors);	
	colorR.setParent(parColors);
	parColors.setName("Color Control");
	parameters.add(colorR.set("colorR", 255, 0, 255));

	parameters.add(colorG.set("colorG",0,0,255));
	colorG.setParent(parColors);
	parameters.add(colorB.set("colorB",0,0,255));
	colorB.setParent(parColors);
	parameters.add(colorA.set("colorA",255,0,255));
	colorA.setParent(parColors);

	//alternative color controll, no osc support yet
	//parameters.add(color.set("color", ofColor(100, 100, 140), ofColor(0, 0), ofColor(255, 255)));

	parameters.add(parEffects);
	parEffects.setName("Effect Controls");
	parameters.add(effectIn.set("effectIn", 255, 0, 255));
	effectIn.setParent(parEffects);
	parameters.add(effectSpeed.set("effectSpeed", 255, 0, 255));
	effectSpeed.setParent(parEffects);
	parameters.add(effectIndex.set("effectIndex", 0, 0, effectPresetsLenght-1));
	effectIndex.setParent(parEffects);
	
	parameters.add(effectDesc.set("effectDesc"));
	effectDesc.setName("effectDesc");
	effectIndex.addListener(this, &ofApp::effectIndexChanged);
	effectIndexChanged(currEffectIndex);
	
	font.load(OF_TTF_SANS, 9, true, true);
	mainControls.setUseTTF(TTF_FONT);
	mainControls.setTextColor(ofColor::white);
	
	mainControls.setup(parameters);
	
	// by now needs to pass the gui parameter groups since the panel internally creates it's own group
	//local inport local outport
	
	sync.setup((ofParameterGroup&)mainControls.getParameter(), OSC_IN_PORT, OSC_IP, OSC_OUT_PORT);

	if (foundNodes)
	{
	for (int i=0;i<csvRows;i++)
	{	
		newSG[i].artnetSetup(_ip[i], GLOBALFRAMERATE, _length[i], _name[i]);	  
	}
	}

	
}

//--------------------------------------------------------------
void ofApp::update(){

	sync.update();
	//alternative color controll, no osc support yet
	//currCol = color;
	
	//casting of color :: optional, but does improves speed, adds smoothing
	currCol.r = (int)colorR;
	currCol.g = (int)colorG;
	currCol.b = (int)colorB;
	currCol.a = (int)colorA;

		if (foundNodes)
		{
		for(int i=0; i<csvRows; i++)
		{
			newSG[i].artnetUpdate(currCol, 0, _length[0], effectSpeed, effectIn, effectIndex);			
		}
		}
	
}

//--------------------------------------------------------------
void ofApp::draw(){

	mainControls.draw();

	if (foundNodes)
	{
	for (int i=0;i<csvRows;i++)
	{
		newSG[i].artnetDraw(300 + i * 100, 50);		
	}
	}
	else
	{
		ofPushStyle();
		ofSetColor(ofColor::red);
		ofDrawBitmapString("No Entry found! Check Csv-File", ofGetWidth()/2, ofGetHeight()/2);
		ofPopStyle();
	}
	//ofSetColor(240, 240, 240);
	showHints();
	showSessionInfo();
}


void ofApp::effectIndexChanged(float&index)
{	
	unsigned int tempIndex = roundf(index);

	if (index <= effectPresetsLenght-1)
	{
		effectIndex.set(index);
		effectDesc.set(effectPresetsNames[tempIndex]);
	}
	else 
	{
		index = 0;	
		effectIndex.set(index);
	}	
}

void ofApp::showHints()
{	
	ofPushStyle();
	ofSetColor(ofColor::green);
		int padBottom = 20;
		ofDrawBitmapString("Show Session Information :: i ", 600, 620);
		ofDrawBitmapString("Save Presets (.xml) file :: s ", 600, 640);
		ofDrawBitmapString("Load Presets (.xml) file :: o ", 600, 660);
		ofDrawBitmapString("Toogle between Effects :: <- and ->", 600, 680);
		ofDrawBitmapString("PANIC BUTTON :: ESC ", 600, 700);
		ofPopStyle();
}


bool ofApp::checkFilePropOpen(ofFileDialogResult f)
{
	bool checkP = false;

	ofFile file(f.getPath());	
	string fExt = ofToUpper(file.getExtension());
	if (file.exists() && fExt == "XML")	
	{
		checkP = true;
	}
	return checkP;
}

bool ofApp::checkFilePropSave(ofFileDialogResult f)
{
	bool checkP = false;
	if (!file.exists())
	{
		checkP = true;
	}
	return checkP;
}

void ofApp::showSessionInfo()
{
	if (showSessionInfoTrigger == 1)
	{
		ofPushStyle();
		ofSetColor(ofColor::red);
		ofDrawBitmapString("BranchType: " BRANCH_TYPE, 100, 600);
		ofDrawBitmapString("Version: " VERSION_NUMBER, 100, 620);
		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

	if (key == OF_KEY_RIGHT) 
	{
		currEffectIndex = currEffectIndex + 1;
		effectIndexChanged(currEffectIndex);
	}

	if (key == OF_KEY_LEFT)
	{
		if (currEffectIndex != 0)
		{	currEffectIndex = currEffectIndex - 1;
			effectIndexChanged(currEffectIndex);
		}
		else
		{
			currEffectIndex = effectPresetsLenght;
		}
	}

	if (key == 'i')
	{

		if (showSessionInfoTrigger == 1)
		{
			showSessionInfoTrigger = 0;
		}
		else
		{
			showSessionInfoTrigger = 1;
		}
	}


	
	if (key == 's')
	{
		string tmpFN;
		std::string h = "save.xml";
		fDSavePreset = ofSystemSaveDialog(h, h);
		if (checkFilePropSave(fDSavePreset))
		{
			tmpFN = fDSavePreset.getName();		
			mainControls.saveToFile(tmpFN);
		}
	
	}

	if (key == 'o')
	{
		fDLoadPreset = ofSystemLoadDialog("Select .xml file ");
	if (fDLoadPreset.bSuccess && checkFilePropOpen(fDLoadPreset))
	{
		mainControls.loadFromFile(fDLoadPreset.getName());
	}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

void ofApp::exit()
{
	mainControls.clear();
}


//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
