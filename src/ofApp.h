#pragma once

#include "ofMain.h"
#include "ofxUST.h"
#include "ofxGui.h"
#include "ofxOsc.h"


enum OscSendingType
{
  SENDTYPE_POINT,
  SENDTYPE_POUINTS_STRING,
  SENDTYPE_CLUSTERED_POINT,
  SENDTYPE_CLUSTERED_POINTS_STRING,
  SENDTYPE_SIZE
};

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
  ofParameterGroup     sensorParams;
  ofParameter< int >   direction;
  void                 directionChanged( int& _direction );
  ofParameter< bool >  bMirror;
  void                 mirrorChanged( bool& _b );
  ofParameter< int >   step;
  void                 stepChanged( int& _v );
  ofParameter< int >   distToScreen;
  ofParameter< int >   mmW, mmH;
  ofParameter< int >   pxW, pxH;
  ofParameter< int >   clusterThresholdNum;
  
  ofRectangle            screenArea;
  std::vector< ofVec2f > mmTouchPoints;
  std::vector< ofVec2f > touchPoints;
  std::vector< ofVec2f > mmClusterPoints;
  std::vector< ofVec2f > clusterPoints;
  std::vector< float >   clusterRadius;
  float                  drawingScale;
  
  // osc
  ofxOscSender               oscSender;
  void                       sendPointsAsString( std::vector< ofVec2f >* _points );
  void                       sendPoints( std::vector< ofVec2f >* _points );
  ofParameterGroup           oscParams;
  ofParameter< std::string > oscHost;
  ofParameter< int >         oscPort;
  ofParameter< std::string > oscAddress;
  ofParameter< int >         sendingType;
  std::deque< std::string >  messageHistory;
  
  // gui
  ofxPanel             gui;
  ofParameterGroup     params;
  ofParameter< float > fps;
  ofFile               settingFile;
  void                 setupGui();
  bool                 bDrawGui;
};
