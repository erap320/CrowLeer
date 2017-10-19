#include "uri.h"

#include <iostream>

using std::cout; using std::endl;

class uri {
public:
	string protocol;
	string domain;
	string path;
	string filename;
	string extension;
	string querystring;
	string anchor;

	uri operator=(const uri& other)
	{
		this->protocol = other.protocol;
		this->domain = other.domain;
		this->path = other.path;
		this->filename = other.filename;
		this->extension = other.extension;
		this->querystring = other.querystring;
		this->anchor = other.anchor;
		return *this;
	}

	void debug()
	{
		cout << "Protocol:\t" << this->protocol << endl;
		cout << "Domain:\t" << this->domain << endl;
		cout << "Path:\t" << this->path << endl;
		cout << "Filename:\t" << this->filename << endl;
		cout << "Extension:\t" << this->extension << endl;
		cout << "Querystring:\t" << this->querystring << endl;
		cout << "Anchor:\t" << this->anchor << endl;
	}

	uri()
	{}

	uri(string str)
	{
		*this = parse(str);
	}
};

uri parse(string str)
{
	int pos;
	uri temp;
	str = trim(str);
	pos = str.find(":");
	if (!pos == string::npos)
		temp.protocol = str.substr(0, pos);
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