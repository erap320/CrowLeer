#include "uri.h"

#include <iostream>

using std::cout; using std::endl;

uri uri::operator=(const uri& other)
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
{}

uri::uri(string str)
{
	*this = parse(str);
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

uri parse(string str, uri* const parent)
{
	uri temp;
	int pos;
	int end;

	str = trim(str);

	string relative=""; //Holds relative paths informations

	//Relative path
	if (str[0] == '\\' || str[0] == '.' || (str[0]=='/' && str[1]!='/') )
	{
		relative = str[0];
		str.erase(0, 1);
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

	//Protocol
	pos = str.find(":");
	if (pos != string::npos && str[pos+1]=='/' && str[pos+2]=='/')
	{
		temp.protocol = str.substr(0, pos);
		str.erase(0, pos);
	}
	else if (parent != nullptr)
		temp.protocol = parent->protocol;

	//Domain
	if (relative.empty())
	{

		pos = str.find("//");
		if (pos != string::npos)
		{
			pos += 2;
			end = str.find("/", pos);
			if (end != string::npos)
			{
				temp.domain = str.substr(pos, end - pos);
				str.erase(0, end + 1);
			}
			else
			{
				temp.domain = str.substr(pos);
				str.clear();
				return temp;
			}
		}
		else if (parent != nullptr)
		{
			temp.domain = parent->domain;
		}
	}
	else if(parent != nullptr)
	{
		temp.domain = parent->domain;
		if (relative == ".")
			temp.path = parent->path;
		else if (relative == "..")
		{
			temp.path = parent->path;
			pos = temp.path.find_last_of("/");
			if (pos != string::npos)
				temp.path.erase(pos);
		}
	}

	//Path
	pos = str.find_last_of("/");
	if(pos != string::npos)
	{
		temp.path = str.substr(0, pos);
		str.erase(0, pos + 1);
	}

	//Filename
	pos = str.find_last_of(".");
	if (pos != string::npos)
	{
		temp.filename = str.substr(0, pos);
		str.erase(0, pos + 1);
	}
	else if(temp.path.empty()) //Check back for missed Path
	{
		pos = str.find_first_of("?#");
		if(pos != string::npos)
		{
			temp.path = str.substr(0, pos);
			str.erase(0, pos);
		}
	}

	//Extension
	pos = str.find_first_of("?#");
	if (!temp.filename.empty())
	{
		if (pos != string::npos)
		{
			temp.extension = str.substr(0, pos);
			str.erase(0, pos);
		}
		else
		{
			temp.extension = str;
			str.clear();
			return temp;
		}
	}
	else if(str.length() > 0 && !temp.path.empty()) //Check back for missed Path
	{
		if (pos != string::npos)
		{
			temp.path += "/"+str.substr(0, pos);
			str.erase(0, pos);
		}
		else
		{
			temp.path += "/"+str;
			str.clear();
			return temp;
		}
	}

	//Querystring
	if (str[0] == '?')
	{
		pos = str.find("#");
		if (pos != string::npos)
		{
			temp.querystring = str.substr(1, pos-1);
			str.erase(0, pos);
		}
		else
		{
			temp.querystring = str.substr(1);
			str.clear();
			return temp;
		}
	}

	//Anchor
	if (str[0] == '#')
	{
		temp.anchor = str.substr(1);
		str.clear();
	}

	return temp;
}

bool check(rule r) {
	return true;
}

bool check(regex r) {
	return true;
}
