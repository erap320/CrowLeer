#pragma once

#include <string>
#include <unordered_set>
#include <queue>
#include <vector>
#include <mutex>
#include <filesystem>
#include <map>
#include <fstream>

#include "uri.hpp"

#define CURL_STATICLIB
#include "curl.h"

namespace fs = std::experimental::filesystem;

using std::string;
using std::unordered_set;
using std::queue;
using std::mutex;

//Custom CURL option
struct curl_option {
	string name;
	string parameter;
};

extern mutex queueMutex;
extern mutex consoleMutex;
extern std::vector<curl_option> options;
extern std::map<string, CURLoption> optcode;

void curl_options_init();

CURLoption curl_option_value(string option);

//Handles all console and file output
class Output
{
public:
	~Output();

	void useLog(string name);

	std::ofstream log;
};

Output& operator <<(Output &out, string s);
Output& operator <<(Output &out, int i);

extern Output out;

//Function used by CURL to add chunks of data to the response string
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

//Make an HTTP request with CURL
std::string HTTPrequest(std::string url);

//View every item in the data structure
void debug_out(const unordered_set<string>& data);

//Save string in file
bool writeToDisk(const string& str, fs::path path);

//Finds the position of the next valid "href" string, that leads to a real URL
int findhref(const string& response, int offset);

//Search for hrefs in the response, verify if it was already found elsewhere and eventually push it in the todo queue
//Excluded or too deep urls aren't pushed to the queue
void crawl(const string& response, unordered_set<string>& data, queue<uri>& todo, bool saveflag, std::regex excludeCondition, int maxdepth, uri* const parent = nullptr);

//Fixes initial urls to make them uri parsing compliant
string validate(string url);

//Outputs error text to console
void error_out(string s);

//Outputs text with a special color if the flag is raised
//Only to be used inside a mutex block!
void special_out(string s, bool color);