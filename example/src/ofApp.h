#pragma once

#include "ofMain.h"
#include "ofxSquidexHelper.h"

class ofApp : public ofBaseApp{

public:
    //default OF calls
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

private:
    ofxSquidexHelper squidexHelper;

};
