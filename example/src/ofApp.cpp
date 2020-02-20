#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
//    ofBackground(22, 0, 0);

    ofSetWindowShape(400, 150);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
   
    ofBackgroundGradient(ofColor::linen, ofColor::white, OF_GRADIENT_CIRCULAR);
    
    ofSetColor(ofColor::red);
    ofDrawBitmapString("Press 1 to initiate request.\nOutput will be in console.", 36, 30);
    float y = 30 + 20 * sinf( 0.1 * ofGetFrameNum() );
    ofDrawCircle(24, y, 5);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='1'){
        std::cout << "starting request" << std::endl;
        
        // token stuffs
        string tokenURL = "https://cloud.squidex.io/identity-server/connect/token";
        string clientID = "lptest1:ofxapp-test-client";
        string clientSecret = "wneesi0nbko1uc38o4t2ykryxvu2sv6icmlcps0rrvyx";

        string accessToken = squidexHelper.getSquidexToken(tokenURL, clientID, clientSecret);

        std::cout << "got token " << accessToken << std::endl;

        //if the token isn't empty
        if(accessToken.empty() == false){
            // getting content stuff
            string contentURL = "https://cloud.squidex.io/api/content/lptest1/ofxtester/";
            ofJson squidexContent = squidexHelper.getSquidexContent(contentURL, accessToken);

            std::cout << "got content " << squidexContent << std::endl;
        }else{
            std::cout << "couldn't get token" << std::endl;
        }
    }else if (key == '2'){

    }
}


