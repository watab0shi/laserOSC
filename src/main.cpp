#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#include "ofMain.h"
#include "ofApp.h"


// main
//----------------------------------------
int main()
{
  ofSetupOpenGL( 1280,800,OF_WINDOW );
  ofRunApp( new ofApp() );
}
