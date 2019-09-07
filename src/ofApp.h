#pragma once

#include "ofMain.h"
#include "ofxUST.h"
#include "ofxGui.h"
#include "ofxOsc.h"


// ofApp
//----------------------------------------
class ofApp : public ofBaseApp
{
public:
  void setup();
  void update();
  void draw();
  void mouseScrolled( int _x, int _y, float _scrollX, float _scrollY );
  void keyPressed( int _key );
  void windowResized( int _w, int _h );
  void exit();
  
  int w, h;

  // ust
  ofxUST               ust;
  ofParameter< int >   direction;
  void                 directionChanged( int& _direction );
  ofParameter< bool >  bMirror;
  void                 mirrorChanged( bool& _b );
  ofParameter< int >   step;
  void                 stepChanged( int& _v );
  ofParameter< int >   distToScreen;
  ofParameter< int >   mmW, mmH;
  ofParameter< int >   pxW, pxH;
  
  ofRectangle            screenArea;
  std::vector< ofVec2f > mmTouchPoints;
  std::vector< ofVec2f > touchPoints;
  float                  drawingScale;
  
  // osc
  string                     oscHost;
  int                        oscPort;
  ofxOscSender               oscSender;
  void                       sendPointsAsString();
  void                       sendPoints();
  ofParameter< bool >        bSendAsString;
  std::deque< std::string >  messageHistory;
  
  // gui
  ofxPanel             gui;
  ofParameterGroup     params;
  ofParameter< float > fps;
  ofFile               settingFile;
  void                 setupGui();
  bool                 bDrawGui;
};
