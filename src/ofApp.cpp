#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
  setupGui();
  
  ust.setDirection( ofxUST::DIRECTION_DOWN );
  ust.setMirror( bMirror );
  ust.setScanningParameterByAngles( -135, 135, 1 );
  
  if( ust.isConnected() ) ust.startMeasurement();
  else                    ofLog() << "Connection failed!";
  
  oscHost = "localhost";// 127.0.0.1
  oscPort = 9000;
  oscSender.setup( oscHost, oscPort );
  
  ofBackground( 0 );
}

//--------------------------------------------------------------
void ofApp::update()
{
  fps = floor( ofGetFrameRate() * 10 ) / 10;
  
  ust.update();
  
  screenArea.setFromCenter( 0, ( mmH * .5 ) + distToScreen, mmW, mmH );
  
  mmTouchPoints.clear();
  touchPoints.clear();
  
  for( int i = 0; i < ust.coordinates.size(); ++i )
  {
    int mmX = ust.coordinates.at( i ).x;
    int mmY = ust.coordinates.at( i ).y;
    
    if( screenArea.inside( mmX, mmY ) )
    {
      mmTouchPoints.push_back( ofVec2f( mmX, mmY ) );
      
      float x = ofMap( mmX, screenArea.getLeft(), screenArea.getRight(), 0., pxW, true );
      float y = ofMap( mmY, screenArea.getTop(), screenArea.getBottom(), 0., pxH, true );
      
      touchPoints.push_back( ofVec2f( x, y ) );
    }
  }
  
  if( touchPoints.size() > 0 )
  {
    if( bSendAsString ) sendPointsAsString();
    else                sendPoints();
  }
}

//--------------------------------------------------------------
void ofApp::sendPointsAsString()
{
  ofxOscMessage message;
  message.setAddress( "/laserPoints" );
  
  string pointsString = "";
  for( int i = 0; i < touchPoints.size(); ++i )
  {
    if( i > 0 ) pointsString += "/";
    pointsString += ofToString( touchPoints.at( i ).x ) + "," + ofToString( touchPoints.at( i ).y );
  }
  
  message.addStringArg( pointsString );
  oscSender.sendMessage( message );
}

//--------------------------------------------------------------
void ofApp::sendPoints()
{
  for( int i = 0; i < touchPoints.size(); ++i )
  {
    ofxOscMessage message;
    message.setAddress( "/laserPoints" );
    message.addFloatArg( touchPoints.at( i ).x );
    message.addFloatArg( touchPoints.at( i ).y );
    oscSender.sendMessage( message );
  }
}

//--------------------------------------------------------------
void ofApp::draw()
{
  static float s = 0.15;
  ofPushMatrix();
  {
    ofTranslate( ofGetWidth() / 2, 0 );
    ofScale( s, s );
    
    ofSetColor( 120 );
    glBegin( GL_LINES );
    for( int i = 0; i < ust.coordinates.size(); ++i )
    {
      glVertex2f( 0, 0 );
      glVertex2f( ust.coordinates.at( i ).x, ust.coordinates.at( i ).y );
    }
    glEnd();
    
    ofFill();
    ofSetColor( 0, 255, 0 );
    for( int i = 0; i < mmTouchPoints.size(); ++i )
    {
      ofDrawCircle( mmTouchPoints.at( i ).x, mmTouchPoints.at( i ).y, 20 );
    }
    
    ofNoFill();
    ofSetColor( 255 );
    ofDrawRectangle( screenArea );
  }
  ofPopMatrix();
  
  gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed( int key )
{
  if( key == 'm' )
  {
    bMirror = !bMirror;
    ust.setMirror( bMirror );
  }
  
  if( key == OF_KEY_RIGHT ) ust.setDirection( ofxUST::DIRECTION_RIGHT );
  if( key == OF_KEY_DOWN )  ust.setDirection( ofxUST::DIRECTION_DOWN );
  if( key == OF_KEY_LEFT )  ust.setDirection( ofxUST::DIRECTION_LEFT );
  if( key == OF_KEY_UP )    ust.setDirection( ofxUST::DIRECTION_UP );
}

//--------------------------------------------------------------
void ofApp::exit()
{
  gui.saveToFile( settingFile.getAbsolutePath() );
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
  bMirror.addListener( this, &ofApp::mirrorChanged );
  step.addListener( this, &ofApp::stepChanged );
  
  params.setName( "LaserOSC" );
  params.add( fps.set( "FPS", 0, 0, 60 ) );
  params.add( bMirror.set( "Mirror", true ) );
  params.add( step.set( "Step", 1, 1, 10 ) );
  params.add( distToScreen.set( "DistToScreen", 0, 0, 2000 ) );
  params.add( mmW.set( "ScreenW", 4000, 1000, 5000 ) );
  params.add( mmH.set( "ScreenH", 3000, 1000, 5000 ) );
  params.add( pxW.set( "PixelW", 1280, 1024, 1920 ) );
  params.add( pxH.set( "PixelH", 800, 768, 1200 ) );
  params.add( bSendAsString.set( "SendAsString", false ) );
  
  string savePath = "gui/settings.xml";
  gui.setup( params, savePath );
  
  settingFile = ofFile( savePath );
  if( settingFile.exists() ) gui.loadFromFile( savePath );
}

//--------------------------------------------------------------
void ofApp::mirrorChanged( bool& _b )
{
  ust.setMirror( bMirror );
}

//--------------------------------------------------------------
void ofApp::stepChanged( int& _v )
{
  ust.stopMeasurement();
  ust.setScanningParameterBySteps( ust.getMinStep(), ust.getMaxStep(), _v );
  ust.startMeasurement();
}
