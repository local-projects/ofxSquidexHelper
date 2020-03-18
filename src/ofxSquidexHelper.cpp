//
//  SquidexHelper.cpp
//  SquidexTest
//
//  Created by Cameron Erdogan on 2/19/20.
//

#include "ofxSquidexHelper.h"

/*
 a slight more first party way...less robust however
 
    std::ostringstream bodyStream;
    bodyStream << "grant_type=client_credentials&client_id=" << clientID << "&client_secret=" << clientSecret << "&scope=squidex-api";
    string body = bodyStream.str();
    
    ofHttpRequest request;
    request.method = ofHttpRequest::POST;
    request.url = url;
    request.body = body;

    ofURLFileLoader http;
    auto response = http.handleRequest(request);

    std::cout << "token request:" << std::endl;
    std::cout << "status " << response.status << std::endl;
    std::cout << "data " << response.data.getText() << std::endl;
    std::cout << "error " << response.error << std::endl;

    if(response.data.getText().empty() == false){
        ofxJSON responseJSON = ofxJSON(response.data.getText());
        string authToken = responseJSON["access_token"].asString();
        return authToken;
    }else{
        return "";
    }
 
 */

/*
 
 GET TOKEN
 https://cloud.squidex.io/api/docs#section/Authentication
 
 Function : Makes post request to squidex to retrieve auth token
 
 */

string ofxSquidexHelper::getSquidexToken(string url, string clientID, string clientSecret){
    ofxHTTP::Client client;

    ofxHTTP::PostRequest request(url);
    std::multimap<std::string, std::string> formFields =
    {
        { "grant_type", "client_credentials" },
        { "client_id", clientID },
        { "client_secret", clientSecret },
        { "scope", "squidex-api" }
    };
    request.addFormFields(formFields);

    try
    {
        // Execute the request.
        auto response = client.execute(request);

        // Check the response.
        if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
        {
            // A successful response.

            // We expect JSON in response, so we don't bother checking and
            // just read the response directly into an ofJson object.
            //
            // We can also get the response as a ofBuffer (response->buffer()),
            // ofXml, ofPixels, etc and can check to see the response type with
            // the response->isXXX() methods.

            // info on the json here:
            // https://github.com/nlohmann/json#json-as-first-class-data-type
            ofJson responseJson = response->json();

            return responseJson["access_token"];
        }
        else
        {
            ofLogError("ofApp::setup") << response->getStatus() << " " << response->getReason();
        }
    }
    catch (const Poco::Exception& exc)
    {
        ofLogError("ofApp::setup") << exc.displayText();
    }
    catch (const std::exception& exc)
    {
        ofLogError("ofApp::setup") << exc.what();
    }

    return "";
}

/*
   the "first party" way. again a little less robust.
 
   std::map<string, string> headers;
   std::ostringstream authStream;
   authStream << "Bearer " << token;
   
   headers["Authorization"] = authStream.str();
   
   ofHttpRequest request;
   request.method = ofHttpRequest::GET;
   request.url = url;
   request.headers = headers;
   
   ofURLFileLoader http;
   auto response = http.handleRequest(request);
       
   std::cout << "content request" << std::endl;
   std::cout << "status " << response.status << std::endl;
   std::cout << "data " << response.data.getText() << std::endl;
   std::cout << "error " << response.error << std::endl;

   return response.data.getText();
 */

/*
 
 BASIC GET
 
 Function : makes a get request with provided url & token - returns JSON object
            This function can be used for any basic get requests to squidex that return JSON
 
 */

ofJson ofxSquidexHelper::getSquidexContent(string url, string token){
    
    std::ostringstream authStream;
    authStream << "Bearer " << token;
    
    ofxHTTP::Client client;
    ofxHTTP::GetRequest request(url);
    ofxHTTP::ClientSessionSettings settings;
    Poco::Net::NameValueCollection defaultHeaders;
    defaultHeaders.add("Authorization", authStream.str());
    settings.setDefaultHeaders(defaultHeaders);
    client.context().setClientSessionSettings(settings);
    
    try
    {
        // Execute the request.
        auto response = client.execute(request);

        // Check the response.
        if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
        {
            // A successful response.

            // We expect JSON in response, so we don't bother checking and
            // just read the response directly into an ofJson object.
            //
            // We can also get the response as a ofBuffer (response->buffer()),
            // ofXml, ofPixels, etc and can check to see the response type with
            // the response->isXXX() methods.

            // info on the json here:
            // https://github.com/nlohmann/json#json-as-first-class-data-type
            ofJson responseJson = response->json();

            return responseJson;
        }
        else
        {
            ofLogError("ofApp::setup") << response->getStatus() << " " << response->getReason();
        }
    }
    catch (const Poco::Exception& exc)
    {
        ofLogError("ofApp::setup") << exc.displayText();
    }
    catch (const std::exception& exc)
    {
        ofLogError("ofApp::setup") << exc.what();
    }

    return "";
}

/*
 
 GET ASSET CONTENT
 https://cloud.squidex.io/api/docs#operation/AssetContent_GetAssetContent
 
 Function : download image asset content from squidex - returns ofPixels to load images
 uri structure : https://cloud.squidex.io/api/assets/{id}
 
 May want to add ability to get extented asset info to define image format size, etc..
 However default detection seems to be working fine atm
 - if and when this is wanted getAsset extended info via "items/_links/self" uri
 
 */
ofPixels ofxSquidexHelper::getSquidexAssetContent( string url, string token ){

    std::ostringstream authStream;
    authStream << "Bearer " << token;
    
    ofxHTTP::Client client;
    ofxHTTP::GetRequest request( url );
    ofxHTTP::ClientSessionSettings settings;
    Poco::Net::NameValueCollection defaultHeaders;
    defaultHeaders.add( "Authorization", authStream.str() );
    settings.setDefaultHeaders( defaultHeaders );
    client.context().setClientSessionSettings( settings );
    
    try
    {
        //exe request
        auto response = client.execute( request );
        
        //check for success
        if( response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK )
        {
            // response->pixels() Method Causing Errors
            //
            // std::cout << "Is  Pixels : " << response->isPixels() << std::endl;
            // p = response->pixels();
            //
            // So we check there is a stream
            // and buffer the response & load into pixels
            if( response->hasResponseStream() )
            {
                ofPixels p;
                ofBuffer buffer( response->stream() );
                ofLoadImage( p, buffer );
                return p;
            }
            else
            {
                ofLogError("ofxSquidexHelper::getSquidexAsset") << "Response has no stream";
            }
        }
        else
        {
            ofLogError("ofxSquidexHelper::getSquidexAsset") << response->getStatus() << " " << response->getReason();
        }
    }
    catch ( const Poco::Exception& exc )
    {
        ofLogError("ofxSquidexHelper::getSquidexAsset") << exc.displayText();
    }
    catch ( const std::exception& exc )
    {
        ofLogError("ofxSquidexHelper::getSquidexAsset") << exc.what();
    }
    
    ofPixels p;
    return p;
}
 

/*
 
@TODO: INCOMPLETE

POST NEW ASSET
https://cloud.squidex.io/api/docs#operation/Assets_PostAsset

Function : Uploads Image from the path provided to squidex - returns JSON response
uri structure : https://cloud.squidex.io/api/apps/{app}/assets
params : $parentId=XXXX-XXXX-XXXX

ParentId param denotes which directory the asset should go
to retrieve all parent folders : https://cloud.squidex.io/api/docs#operation/AssetFolders_GetAssetFolders
 
*/
ofJson ofxSquidexHelper::postSquidexAsset(string url, string token, string localPath, string filename){
    
    string file = localPath + filename;
    
    std::ostringstream authStream;
    authStream << "Bearer " << token;
    
    ofxHTTP::Client client;
    ofxHTTP::PostRequest request( url );
    ofxHTTP::ClientSessionSettings settings;
    Poco::Net::NameValueCollection defaultHeaders;
    
    defaultHeaders.add("Authorization", authStream.str());
    settings.setDefaultHeaders(defaultHeaders);
    client.context().setClientSessionSettings(settings);
    
    
    
    //-----------------------------------------
    //@TODO: ERROR - THIS DOESNT WORK
    //Squidex takes 1 file at a time / Multipart encoded content
    // ERROR : 400 - "Cannot create asset"
    
    //lets test sending MIME with it
    std::multimap<std::string, std::string> formFields =
    {
        {"base_64_encoded_image", ofxIO::Base64Encoding::encode( ofBufferFromFile( file ) )},
        {"mimeType", "image/png"}
    };
    
    //    std::vector<ofxHTTP::FormPart> formFiles = {
    //        ofxHTTP::FormPart(ofxHTTP::FormPart::Type::FILE, "TEST", file)
    //    };
    
    
    request.addFormFields( formFields );
    
    //request.addFormParts( formFiles );
    
    request.addFormFile( filename, file, "image/png" );
    
    //request.setFormEncoding(ofx::HTTP::PostRequest::FORM_ENCODING_MULTIPART);

    //std::cout << request.getContentType() << std::endl;
    
    //-----------------------------------------
    
    try
    {
        auto response = client.execute( request );
        
        if(response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
        {
            ofJson responseJson = response->json();
            return responseJson;
        }
        else if( response->getStatus() == Poco::Net::HTTPResponse::HTTP_CREATED)
        {
            ofJson responseJson = response->json();
            return responseJson;
        }
        else
        {
            ofLogError("ofxSquidexHelper::postSquidexNewAsset") << response->getStatus() << " " << response->getReason();
            //debugging
            ofJson responseJson = response->json();
            return responseJson;
        }
    }
    catch (const Poco::Exception& exc)
    {
        ofLogError("ofxSquidexHelper::postSquidexNewAsset") << exc.displayText();
    }
    catch (const std::exception& exc)
    {
        ofLogError("ofxSquidexHelper::postSquidexNewAsset") << exc.what();
    }
    
    return "";
    
}

/*
 
@TODO: UNTEST

POST NEW CONTENT

Function : Posts json object to squidex
This can really be used for any basic POST with JSON body
 
*/
ofJson ofxSquidexHelper::postSquidexContent(string url, string token, ofJson data){
    
    std::ostringstream authStream;
    authStream << "Bearer " << token;
    
    ofxHTTP::Client client;
    ofxHTTP::JSONRequest request( url, "HTTP/1.1" );
    
    ofxHTTP::ClientSessionSettings settings;
    Poco::Net::NameValueCollection defaultHeaders;
    
    defaultHeaders.add("Authorization", authStream.str());
    settings.setDefaultHeaders(defaultHeaders);
    client.context().setClientSessionSettings(settings);
    
    request.setJSON( data );
    
    try
    {
        auto response = client.execute( request );
        
        if(response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
        {
            ofJson responseJson = response->json();
            return responseJson;
        }
        else if( response->getStatus() == Poco::Net::HTTPResponse::HTTP_CREATED)
        {
            ofJson responseJson = response->json();
            return responseJson;
        }
        else
        {
            ofLogError("ofxSquidexHelper::postSquidexContent") << response->getStatus() << " " << response->getReason();
            ofJson responseJson = response->json();
            return responseJson;
        }
    }
    catch (const Poco::Exception& exc)
    {
        ofLogError("ofxSquidexHelper::postSquidexContent") << exc.displayText();
    }
    catch (const std::exception& exc)
    {
        ofLogError("ofxSquidexHelper::postSquidexContent") << exc.what();
    }
    
    return "";
    
}
