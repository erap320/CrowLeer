#pragma once
#include <string>
#include <unordered_set>
#include <queue>
#include <mutex>

#include "uri.h"

using std::string;
using std::unordered_set;
using std::queue;
using std::mutex;

extern mutex lock;

//Function used by CURL to add chunks of data to the response string
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

//Make an HTTP request with CURL
std::string HTTPrequest(std::string url);

//View every item in the data structure
void debug_out(const unordered_set<string>& data);

//Save string in file
bool debug_file(const string& str, string path);

//Finds the position of the next valid "href" string, that leads to a real URL
int findhref(const string& response, int offset);

//Search for hrefs in the response, verify if it was already found elsewhere and eventually push it in the todo queue
void crawl(const string& response, unordered_set<string>& data, queue<uri>& todo, uri* const parent = nullptr);

//Fixes initial urls to make them uri parsing compliant
string validate(string url);