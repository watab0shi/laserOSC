#include "ofApp.h"

namespace
{
  const int MAX_NUM_MESSAGE_HISTORY = 30;

  const std::vector< std::string > sendingTypeNames = {
    "SENDTYPE_POINT",
    "SENDTYPE_POUINTS_STRING",
    "SENDTYPE_CLUSTERED_POINT",
    "SENDTYPE_CLUSTERED_POINTS_STRING"
  };
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
  
  std::vector< ofVec2f > tmpPoints;
  mmClusterPoints.clear();
  clusterPoints.clear();
  clusterRadius.clear();
  for( int i = 0; i < ust.coordinates.size(); ++i )
  {
    int mmX = ust.coordinates.at( i ).x;
    int mmY = ust.coordinates.at( i ).y;
    
    bool bHit = screenArea.inside( mmX, mmY );
    if( bHit )
    {
      mmTouchPoints.push_back( ofVec2f( mmX, mmY ) );
      
      float x = ofMap( mmX, screenArea.getLeft(), screenArea.getRight(), 0., pxW, true );
      float y = ofMap( mmY, screenArea.getTop(), screenArea.getBottom(), 0., pxH, true );
      
      touchPoints.push_back( ofVec2f( x, y ) );

      tmpPoints.push_back( ofVec2f( mmX, mmY ) );
    }
    else
    {
      // 2ŒÂˆÈã˜A‘±‚µ‚Ä‚½‚ç
      if( tmpPoints.size() >= 10 )
      {
        ofPolyline pl;
        for( auto& p : tmpPoints )
        {
          pl.addVertex( p );
        }
        pl.addVertex( tmpPoints.front() );
        pl = pl.getResampledByCount( 10 );

        ofRectangle rect = pl.getBoundingBox();
        float radius = ofVec2f( rect.width, rect.height ).length() / 2;
        clusterRadius.push_back( radius );

        ofVec2f p = rect.getCenter();
        mmClusterPoints.push_back( p );

        float x = ofMap( p.x, screenArea.getLeft(), screenArea.getRight(), 0., pxW, true );
        float y = ofMap( p.y, screenArea.getTop(), screenArea.getBottom(), 0., pxH, true );

        clusterPoints.push_back( ofVec2f( x, y ) );

        tmpPoints.clear();
      }

      tmpPoints.clear();
    }
  }
  
  switch( ( OscSendingType )sendingType.get() )
  {
  case SENDTYPE_POINT:
    sendPoints( &touchPoints );
    break;

  case SENDTYPE_POUINTS_STRING:
    sendPointsAsString( &touchPoints );
    break;

  case SENDTYPE_CLUSTERED_POINT:
    sendPoints( &clusterPoints );
    break;

  case SENDTYPE_CLUSTERED_POINTS_STRING:
    sendPointsAsString( &clusterPoints );
    break;
  }
}

// sendPointsAsString
//----------------------------------------
void ofApp::sendPointsAsString( std::vector< ofVec2f >* _points )
{
  if( _points->empty() ) return;

  ofxOscMessage message;
  message.setAddress( oscAddress );
  
  std::string pointsString = "";
  int i = 0;
  for( auto& p : *_points )
  {
    if( i > 0 ) pointsString += "/";
    pointsString += ofToString( p.x ) + "," + ofToString( p.y );
    ++i;
  }
  
  message.addStringArg( pointsString );
  oscSender.sendMessage( message );

  messageHistory.push_back( oscAddress.get() + " " + pointsString );
  if( messageHistory.size() > MAX_NUM_MESSAGE_HISTORY )
  {
    messageHistory.pop_front();
  }
}

// sendPoints
//----------------------------------------
void ofApp::sendPoints( std::vector< ofVec2f >* _points )
{
  if( _points->empty() ) return;

  for( auto& p : *_points )
  {
    ofxOscMessage message;
    message.setAddress( oscAddress );
    message.addFloatArg( p.x );
    message.addFloatArg( p.y );
    oscSender.sendMessage( message );

    messageHistory.push_back( oscAddress.get() + " " + ofToString( p.x ) + " " + ofToString( p.y ) );
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
    for( auto& p : ust.coordinates )
    {
      glVertex2f( 0, 0 );
      glVertex2f( p.x, p.y );
    }
    glEnd();
    
    ofFill();
    ofSetColor( 0, 255, 255 );
    for( auto& p : mmTouchPoints )
    {
      ofDrawCircle( p, 5 );
    }

    ofFill();
    ofSetColor( 0, 255, 0 );
    int i = 0;
    for( auto& p : mmClusterPoints )
    {
      //float r = clusterRadius.at( i );
      ofDrawCircle( p, 10 );
      ++i;
    }
    
    ofNoFill();
    ofSetColor( 255 );
    ofDrawRectangle( screenArea );
  }
  ofPopMatrix();
  
  if( bDrawGui )
  {
    int i = 0;
    int y = h - 40;
    for( auto& m : messageHistory )
    {
      ofDrawBitmapString( ofToString( i, 2, '0' ) + " : " + m, 20, y );
      y -= 20;
      ++i;
    }

    ofDrawBitmapString( sendingTypeNames.at( sendingType ), 20, h - 20 );

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
  
  int hue = 0;// gui color
  
  ofxGuiSetFont( "gui/Menlo-Italic.ttf", 10 );
  ofxGuiSetDefaultWidth( 255 );
  ofxGuiSetHeaderColor( ofColor::fromHsb( hue, 255, 180 ) );
  ofxGuiSetFillColor( ofColor::fromHsb( hue, 255, 180 ) );
  ofxGuiSetBackgroundColor( ofColor( 36 ) );
  
  sensorParams.setName( "Sensor" );
  sensorParams.add( direction.set( "Direction", ( int )ofxUST::DIRECTION_DOWN, 0, ( int )ofxUST::DIRECTION_SIZE - 1 ) );
  sensorParams.add( bMirror.set( "Mirror", true ) );
  sensorParams.add( step.set( "Step", 1, 1, 10 ) );
  sensorParams.add( distToScreen.set( "DistToScreen", 0, 0, 2000 ) );
  sensorParams.add( mmW.set( "ScreenW", 4000, 1000, 5000 ) );
  sensorParams.add( mmH.set( "ScreenH", 3000, 1000, 5000 ) );
  sensorParams.add( pxW.set( "PixelW", 1280, 1024, 1920 ) );
  sensorParams.add( pxH.set( "PixelH", 800, 768, 1200 ) );
  
  oscParams.setName( "OSC" );
  oscParams.add( oscHost.set( "OscHost", "127.0.0.1" ) );
  oscParams.add( oscPort.set( "OscPort", 8000, 0, 10000 ) );
  oscParams.add( oscAddress.set( "OscAddress", "/laserPoints" ) );
  oscParams.add( sendingType.set( "SendingType", ( int )SENDTYPE_CLUSTERED_POINT, 0, ( int )SENDTYPE_SIZE - 1 ) );
  
  params.setName( "LaserOSC" );
  params.add( fps.set( "FPS", 0, 0, 60 ) );
  params.add( sensorParams );
  params.add( oscParams );
  
  string savePath = "gui/settings.xml";
  gui.setup( params, savePath );
  gui.setUseTTF( true );
  gui.setSize( 255, gui.getShape().height + 10 );
  gui.setHeaderBackgroundColor( ofColor::fromHsb( hue, 255, 180 ) );
  
  // load gui settings
  settingFile = ofFile( ofToDataPath( "gui/settings.xml" ) );
  if( settingFile.exists() ) gui.loadFromFile( settingFile.getAbsolutePath() );
  
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
