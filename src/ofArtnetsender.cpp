#include "ofArtnetsender.h"


ArtnetSender::ArtnetSender()
{

}

void ArtnetSender::artnetSetup(string _ip, short _fps,short _totalChannels,string _name)
{
startTime = ofGetElapsedTimeMillis();;
cyclePosition = 0;
  sendData.allocate(170, 1, GL_RGB);
  //standart artnet port, please refer to readme
  artnet.setup(_ip,6454);
  deviceIP=_ip;  
  deviceDesc=_name;
  totalChannels=_totalChannels;
  artnet.setFps(_fps);
}

void ArtnetSender::artnetUpdate(ofColor _color, int _pos, int length, int _effectS, int _effectI, int effectIndex)
{
    universePos=_pos;
    universeLenght= totalChannels;
    universeColor=_color;

	switch (effectIndex) {
	case 0:
		artnetEffectSolid(universePos, universeLenght);
		break;
	case 1:
		artnetEffectRunningUp_Down(universePos, universeLenght,_effectS,_effectI, universeColor);
		break;

	case 2:
		artnetEffectMathDown(universePos, universeLenght, _effectS, _effectI,universeColor);
		break;

	case 3:
		artnetEffectFlash(universePos, universeLenght, _effectS, _effectI);
		break;

	case 4:
		artnetEffectRandomSparkle(universePos, universeLenght, _effectS, _effectI);
		break;

	case 5:
		artnetEffectSimpleSine(universePos, universeLenght, _effectS, _effectI,universeColor);
		break;

	case 6:
		artnetEffectFlashInverted(universePos, universeLenght, _effectS, _effectI, universeColor);
		break;
	default:
		artnetEffectSolid(universePos, universeLenght);
	}

    sendData.begin();
        ofClear(0);
        ofSetColor(universeColor);
        {
        ofDrawRectangle(universePos, 0, universeLenght, 1);
        }
    sendData.end();
    sendData.readToPixels(payloadData);
    artnet.sendArtnet(payloadData);
    ofPopStyle();
}

void ArtnetSender::artnetDraw(int _xpos, int _ypos)

{
	ofPushStyle();
	int paddingBottom = 20;
    ofDrawBitmapString(deviceDesc,_xpos, paddingBottom);
	paddingBottom += 20;
	ofDrawBitmapString("Ip: " + ofToString(deviceIP), _xpos, paddingBottom);
	paddingBottom += 20;
	ofDrawBitmapString("Total Lenght: " + ofToString(totalChannels), _xpos, paddingBottom);
	paddingBottom += 20;
	ofDrawBitmapString("Cur. Position: " + ofToString(universePos), _xpos, paddingBottom);
	paddingBottom += 20;
	ofDrawBitmapString("cur. Length: " + ofToString(universeLenght), _xpos, paddingBottom);
	paddingBottom += 20;
	ofPushMatrix();
	ofTranslate(_xpos+80, paddingBottom, 0);
	ofRotateZDeg(90);  
    ofSetColor(universeColor);		
	ofRectangle rRect= ofRectangle(0, 0, 100, 80);
	ofScale(10, 1, 1.0F);
	sendData.draw(rRect);	
	ofPopMatrix();
	ofPopStyle();

}

void ArtnetSender::artnetEffectSolid(int & _xpos, int & _lenght)
{
	
}



void ArtnetSender::artnetEffectRunningUp_Down(int & _xpos, int & _lenght, int _effectS, int _effectI,ofColor& _col)
{
	cycleLimiter = _lenght;
	int tTriggerT = _effectS * 0.1f;
	_xpos = artnetTimer(tTriggerT, _lenght, true);
	_lenght = _effectI / round(_effectI/8);	

}

void ArtnetSender::artnetEffectMathDown(int & _xpos, int & _lenght, int _effectS, int effectI, ofColor& col)
{
	
		int tLength = _lenght;
		int cuLen;
		int tPos;
		
		int t = artnetTimer(_effectS / 2, 5, true);
		//int t = (_effectS / 255) * 100;

		switch (t)
		{
		case 1:
			col = ofColor(ofColor::coral);
			tPos = round(tLength*0.8);
			cuLen = round(tLength*0.2);
			break;
		case 2:
			col = ofColor(ofColor::aliceBlue);
			tPos = round(tLength*0.6);
			cuLen = round(tLength*0.2);
			break;

		case 3:
			col = ofColor(ofColor::beige);
			tPos = round(tLength*0.4);
			cuLen = round(tLength*0.2);
			break;

		case 4:
			col = ofColor(ofColor::darkOrchid);
			tPos = 1;
			cuLen = tLength - tPos;
					break;
		
		default:
			col = ofColor(ofColor::black);
			tPos = 0;
			cuLen = 0;
			break;
		}

		_xpos = tPos;
		_lenght = cuLen;
}

void ArtnetSender::artnetEffectFlash(int & _xpos, int & _lenght, int effectS, int effectI)
{
	int cuLen = 0;
	cycleLimiter = round(effectI/10);
	
	triggerTime = ofMap(effectS, 0, 255, 0, 255);

	

	if (cyclePosition <= cycleLimiter) {
		timer = ofGetElapsedTimeMillis() - startTime;
		if (timer >= triggerTime) {
			cyclePosition++;
			startTime = ofGetElapsedTimeMillis();
			cuLen = _lenght;
		}
	}
	else
	{
		cyclePosition = 0;
		cuLen = 0;
	}

	_lenght = cuLen;
	_xpos = 0;
}


void ArtnetSender::artnetEffectFlashInverted(int & _xpos, int & _lenght, int effectS, int effectI, ofColor & _col)
{
	int cuLen = 0;
	cycleLimiter = round(effectI / 10);

	triggerTime = ofMap(effectS, 0, 255, 0, 255);
	ofColor c = _col;

	if (cyclePosition <= cycleLimiter) {
		timer = ofGetElapsedTimeMillis() - startTime;
		if (timer >= triggerTime) {
			cyclePosition++;
			startTime = ofGetElapsedTimeMillis();
			cuLen = _lenght;
			_col = c;
		}
	}
	else
	{
		cyclePosition = 0;
		cuLen = _lenght;
		_col = c.invert();
	}

	_lenght = cuLen;
	_xpos = 0;
}

void ArtnetSender::artnetEffectRandomSparkle(int & _xpos, int & _lenght, int effectS, int effectI)
{
		
		int rand = (int)(ofRandom(_lenght)); 
		if (artnetTimer(round(effectS*3), 10, true) % 2 == 0)
		{
			_lenght = ofMap(effectI, 1, 255, 1, totalChannels);
			_xpos = rand;
		}
		else
		{
			_lenght = 0;
			_xpos = 0;
		}

}

void ArtnetSender::artnetEffectBlackout(ofColor & col)
{
	col = ofColor(ofColor::black);
}

void ArtnetSender::artnetEffectSimpleSine(int& _xpos, int& _lenght, int effectS, int effectI, ofColor& _col)
{
	//int len = ofMap(effectI, 1, 255, 1, totalChannels);
	ofColor colr = _col;
	_lenght = totalChannels;
	_xpos = 0;

	//_xpos = artnetPerlinNoise(_lenght);
	
	_col.r = artnetSineW(0, colr.r,true);
	_col.g = artnetSineW(0, colr.g, true);
	_col.b = artnetSineW(0, colr.b, true);
}

int ArtnetSender::artnetTimer(int _triggerTime,int limit,bool consoleTimer)

{
	if (cyclePosition < limit) {
		timer = ofGetElapsedTimeMillis() - startTime;
		if (timer >= _triggerTime) {
			cyclePosition++;
			if (consoleTimer)
			cout << cyclePosition << endl;
			startTime = ofGetElapsedTimeMillis();
		}
	}
	else
	{
		cyclePosition = 0;
	}

	return cyclePosition;
	
}

int ArtnetSender::artnetSineW(int min,int max, bool k)
{
	sw = ofMap(sin(ofGetElapsedTimef()), -1, 1, (float)min, (float)max,false);
	if (k)
	cout << sw << endl;
	return sw;
}

int ArtnetSender::artnetFMOD(int max)
{
	fm = fmod(ofGetElapsedTimef(), max);
	cout << fm << endl;
	return fm;
}

int ArtnetSender::artnetPerlinNoise(int max)
{
	
	float x=ofNoise(ofRandom(1));
	 float calc = max *x;
	 int s = calc;
	cout << calc << endl;
	return calc;
}



