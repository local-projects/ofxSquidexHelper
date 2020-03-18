//
//  SquidexHelper.hpp
//  SquidexTest
//
//  Created by Cameron Erdogan on 2/19/20.
//

#pragma once

#include "ofMain.h"
#include "ofxHTTP.h"
#include "ofURLFileLoader.h"

class ofxSquidexHelper {
public:
    string getSquidexToken(string url, string clientID, string clientSecret);
    ofJson getSquidexContent(string url, string token);
    ofPixels getSquidexAssetContent(string url, string token);
    ofJson postSquidexAsset(string url, string token, string localPath, string filename);
    ofJson postSquidexContent(string url, string token, ofJson data);
};
