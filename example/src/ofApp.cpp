#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowShape(550, 400);
    curr_img = 0;
    
    //Update these to your CMS info
    //Or create a settings.json file in bin/data/tmp - refer to readme for more info
    Settings::get().load("tmp/settings.json");
    
    appName = Settings::getString("appName");
    clientID = Settings::getString("clientID");
    clientSecret = Settings::getString("clientSecret");
    contentParent = Settings::getString("contentParent");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
   
    ofBackgroundGradient(ofColor::linen, ofColor::white, OF_GRADIENT_CIRCULAR);
    
    ofSetColor(ofColor::red);
    float y = 30 + 20 * sinf( 0.1 * ofGetFrameNum() );
    ofDrawCircle(24, y, 5);
    
    ofDrawBitmapString("Press '1' to initiate token request.\nPress '2' to getContent & download assets to display\nPress 'n' to display next image ( if assets downloaded )\nPress '3' to post hardcoded asset /uploads/1_TEST.png ( WIP )\nPress '4' to post a test content piece \nOutput will be in console.", 36, 30);
    
    ofPushMatrix();
    if( assets.size () > 0 ){
        ofSetColor(ofColor::white);
        auto w = assets[ curr_img ].getWidth()/5, h = assets[ curr_img ].getHeight()/5;
        ofTranslate(( ofGetWidth() /2 ) - (w/2),  ( ofGetHeight() /1.5 ) - (h/2));
        assets[ curr_img ].draw( 0, 0, w ,h );
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::initToken(){
   
    std::cout << "Init Request for Access Token Has Started...\n";
    
    if( accessToken.empty() ){
        
        string url = "https://cloud.squidex.io/identity-server/connect/token";
        
        accessToken = squidex.getSquidexToken(url, clientID, clientSecret);
        
        std::cout << "Retrieved Token : " << accessToken;
        
    }else{
        std::cout << "Acess Token already present : " << accessToken;
    }
    
    std::cout << "\nInit End" << std::endl;
    
}

//--------------------------------------------------------------
//gets all content per params defined in uri
//then passes to getAssetContent() to download the content & display
//--------------------------------------------------------------
void ofApp::getContent(){
   
    std::cout << "Content Query has started...\n";
    
    //get token if you dont have one
    if( accessToken.empty() ) initToken();
    
    string url = "https://cloud.squidex.io/api/content/" + appName + "/" + contentParent + "/";
    
    //optional params -
    //here we are asking for the top 200 content objects that are marked as approved
    //
    //string params = "?$top=200&$filter=data/isApproved/iv%20eq%20true";
    //url += params;
    
    //make request
    ofJson content = squidex.getSquidexContent( url, accessToken );
    
    std::cout << "Content Retrieved : \n" << content.dump(2);
    std::cout << "\nContent Query Complete" << std::endl;
    std::cout << "--Start of Content";
    
    //parse out image ids from json & add to array
    vector<string> ids;
    auto items = content.at("items");
    
    for( auto & i : items ){
        auto id = i.at("data").at("image").at("iv")[0].get<std::string>();
        std::cout << "\nimage id: " << id;
        ids.push_back(id);
    }
    
    std::cout << "\n--End of Content ids" << std::endl;
    
    //send ids to getAssets to download images
    if( ids.size() > 0 ) getAssetContent( ids );
    
}

//--------------------------------------------------------------
// Gets multiple assets based on vector of asset ids
// & loads them into an image vector for display
//--------------------------------------------------------------
void ofApp::getAssetContent( vector<string> ids ){
   
    std::cout << "Get Assets Query Has Started...\n";
    
    if( accessToken.empty() ) initToken();
    
    for( auto & id : ids ){
        
        string url = "https://cloud.squidex.io/api/assets/" + id;
        
        std::cout << "requesting image from : " << url << "\n";
        
        ofPixels p = squidex.getSquidexAssetContent( url, accessToken );
        
        std::cout << "pixels size: " << p.size() << "\n";
        
        ofImage tmp;
        tmp.setFromPixels(p);
        assets.push_back(tmp);

    }
    
    std::cout << "Added " << assets.size() << " images to assets array" << std::endl;
    
}

//--------------------------------------------------------------
// Requests all Assets in app
// returns json object with all assets & details
//--------------------------------------------------------------
void ofApp::getAssets( ){
   
    std::cout << "Get Assets Query Has Started...\n";
    
    if( accessToken.empty() ) initToken();
    
    string url = "https://cloud.squidex.io/api/apps/" + appName + "/assets";
    string params = "";
    
    ofJson data =  squidex.getSquidexContent( url, accessToken );
    
    std::cout << "Result : \n" << data.dump(2) << "\n" << std::endl;
    
    //assigning the first asset id I find to var - asset_id;
    //this is to test postContent function as we need an asset_id
    //ideally we would first postAsset() get id then postContent()
    ofJson assets = data.at("items");
    test_asset_id = assets[0].at("id").get<std::string>();
    
}

//--------------------------------------------------------------
//PLEASE NOTE : to use this, ensure your content data object matches that in CMS -
//              these fields are specific to my content object
//
//takes asset id and creates a json object of content data to send to cms
//this approach can be used for any basic Squidex POST JSON data
//--------------------------------------------------------------
void ofApp::postContent( string id ){
    
    std::cout << "Post Content has started...\n";
    
    if( accessToken.empty() ) initToken();
    
    //for testing - we grab all asset ids & assign one to test_asset_id
    if( test_asset_id.empty() ) getAssets();
    id = test_asset_id;
    //
    
    string url =  "https://cloud.squidex.io/api/content/" + appName + "/" + contentParent + "/";
    string params = "?publish=true";
    
    //build data
    //content design specific
    ofJson data = {
        { "stationID" , {
            { "iv" , "1" }
        }},
        { "image" , {
            { "iv", { id }}
        }},
        { "isApproved" , {
            { "iv" , false }
        }}
    };
    
    std::cout << "Post Content data : \n" << data.dump(2) << "\n";

    ofJson response = squidex.postSquidexContent( url + params, accessToken, data );
    
    std::cout << "\n" << response.dump(2) << "\n" << std::endl;
    
    std::cout << "Post Content Complete " << std::endl;
}

//--------------------------------------------------------------
// @TODO: NOT WORKING - SQUIDEX HELPER NEEDS WORK ON THIS FUNCTION
//
// Supposed to take the filename of the file to upload to squidex as an asset
// & return a JSON object so we can assign that asset to a content piece
//--------------------------------------------------------------
void ofApp::postAsset( string fileName, string cmsParentFolder ){
   
    //---------------------------------
    //POST Image Asset To Squidex
    //---------------------------------
    
    std::cout << "Post Asset has started...\n";
    
    if( accessToken.empty() ) initToken();
    
    string path = "uploads/" + fileName;
    
    //check file exists
    if( !ofFile::doesFileExist( path , true ) ){
        ofLogError("ofApp::postAsset") << "Couldn't find : " << path;
        return;
    }
    
    string url = "https://cloud.squidex.io/api/apps/" + appName + "/assets";
    
    // IF 'cmsParentFolder" value is not "" we get all parent folders to find ID of 'cmsParentFolder'
    // this is appended to url as param
    if ( !cmsParentFolder.empty() ){

        std::cout << "Retrieving Asset Folders...\n";

        //get asset folders from squidex - so we can retrieve ID
        string s = "https://cloud.squidex.io/api/apps/" + appName + "/assets/folders";

        //use getSquidexContent - for basic get/ json requests
        ofJson data = squidex.getSquidexContent( s, accessToken );

        std::cout << data.dump(2) << "\n";

        auto items = data.at("items");

        for( auto & i : items ){
            if( i.at("folderName") == cmsParentFolder){
                //grab id & append to url
                url = url + "?parendId=" + i.at("id").get<std::string>();
                std::cout << "Found Matching Folder \n";
            }else{
                ofLogError("ofApp::postAsset") << "Unable to find parent folder with the name specified in settings - will post to root directory";
            }
        }
    }
    
    std::cout << "Posting asset to " << url << std::endl;
    
    //debugging - posting to this url echoes back our request
    //url = "https://postman-echo.com/post";
    
    //make the post - not working yet
    ofJson data = squidex.postSquidexAsset( url, accessToken, "uploads/" , fileName );
    
    //print out result ( atm will print our error message )
    std::cout << "\n" << data.dump(2) << "\n" << std::endl;
    
    //---------------------------------
    //POST Content Object to Squidex with New Asset Info
    //---------------------------------
    
    //1. if success retrieve new asset id from response
    
    //2. postContent ( string fileName, string id) [ TBD ]
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch(key) {
        case '1' :
            initToken();
            break;
        case '2' :
            getContent();
            break;
        case '3' :
            postAsset( "1_TEST.png", "HopeContribution" );
            break;
        case '4' :
            postContent( test_asset_id );
            break;
        case 'n' :
            curr_img++;
            curr_img = curr_img % assets.size();
            break;
        default:
            break;
    }
    
}


