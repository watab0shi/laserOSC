#pragma once

#include "ofMain.h"
#include "ofxUST.h"
#include "ofxGui.h"
#include "ofxOsc.h"


//--------------------------------------------------------------
class ofApp : public ofBaseApp
{
public:
  void setup();
  void update();
  void draw();
  void keyPressed( int key );
  void exit();
  
  // ust
  ofxUST               ust;
  ofParameter< bool >  bMirror;
  void                 mirrorChanged( bool& _b );
  ofParameter< int >   step;
  void                 stepChanged( int& _v );
  ofParameter< int >   distToScreen;
  ofParameter< int >   mmW, mmH;
  ofParameter< int >   pxW, pxH;
  
  ofRectangle          screenArea;
  vector< ofVec2f >    mmTouchPoints;
  vector< ofVec2f >    touchPoints;
  
  // osc
  string               oscHost;
  int                  oscPort;
  ofxOscSender         oscSender;
  void                 sendPointsAsString();
  void                 sendPoints();
  ofParameter< bool >  bSendAsString;
  
  // gui
  ofxPanel             gui;
  ofParameterGroup     params;
  ofParameter< float > fps;
  ofFile               settingFile;
  void                 setupGui();
};
