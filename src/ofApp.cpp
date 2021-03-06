/*
WINDOWS BRANCH 
*/
#include "ofApp.h"
//GENERAL
#define GLOBALFRAMERATE 40
#define BRANCH_TYPE "WINDOWS"
#define PROJECT_NAME "WELOSA"
#define VERSION_NUMBER "0.0.6"
#define ENABLE_VSYNC TRUE
#define TTF_FONT TRUE
//OSC
#define OSC_IN_PORT 6666
#define OSC_OUT_PORT 6667
#define OSC_IP "localhost"

//SLIDER --> 4 sliders with r,g,b,alpha, has OSC SUPPORT !
//#define COLOR_CONTROLL_SLIDER 
//PICKER --> Color Picker, has no OSC SUPPORT
#define COLOR_CONTROLL_PICKER


//--------------------------------------------------------------
void ofApp::setup(){

scanXMLPresets();
ofSetLogLevel(OF_LOG_NOTICE);
csvScanCSV();


	ofSetFrameRate(GLOBALFRAMERATE);
	ofSetVerticalSync(ENABLE_VSYNC);
	ofEnableAntiAliasing();	
	ofBackground(ofColor::black);   		
	//do not change the name, its a part of the osc message
	parameters.setName("parameters");
	mainControls.setHeaderBackgroundColor(ofColor::black);	
	//Experimental:: choose the begin and end of your universe 

	/*parameters.add(parLenStart);
	parLenStart.setName("Start/End Channel");
	parameters.add(channelStart.set("channelStart",1,0,60));
	channelStart.setParent(parLenStart);
	parameters.add(channelLength.set("channelLength",60,0,100));
	channelLength.setParent(parLenStart);
	*/



#if defined(COLOR_CONTROLL_SLIDER)
	parameters.add(parColors);
	parColors.setName("Color Control");
	colorR.setParent(parColors);
	parameters.add(colorR.set("colorR", 255, 0, 255));
	parameters.add(colorG.set("colorG",0,0,255));
	colorG.setParent(parColors);
	parameters.add(colorB.set("colorB",0,0,255));
	colorB.setParent(parColors);
	parameters.add(colorA.set("colorA",255,0,255));
	colorA.setParent(parColors);
#elif defined (COLOR_CONTROLL_PICKER)
	parameters.add(colorPicker.set("color", ofColor(100, 100, 140), ofColor(0, 0), ofColor(255, 255)));
	colorPicker.setName("Color Picker");
		
#endif

	parameters.add(parEffects);
	parEffects.setName("Effect Controls");
	parameters.add(effectIn.set("effectIn", 10, 1, 255));
	effectIn.setParent(parEffects);
	parameters.add(effectSpeed.set("effectSpeed", 255, 1, 255));
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
	mainControls.setDefaultHeight(20);
	mainControls.setBorderColor(50);
	mainControls.setFillColor(ofColor(24));
	mainControls.setTextColor(ofColor::antiqueWhite);
	mainControls.setHeaderBackgroundColor(ofColor::blueSteel);
	mainControls.maximizeAll();
	mainControls.setup(parameters);
	
	// by now needs to pass the gui parameter groups since the panel internally creates it's own group
	//local inport local outport
	
	sync.setup((ofParameterGroup&)mainControls.getParameter(), OSC_IN_PORT, OSC_IP, OSC_OUT_PORT);

	if (foundNodes)
	{
	for (int i=0;i<csvRows;i++)
	{	
		artnetNodes[i].artnetSetup(nodesIP[i], GLOBALFRAMERATE, nodesMaxLen[i], nodesDesc[i]);	  
	}
	}
}

//--------------------------------------------------------------
void ofApp::update(){


	ofSetWindowTitle(PROJECT_NAME " " "FPS:"+getFrameRate());
	sync.update();

	//casting of color :: optional, but does improves speed, adds smoothing
#if defined(COLOR_CONTROLL_SLIDER)
	currCol.r = (int)colorR;
	currCol.g = (int)colorG;
	currCol.b = (int)colorB;
	currCol.a = (int)colorA;

#elif defined (COLOR_CONTROLL_PICKER)
	currCol = colorPicker;
#endif

		if (foundNodes)
		{
		for(int i=0; i<csvRows; i++)
		{
			artnetNodes[i].artnetUpdate(currCol, 0, nodesMaxLen[i], effectSpeed, effectIn, effectIndex);			
		}
		}	
}

//--------------------------------------------------------------
void ofApp::draw(){

	
	XMLPresetsDraw();
	mainControls.draw();

	if (foundNodes)
	{
	for (int i=0;i<csvRows;i++)
	{
		artnetNodes[i].artnetDraw(300 + i * 200, 50);		
	}
	}
	else
	{
		ofPushStyle();
		ofSetColor(ofColor::red);
		ofDrawBitmapString("No Entry found! Check Csv-File", ofGetWidth()/2, ofGetHeight()/2);
		ofPopStyle();
	}	
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

void ofApp::csvScanCSV()
{
	if (csv.load(ofToDataPath(csvPath)))
	{
		
		if (csv.getNumRows() != 0)
		{
			foundNodes = true;
			csvRows = csv.getNumRows();
			artnetNodes.resize(csvRows);

			for (int i = 0; i < csv.getNumRows(); i++)
			{
				nodesDesc.push_back(csv[i][0]);
				nodesIP.push_back(csv[i][1]);
				nodesMaxLen.push_back(ofToInt((csv[i][2])));
			}
		}
		else
		{
			foundNodes = false;
		}
	}
	
}

void ofApp::showHints()
{	
	ofPushStyle();
	ofSetColor(ofColor::green);		
	int startY = 500;
		ofDrawBitmapString("Show Session Information :: i ", 10, startY);
		ofDrawBitmapString("Save Presets (.xml) file :: s ", 10, startY +20);
		ofDrawBitmapString("Load Presets (.xml) file :: o ", 10, startY+40);
		ofDrawBitmapString("Toogle between Effects :: <- and ->", 10, startY+60);
		ofDrawBitmapString("PANIC BUTTON :: ESC ", 10, startY+80);
		ofPopStyle();
}


bool ofApp::checkFilePropOpen(ofFileDialogResult f)
{
	bool fileCheck = false;
	ofFile file(f.getPath());	
	string fileExt = ofToUpper(file.getExtension());
	if (file.exists() && fileExt == "XML")	
	{
		fileCheck = true;
	}
	return fileCheck;
}

bool ofApp::checkFilePropSave(ofFileDialogResult f)
{
	bool fileCheck = false;
	if (!file.exists())
	{
		fileCheck = true;
	}
	return fileCheck;
}

string ofApp::getFrameRate()
{
	std::stringstream fr;
	fr << ofGetFrameRate();
	return fr.str();
}

void ofApp::scanXMLPresets()
{
	xmlPresetList.clear();
	ofDirectory ndir;
	ndir.allowExt("xml");
	ndir.listDir("");

	if (ndir.size() != 0)
	{
		foundXMLFiles = true;
		for (int i = 0; i < ndir.size(); i++) {
			//second stament doenst work
			if ((ndir.getName(i) != "settings.xml")|| (ndir.getName(i) != "analyzer.xml"))
			{
				xmlPresetList.push_back(ndir.getPath(i));				
			}
		}
	}
}

void ofApp::XMLPresetsDraw()
{	

	if (foundXMLFiles)
	{
		ofPushStyle();
		ofSetColor(ofColor::blue);
		string curPresDesc = "Curr Preset : " + xmlPresetList[currentXMLPreset];
		int lastyPos=0;
		int counter;
		
			ofDrawBitmapStringHighlight("Xml-Presets", ofGetWidth() - 150, 80, ofColor::blue, ofColor::wheat);
		for (int i = 0; i < xmlPresetList.size(); i++)
		{
			counter = i + 1;
			ofDrawBitmapString(ofToString(counter) + " : " + xmlPresetList[i], ofGetWidth()-150, 100 + i * 30);
			lastyPos = 100 + i * 30;
		}

		if (isXMLSelected)
		{
			ofDrawBitmapStringHighlight(curPresDesc, ofGetWidth() - 150, lastyPos+30, ofColor::red, ofColor::wheat);
		}

		ofPopStyle();
	}

}

void ofApp::setXMLPreset(int key)
{	
	isXMLSelected = true;
	if (key < xmlPresetList.size())
	{
		mainControls.loadFromFile(xmlPresetList[key]);
		currentXMLPreset = key;
	}
}

void ofApp::showSessionInfo()
{
	if (showSessionInfoTrigger == 1)
	{
		ofPushStyle();
		ofSetColor(ofColor::red);
		ofDrawBitmapString(PROJECT_NAME "BranchType: " BRANCH_TYPE, 100, 600);
		ofDrawBitmapString("Version: " VERSION_NUMBER, 100, 620);
		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

	if (key == OF_KEY_RIGHT) 
	{
		currEffectIndex++;
		effectIndexChanged(currEffectIndex);
		isXMLSelected = false;
		
	}

	if (key == OF_KEY_LEFT)
	{
		if (currEffectIndex != 0)
		{	currEffectIndex --;
			effectIndexChanged(currEffectIndex);
		}
		else
		{
			currEffectIndex = effectPresetsLenght;
		}
		isXMLSelected = false;	
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
		
		fDSavePreset = ofSystemSaveDialog("test.xml", "Vice versa");
		if (checkFilePropSave(fDSavePreset))
		{		
			mainControls.saveToFile(fDSavePreset.getName());
			scanXMLPresets();
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


	if (key == '1')
	{
		setXMLPreset(0);
		
	}
	if (key == '2')
	{
		setXMLPreset(1);
	}

	if (key == '3')
	{
		setXMLPreset(2);
	}

	if (key == '4')
	{
		setXMLPreset(3);
	}

	if (key == '5')
	{
		setXMLPreset(4);
	}
	if (key == '6')
	{
		setXMLPreset(5);
	}
	if (key == '7')
	{
		setXMLPreset(6);
	}
	if (key == '8')
	{
		setXMLPreset(7);
	}

	if (key == '9')
	{
		setXMLPreset(8);
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
	for (int i = 0; i < csvRows; i++)
	{
		artnetNodes[i].artnetClearBuffer();
	}
	mainControls.clear();
}


//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
