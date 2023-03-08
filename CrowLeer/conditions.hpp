#ifndef CONDITIONS_HPP
#define CONDITIONS_HPP

#include <regex>

using std::regex;

class rule {
public:
	//Rules for different parts fo the uri
	regex protocol;
	regex domain;
	regex path;
	regex filename;
	regex extension;
	regex querystring;
	regex anchor;

	//Rule to match the .tostring() of the uri
	regex global;

	rule();
};

#endif