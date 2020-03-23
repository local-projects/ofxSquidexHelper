#pragma once

#include "ofMain.h"
#include "ofxSquidexHelper.h"
#include "ofxJsonSettings.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    void initToken();
    void getContent();
    void getAssets();
    void getAssetContent( vector<string> ids );
    void postContent( string id );

    //@TODO: NOT WORKING - SQUIDEXHELPER ISSUE
    void postAsset( string fileName, string cmsParentFolder );

private:
    ofxSquidexHelper squidex;
    string accessToken;
    string appName, contentParent, clientID, clientSecret;
    
    vector<ofImage> assets;
    int curr_img;
    string test_asset_id;

};
