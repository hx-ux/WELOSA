#include "ofArtnetsender.h"


ArtnetSender::ArtnetSender()
{

}

void ArtnetSender::artnetSetup(string _ip, short _fps,int _totalChannels,string _name)
{
  startTime = ofGetElapsedTimeMillis();;
  cyclePosition = 0;
  //allocate only, what u need (_totalChannles)
  sendData.allocate(_totalChannels, 1, GL_RGB);
  //standart artnet port, please refer to readme
  artnet.setup(_ip,6454);  
  deviceIP=_ip;  
  deviceDesc=_name;
  totalChannels=_totalChannels;
  artnet.setFps(_fps);
}

void ArtnetSender::artnetUpdate(ofColor _color, int _pos, int length, int _effectS, int _effectI, int effectIndex)
{

	tempIndex = effectIndex;

	if (effectIndex != tempIndex)
	{
		artnetClearBuffer();
		SendThreadSleep();
		tempIndex = effectIndex;
	}

    universePos=_pos;
    universeLenght= totalChannels;
    universeColor=_color;
	
	switch (effectIndex) {		
	case 0:
		EffectSolid(universePos, universeLenght);
		break;
	case 1:
		RunningUp_Down(universePos, universeLenght,_effectS,_effectI, universeColor);
		break;

	case 2:
		EffectMathDown(universePos, universeLenght, _effectS, _effectI,universeColor);
		break;

	case 3:
		EffectFlash(universePos, universeLenght, _effectS, _effectI);
		break;

	case 4:
		EffectRandomSparkle(universePos, universeLenght, _effectS, _effectI);
		break;

	case 5:
		EffectSimpleSine(universePos, universeLenght, _effectS, _effectI,universeColor);
		break;

	case 6:
		EffectFlashInverted(universePos, universeLenght, _effectS, _effectI, universeColor);
		break;
	case 7:
		EffectPerlinNoise(_effectS, _effectI);
		break;
	default:
		EffectSolid(universePos, universeLenght);
	}

    sendData.begin();
        ofClear(0);
        ofSetColor(universeColor);
        {
        ofDrawRectangle(universePos, 0, universeLenght, 1);
        }
    sendData.end();
    sendData.readToPixels(payloadData);
	artnet.startThread();
    artnet.sendArtnet(payloadData);	
	artnet.stopThread();
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
	ofScale(10, 1, 1.0F);	
	sendData.draw(ofRectangle(0, 0, totalChannels, 80));
	ofSetColor(ofColor::wheat);
	ofPopMatrix();
	ofPopStyle();
}

void ArtnetSender::artnetClearBuffer()
{
	sendData.clear();	
}

void ArtnetSender::EffectSolid(int & _xpos, int & _lenght)
{
	//do nothing :)
}


void ArtnetSender::RunningUp_Down(int & _xpos, int & _lenght, int _effectS, int _effectI,ofColor& _col)
{
	_xpos = CreateTimer((255 / _effectS)*10, totalChannels, true);
	_lenght = 20; 
}

void ArtnetSender::EffectMathDown(int & _xpos, int & _lenght, int _effectS, int effectI, ofColor& col)
{	
		int tLength = _lenght;
		int cuLen;
		int tPos;

		int t = CreateTimer(_effectS /2, 5, true);
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

void ArtnetSender::EffectFlash(int & _xpos, int & _lenght, int effectS, int effectI)
{

	if (CreateTimer(round(255/effectS)*100, 4, true)  % 2==0)
	{
		universeColor = ofColor::black;
	}
	_lenght = totalChannels;
}


void ArtnetSender::EffectFlashInverted(int & _xpos, int & _lenght, int effectS, int effectI, ofColor & _col)
{
	if (CreateTimer(round(255 / effectS) * 100, 4, true) % 2 == 0)
	{
		universeColor = universeColor.invert();
	}	
	_lenght = totalChannels;

}

void ArtnetSender::EffectRandomSparkle(int & _xpos, int & _lenght, int effectS, int effectI)
{
		int rand = (int)(ofRandom(_lenght)); 
		if (CreateTimer(round(effectS*3), 10, true) % 2 == 0)
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

void ArtnetSender::EffectSimpleSine(int& _xpos, int& _lenght, int effectS, int effectI, ofColor& _col)
{	
	universeColor.r = CreateSineWave(0, _col.r,true);
	universeColor.g = CreateSineWave(0, _col.g, true);
	universeColor.b = CreateSineWave(0, _col.b, true);
	
	_lenght = totalChannels;
	_xpos = 0;
}

void ArtnetSender::EffectPerlinNoise(int effectS, int effectI)
{	
	universeColor.a = (255 * artnetPerlinNoise((float)CreateTimer(1, 100) / 10)) - 100;	
}

void ArtnetSender::SendThreadSleep()
{
	artnet.yield();
}

///<summary>
/// Increment from 1 to {limit} per {triggerTime} in ms 
///</summary>
int ArtnetSender::CreateTimer(int _triggerTime,int limit,bool consoleTimer)

{
	if (cyclePosition < limit) {
		timer = ofGetElapsedTimeMillis() - startTime;
		if (timer >= _triggerTime) {
			cyclePosition++;
			if (consoleTimer)
			{
				cout << cyclePosition << endl;
			}
			startTime = ofGetElapsedTimeMillis();
		}
	}
	else
	{
		cyclePosition = 0;
	}

	return cyclePosition;
	
}

/// <summary>
/// 
/// </summary>
/// <param name="min">min val </param>
/// <param name="max">max val</param>
/// <param name="k">shows values to the console</param>
/// <returns>Sine wave</returns>
int ArtnetSender::CreateSineWave(int min,int max, bool k)
{
	sw = ofMap(sin(ofGetElapsedTimef()), -1, 1, (float)min, (float)max,false);
	if (k)
	{
		cout << sw << endl;
	}
	return sw;
}

/// <summary>
/// 
/// </summary>
/// <param name="max"></param>
/// <returns></returns>
int ArtnetSender::CreateFMOD(int max)
{
	fm = fmod(ofGetElapsedTimef(), max);
	cout << fm << endl;
	return fm;
}
/// <summary>
/// 
/// </summary>
/// <param name="max"></param>
/// <returns></returns>
float ArtnetSender::artnetPerlinNoise(float max)
{
	float x=ofNoise(max);
	cout << x << endl;
	return x;
}



