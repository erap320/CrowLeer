#include "uri.hpp"
#include "conditions.hpp"
#include "utils.hpp"

#include <iostream>
#include <fstream>

using std::cout; using std::endl;
using std::regex_match;

uri uri::operator=(const uri& other)
{
	this->protocol = other.protocol;
	this->domain = other.domain;
	this->path = other.path;
	this->filename = other.filename;
	this->extension = other.extension;
	this->querystring = other.querystring;
	this->anchor = other.anchor;
	this->depth = other.depth;
	return *this;
}

void uri::debug()
{
	cout << "Protocol:\t" << this->protocol << endl;
	cout << "Domain:\t\t" << this->domain << endl;
	cout << "Path:\t\t" << this->path << endl;
	cout << "Filename:\t" << this->filename << endl;
	cout << "Extension:\t" << this->extension << endl;
	cout << "Querystring:\t" << this->querystring << endl;
	cout << "Anchor:\t\t" << this->anchor << endl;
}

uri::uri()
{
	this->depth = 0;
}

uri::uri(string str)
{
	*this = parse(str);
	this->depth = 0;
}

string uri::tostring()
{
	string temp;

	if (!this->protocol.empty())
	{
		temp = this->protocol;
		temp += "://";
	}

	temp += this->domain;

	if (!this->path.empty())
	{
		temp += "/";
		temp += this->path;
	}

	if (!this->filename.empty())
	{
		temp += "/";
		temp += this->filename + "." + this->extension;
	}

	if (!this->querystring.empty())
	{
		temp += "?";
		temp += this->querystring;
	}

	if (!this->anchor.empty())
	{
		temp += "#";
		temp += this->anchor;
	}

	return temp;
}

string trim(string str)
{
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == ' ')
			str.erase(i,1);
	}

	return str;
}

uri parse(string original, uri* const parent)
{
	string str;
	string completion;
	uri temp;
	int pos;

	//Remove white spaces
	str = trim(original);

	string relative = ""; //Holds relative paths informations

	if (parent != nullptr)
	{
		temp.depth = parent->depth + 1;
	}

	//Check type of relative path
	if (str[0] == '\\' || str[0] == '.' || str[0] == '/' || str[0] == '?' || str[0] == '#')
	{
		relative = str[0];
		str.erase(0, 1);

		if (relative == "/" && str[0] == '/')
		{
			relative = "//";
			str.erase(0, 1);
		}

		if (relative == ".")
		{
			if (str[0] == '.')
			{
				relative = "..";
				str.erase(0, 1);
			}
			str.erase(0, 1);
		}
	}

	//Completion
	if (!relative.empty())
	{
		if (parent != nullptr)
		{
			if (!parent->protocol.empty())
			{
				completion = parent->protocol;
				completion += "://";
			}

			if (relative != "//")
			{
				completion += parent->domain + "/";

				if (relative[0] == '.' || relative == "/")
				{
					if (!parent->path.empty())
					{
						completion += parent->path;
						if (relative == "..")
						{
							pos = (int)completion.find_last_of("/");
							if (pos != string::npos)
								completion.erase(pos);
						}
					}

					completion += "/";
				}
				else if (relative == "?" || relative == "#")
				{
					if(!parent->filename.empty())
						completion += parent->filename + "." + parent->extension;
					if (relative == "?")
					{
						completion += "?";
					}
					if (relative == "#" && !parent->querystring.empty())
					{
						completion += "?" + parent->querystring + "#";
					}
				}
			}

			str.insert(0, completion);
		}
		else
		{
			error_out(">> Missing needed parent URL for " + original);
		}
	}

	try {
		//Let cpp-netlib's uri class parse the completed URL
		network::uri support(str);

		//Convert resulting parts to our uri class
		temp.protocol = support.scheme().to_string();
		temp.domain = support.authority().to_string();
		temp.path = support.path().to_string();
		temp.path.erase(0, 1);
		if (temp.path.back() == '/')
			temp.path.pop_back();
		temp.querystring = support.query().to_string();
		temp.anchor = support.fragment().to_string();

		//Check if there is a filename in the path
		int lastslash = (int)temp.path.find_last_of("/");
		pos = (int)temp.path.find_last_of(".");
		if (pos != string::npos && lastslash != string::npos && lastslash < pos)
		{
			temp.filename = temp.path.substr(lastslash + 1, pos-lastslash-1);
			temp.extension = temp.path.substr(pos+1);
			temp.path.erase(lastslash);
		}
	}
	//The original URL can't be parsed
	catch (network::uri_syntax_error e)
	{
		error_out(">> Parsing error on " + original + "\n   Found in " + parent->tostring());

	}

	return temp;
}

bool uri::check(rule r) {
	bool result = true;
	result = result && regex_match(this->protocol, r.protocol);
	result = result && regex_match(this->domain, r.domain);
	result = result && regex_match(this->path, r.path);
	result = result && regex_match(this->filename, r.filename);
	result = result && regex_match(this->extension, r.extension);
	result = result && regex_match(this->querystring, r.querystring);
	result = result && regex_match(this->anchor, r.anchor);
	
	result = result && regex_match(this->tostring(), r.global);

	return result;
}

bool uri::check(regex r) {
	return regex_match(this->tostring(), r);
}
