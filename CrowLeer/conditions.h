#pragma once
#include <regex>

using std::regex;

class rule {
	regex protocol;
	regex domain;
	regex path;
	regex filename;
	regex extension;
	regex querystring;
	regex anchor;

	rule();
};
