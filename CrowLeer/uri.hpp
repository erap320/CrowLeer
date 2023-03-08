#ifndef URI_HPP
#define URI_HPP

#include "conditions.hpp"
#include <string>
#include "network/uri.hpp"

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

	//Number of files crawled to get to get to this uri
	unsigned int depth;

	uri operator=(const uri& other);
	void debug();
	uri();

	//Parse the string into the uri parts
	uri(string str);

	//Return the uri in a string format made by putting together its components
	string toString() const;

	//Check if the uri respects the rules defined by regex
	bool check(rule r); //With a rule type on each component
	bool check(regex r); //With a global rule to the generated URL string
};

//Remove every space character from string
string trim(string str);

//Divide a string representing a URI in its components for easier management, and make it absolute using a parent uri
uri parse(string str, const uri* parent = nullptr, bool errOut = true);

//Makes an uri relative by removing its protocol and domain
string relative(uri absolute, const uri& base);

#endif