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

	//-->Effect Modifier
    string deviceDesc;

    private:
	//-->Effect Stuff
	float timer;
	float startTime;
	int cyclePosition = 0;
	int cycleLimiter;
	int triggerTime;


    void EffectSolid(int& _xpos, int& _lenght);
    void RunningUp_Down(int& _xpos, int& _lenght, int effectS, int effectI, ofColor& _col);
    void EffectMathDown(int& _xpos, int& _lenght, int effectS, int effectI, ofColor& _col);
    void EffectFlash(int& _xpos, int& _lenght, int effectS, int effectI);
	void EffectFlashInverted(int& _xpos, int& _lenght, int effectS, int effectI,ofColor& _col);
    void EffectRandomSparkle(int& _xpos, int& _lenght, int effectS, int effectI);
    void EffectBlackout(ofColor& _col);

    void EffectSimpleSine(int& _xpos, int& _lenght, int effectS, int effectI, ofColor& _col);

    int CreateTimer(int triggerTime,int cycleLimiter,bool consoleTimer);
    int CreateSineWave(int sMin,int sinMax,bool consoleTimer);
    int CreateFMOD(int max);
    int fm;
    int sw;
	int artnetPerlinNoise(int max);


};

#endif // ARTNETSENDER_H
