#include "ofApp.h"

namespace
{
  const std::string OSC_ADDRESS = "/laserPoints";
  const std::string OSC_HOST    = "127.0.0.1";
  const int         OSC_PORT    = 8000;

  const int         MAX_NUM_MESSAGE_HISTORY = 30;
}

// setup
//----------------------------------------
void ofApp::setup()
{
  w = ofGetWidth();
  h = ofGetHeight();
  drawingScale = 0.15;

  setupGui();
  
  ust.open();
  
  if( ust.isConnected() )
  {
    ust.setScanningParameterByAngles( -90, 90, 1 );
    ust.startMeasurement();
  }
  
  oscHost = "localhost";// 127.0.0.1
  oscPort = 9000;
  oscSender.setup( oscHost, oscPort );

  //ofSetFrameRate( 40 );
  
  ofBackground( 0 );
}

// update
//----------------------------------------
void ofApp::update()
{
  fps = floor( ofGetFrameRate() * 10 ) / 10;
  
  ust.update();
  
  ofVec2f pos;
  ofxUST::Direction d = ust.getDirection();
  if( d == ofxUST::DIRECTION_DOWN  ) pos.y = ( ( float )mmH / 2 ) + distToScreen;
  if( d == ofxUST::DIRECTION_LEFT  ) pos.x = -( ( float )mmW / 2 ) - distToScreen;
  if( d == ofxUST::DIRECTION_UP    ) pos.y = -( ( float )mmH / 2 ) - distToScreen;
  if( d == ofxUST::DIRECTION_RIGHT ) pos.x = ( ( float )mmW / 2 ) + distToScreen;
  screenArea.setFromCenter( pos, mmW, mmH );
  
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

// sendPointsAsString
//----------------------------------------
void ofApp::sendPointsAsString()
{
  ofxOscMessage message;
  message.setAddress( OSC_ADDRESS );
  
  std::string pointsString = "";
  for( int i = 0; i < touchPoints.size(); ++i )
  {
    if( i > 0 ) pointsString += "/";
    pointsString += ofToString( touchPoints.at( i ).x ) + "," + ofToString( touchPoints.at( i ).y );
  }
  
  message.addStringArg( pointsString );
  oscSender.sendMessage( message );

  messageHistory.push_back( OSC_ADDRESS + " " + pointsString );
  if( messageHistory.size() > MAX_NUM_MESSAGE_HISTORY )
  {
    messageHistory.pop_front();
  }
}

// sendPoints
//----------------------------------------
void ofApp::sendPoints()
{
  for( int i = 0; i < touchPoints.size(); ++i )
  {
    float x = touchPoints.at( i ).x;
    float y = touchPoints.at( i ).y;

    ofxOscMessage message;
    message.setAddress( OSC_ADDRESS );
    message.addFloatArg( x );
    message.addFloatArg( y );
    oscSender.sendMessage( message );

    messageHistory.push_back( OSC_ADDRESS + " " + ofToString( x ) + " " + ofToString( y ) );
    if( messageHistory.size() > MAX_NUM_MESSAGE_HISTORY )
    {
      messageHistory.pop_front();
    }
  }
}

// draw
//----------------------------------------
void ofApp::draw()
{
  ofPushMatrix();
  {
	ofxUST::Direction d = ust.getDirection();
	if( d == ofxUST::DIRECTION_DOWN  ) ofTranslate( w / 2, 0 );
	if( d == ofxUST::DIRECTION_LEFT  ) ofTranslate( w, h / 2 );
	if( d == ofxUST::DIRECTION_UP    ) ofTranslate( w / 2, h );
	if( d == ofxUST::DIRECTION_RIGHT ) ofTranslate( 0, h / 2 );

    ofScale( drawingScale, drawingScale );
    
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
  
  if( bDrawGui )
  {
    int i = 0;
    int y = h - 20;
    for( auto& m : messageHistory )
    {
      ofDrawBitmapString( ofToString( i, 2, '0' ) + " : " + m, 20, y );
      y -= 20;
      ++i;
    }

    gui.draw();
  }
}

// mouseScrolled
//----------------------------------------
void ofApp::mouseScrolled( int _x, int _y, float _scrollX, float _scrollY )
{
  drawingScale = ofClamp( drawingScale - _scrollY * 0.01, 0.01, 1.0 );
}

// keyPressed
//----------------------------------------
void ofApp::keyPressed( int _key )
{
  if( _key == ' ' ) bDrawGui = !bDrawGui;

  if( _key == 'm' )
  {
    bMirror = !bMirror;
  }
  
  if( _key == OF_KEY_RIGHT ) direction = ( int )ofxUST::DIRECTION_RIGHT;
  if( _key == OF_KEY_DOWN )  direction = ( int )ofxUST::DIRECTION_DOWN;
  if( _key == OF_KEY_LEFT )  direction = ( int )ofxUST::DIRECTION_LEFT;
  if( _key == OF_KEY_UP )    direction = ( int )ofxUST::DIRECTION_UP;

  if( _key == 'f' ) ofToggleFullscreen();
}

// windowResized
//----------------------------------------
void ofApp::windowResized( int _w, int _h )
{
  w = _w;
  h = _h;
}

// exit
//----------------------------------------
void ofApp::exit()
{
  gui.saveToFile( settingFile.getAbsolutePath() );
}

// setupGui
//----------------------------------------
void ofApp::setupGui()
{
  direction.addListener( this, &ofApp::directionChanged );
  bMirror.addListener( this, &ofApp::mirrorChanged );
  step.addListener( this, &ofApp::stepChanged );
  
  params.setName( "LaserOSC" );
  params.add( fps.set( "FPS", 0, 0, 60 ) );
  params.add( direction.set( "Direction", ( int )ofxUST::DIRECTION_DOWN, 0, ( int )ofxUST::DIRECTION_SIZE - 1 ) );
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

  bDrawGui = true;
}

// directionChanged
//----------------------------------------
void ofApp::directionChanged( int& _direction )
{
  ust.setDirection( ( ofxUST::Direction )_direction );
}

// mirrorChanged
//----------------------------------------
void ofApp::mirrorChanged( bool& _b )
{
  ust.setMirror( bMirror );
}

// stepChanged
//----------------------------------------
void ofApp::stepChanged( int& _v )
{
  ust.stopMeasurement();
  ust.setScanningParameterBySteps( ust.getMinStep(), ust.getMaxStep(), _v );
  ust.startMeasurement();
}
