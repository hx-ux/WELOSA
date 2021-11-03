#ifndef ARTNETSENDER_H
#define ARTNETSENDER_H

#include "ofMain.h"
#include "ofxArtnet.h"

class ArtnetSender
{
public:
    ArtnetSender();
    string deviceIP;
    string devicesubnet;
    short  totalChannels;
    short frameRate;


    ofFbo sendData;
    ofxArtnetSender artnet;
    int universePos;
    int universeLenght;
    ofColor universeColor;
    ofPixels payloadData;

    void artnetSetup(string _ip,short _fps,short _totalChannels,string _name);
    void artnetUpdate(ofColor _color,int _pos,int length, int effectS, int effectI, int effectIndex);
    void artnetDraw(int _xpos, int _ypos );
	void artnetClearBuffer();
	void artnetSenderDestroy();
	//-->Effect Modifier


    string deviceDesc;

    private:
	//-->Effect Stuff
	float timer;
	float startTime;
	int cyclePosition = 0;
	int cycleLimiter;
	int triggerTime;


    void artnetEffectSolid(int& _xpos, int& _lenght);
    void artnetEffectRunningUp_Down(int& _xpos, int& _lenght, int effectS, int effectI, ofColor& _col);
    void artnetEffectMathDown(int& _xpos, int& _lenght, int effectS, int effectI, ofColor& _col);
    void artnetEffectFlash(int& _xpos, int& _lenght, int effectS, int effectI);
	void artnetEffectFlashInverted(int& _xpos, int& _lenght, int effectS, int effectI,ofColor& _col);
    void artnetEffectRandomSparkle(int& _xpos, int& _lenght, int effectS, int effectI);
    void artnetEffectBlackout(ofColor& _col);

    void artnetEffectSimpleSine(int& _xpos, int& _lenght, int effectS, int effectI, ofColor& _col);

    int artnetTimer(int triggerTime,int cycleLimiter,bool consoleTimer);
    int artnetSineW(int sMin,int sinMax,bool consoleTimer);
    int artnetFMOD(int max);
    int fm;
    int sw;
	int artnetPerlinNoise(int max);


};

#endif // ARTNETSENDER_H
