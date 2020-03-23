# ofxSquidexHelper

## TODO: 

* **Fix : postSquidexAsset()** - unable to post asset to squidex, notes regarding the error can be found in code. 

##Intro

Helper for Interfacing with Squidex CMS. Current Functionality includes; 

* Post/ to retrieve oauth token 
* Get/ Basic Request returning JSON 
* Get/ Asset Content Request returning ofPixels
* Post/ Basic JSON data request with JSON response
* Post/ Asset Image Request [ WIP ]. 

A note about Naming re : Content vs. Asset

* CONTENT on Squidex is an object with many attributes, one being an ASSET_ID - which links the ASSET to the CONTENT object
* An ASSET is the actual asset object, including the file and other info
* Therefore 
	* getContent - is getting the content object
	* getAssets - is getting the Asset object 
	* getAssetContent - is getting the asset file


## Dependancies :

	ofxHTTP
		ofxPoco ( included w/ OF )
		ofxSSLManager
		ofxIO
		ofxMediaType
		ofxNetworkUtils


## Usage

**SSLManager set-up :** Requires Client Certificate is located in `bin/data/ssl` download latest here [https://curl.haxx.se/docs/caextract.html](https://curl.haxx.se/docs/caextract.html). Additionally one can be found in `example/bin/data/ssl`

**getSquidexToken( string url, string clientID, string clientSecret )**

* Post request to retrieve oauth token
* Returns token as string
* Token is required for any Squidex request to be authenticated
* [Squidex - Authentication](https://cloud.squidex.io/api/docs#section/Authentication)

**getSquidexContent( string url, string token )**

* General Get request - can be used for any basic GET/ request that responds with JSON.
* returns [ofJson](https://github.com/nlohmann/json) object 


**getSquidexAssetContent( string url, string token )**

* Get Asset Image - Returns an ofPixel Object of the asset ready for usage
* Asset Id is required as URI param 
* [Squidex - Get The Asset Content](https://cloud.squidex.io/api/docs#operation/AssetContent_GetAssetContent)

**postSquidexContent( string url, string token, ofJson data )**

* General JSON POST request - can be used for any basic POST/ request that sends and recieves a JSON object
* Refer to [ofJson](https://github.com/nlohmann/json) for how to build ofJson data object

**[WIP] postSquidexAsset( string url, string token, string localPath, string filename )**

* Post Asset Image - Posts base_64 encoded Image loaded from file
* Optional Param for Parent Folder id
* Returns JSON object with asset information ( including ID ) 

##Example

To run example code;

* make sure you have SSL cert in `bin/data/ssl`
* update the fields in setup - regarding the squidex app credentials


About example code;

* hit '1' to generate access token
* hit '2' to getContent Objects, then Pass them through to getAssetContent to download the images and add to an array to displaying
* hit '3' to post image to cms, image located in `bin/data/uploads` file name must be `"1_TEST.png"` [ this function does not work correctly ]
* hit '4' to get a random asset from cms and generate a content object for it using a JSON post
* hit 'n' while images are present to loop through array and display one by one
