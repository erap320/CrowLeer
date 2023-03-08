#ifndef TIMESTAMP_HPP
#define TIMESTAMP_HPP

#include <ctime>
#include <string>

using std::string;

//Gives a timestamp in string form
string timestamp()
{
	char str[15];
	std::time_t now = std::time(nullptr);
	std::strftime(str, sizeof(str), "%d/%m/%y %H:%M", std::localtime(&now));
	return string(str);
}

//Gives a filename-safe timestamp
string timestamp_safe()
{
	char str[15];
	std::time_t now = std::time(nullptr);
	std::strftime(str, sizeof(str), "%d-%m-%y_%H.%M", std::localtime(&now));
	return string(str);
}

//Gives the current date in dd/mm/yy format
string date()
{
	char str[15];
	std::time_t now = std::time(nullptr);
	std::strftime(str, sizeof(str), "%d/%m/%y", std::localtime(&now));
	return string(str);
}

#endif