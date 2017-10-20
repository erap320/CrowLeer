#pragma once
#include <string>

using std::string;

class uri {
public:
	string protocol;
	string domain;
	string path;
	string filename;
	string extension;
	string querystring;
	string anchor;

	uri operator=(const uri& other);
	void debug();
	uri();

	//Parse the string into the uri parts
	uri(string str);

	//Return the uri in a string format made by putting together its components
	string tostring();
};

//Remove every space character from string
string trim(string str);

//Divide a string representing a URI in its components for easier management, and make it absolute using a parent uri
uri parse(string str, uri* const parent = nullptr);
