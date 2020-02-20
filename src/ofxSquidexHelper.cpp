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

            return responseJson.dump();
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
