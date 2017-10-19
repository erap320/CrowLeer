#pragma once
#include <string>

using std::string;

class uri;

//Divide a string representing a URI in its components for easier management
uri parse(string str);

//Remove every space character from string
string trim(string str);