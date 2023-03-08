#include "uri.hpp"
#include "conditions.hpp"
#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

using std::regex_match;
using std::min;

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
	out << "Protocol:\t" << this->protocol << "\n";
	out << "Domain:\t\t" << this->domain << "\n";
	out << "Path:\t\t" << this->path << "\n";
	out << "Filename:\t" << this->filename << "\n";
	out << "Extension:\t" << this->extension << "\n";
	out << "Querystring:\t" << this->querystring << "\n";
	out << "Anchor:\t\t" << this->anchor << "\n";
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

string uri::toString() const
{
	string temp;

	if (!this->protocol.empty())
	{
		temp = this->protocol;
		temp += "://";
	}

	if (!this->domain.empty())
	{
		temp += this->domain;
	}

	if (!this->path.empty())
	{
		temp += "/";
		temp += this->path;
	}

	if (!this->filename.empty())
	{
		temp += "/";
		temp += this->filename;
		if (!this->extension.empty())
			temp += "." + this->extension;
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

uri parse(string original, const uri* parent, bool errOut)
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
	else {
		pos = (int)str.find_last_of(":");
		if (pos == string::npos)
			relative = '/';
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

				if (relative[0] == '.')
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
			if(errOut)
				error_out(">> Missing needed parent URL for " + original);
		}
	}

	try {
		//Let cpp-netlib's uri class parse the completed URL
		network::uri support(str);

		//Convert resulting parts to our uri class
		temp.protocol = support.scheme().to_string();
		temp.domain = support.authority().to_string();

		//Split the path in the correct parts
		temp.path = support.path().to_string();
		//Remove trailing slash
		if (!temp.path.empty() && temp.path.back() == '/')
			temp.path.pop_back();
		//Check if there is a filename in the path
		int lastslash = (int)temp.path.find_last_of("/");
		pos = (int)temp.path.find_last_of(".");
		if (pos != string::npos && lastslash != string::npos && lastslash < pos)
		{
			temp.filename = temp.path.substr(lastslash + 1, pos - lastslash - 1);
			temp.extension = temp.path.substr(pos + 1);
			temp.path.erase(lastslash);
		}
		//Remove starting slash if still there
		if (!temp.path.empty() && temp.path.front() == '/')
			temp.path.erase(0,1);

		temp.querystring = support.query().to_string();
		temp.anchor = support.fragment().to_string();
	}
	//The original URL can't be parsed
	catch (network::uri_syntax_error e)
	{
		if (errOut)
			error_out(">> Parsing error on " + original + "\n   Found in " + parent->toString() + " : " + (string)e.what() );
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
	
	result = result && regex_match(this->toString(), r.global);

	return result;
}

bool uri::check(regex r) {
	return regex_match(this->toString(), r);
}

string relative(uri absolute, const uri& base)
{
	//If the URL points to a directory, point to its index.html instead
	if (absolute.filename.empty())
	{
		absolute.filename = "index";
		absolute.extension = "html";
	}

	string a = absolute.toString();

	string b = base.toString();
	if (base.filename.empty()) b += '/';

	size_t minLen = min(a.length(), b.length());
	size_t i;

	//Remove the part in common
	for (i = 0; a[i] == b[i] && i < minLen; i++)
		;

	a = a.erase(0, i);
	b = b.erase(0, i);
		
	size_t parentCounter = std::count(b.begin(), b.end(), '/');

	for (i = 0; i < parentCounter; i++)
		a = "../" + a;

	a = std::regex_replace(a, regex("\\?"), "_");
	a = std::regex_replace(a, regex(":|\\*|\\?|\"|<|>|\\|"), "");

	return a;
}