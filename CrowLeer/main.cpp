#include <iostream>
#include <thread>

//Other project headers
#include "uri.h"
#include "utils.h"
#include "getopt.h"

using std::cout; using std::cin; using std::endl;
using std::thread;

void doWork(unordered_set<string>& urls, queue<uri>& todo, uri base)
{
	string url;
	string response;
	uri actual;
	bool oktoread;
	bool download;

	lock.lock();
		oktoread = !todo.empty();
	lock.unlock();

	while (oktoread)
	{
		download = false;

		lock.lock();
			if (todo.front().domain == base.domain)
			{
				download = true;
				actual = todo.front();
				url = actual.tostring();
				cout << todo.size() << " >> " << url << endl;
			}
			todo.pop();
		lock.unlock();

		if (download)
		{
			response = HTTPrequest(url);
			crawl(response, urls, todo, &actual);
		}
		
		lock.lock();
			oktoread = !todo.empty();
		lock.unlock();
	}
}

int main(int argc, char *argv[])
{
	//Variable for the command line options management
	char opt;
	int index;

	//Number of threads used for crawling initialized with its default value
	int thrnum = 10;

	//Variables to initialize
	string url;
	int depth;

	//Command argument options acquisition and management
	while ((opt = getopt(argc, argv, ":hu:t:d:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			cout << "CrowLeer v0.1\nFast and reliable CLI web crawler\n\nUsage: crowleer [hutd]\n\nOptions:\n  -h\tView this help page\n  -u\tURL used to start crawling\n  -t\tNumber of threads used\n  -d\tMaximum crawling depth" << endl;
			cin.get();
			return 0;
			break;
		case 'u':
			cout << "Selected URL: " << optarg << endl;
			url.append(optarg);
			break;
		case 't':
			cout << "Threads number: " << optarg << endl;
			thrnum = atoi(optarg);
			break;
		case 'd':
			cout << "Maximum depth: " << optarg << endl;
			depth = atoi(optarg);
			break;
		case ':':
			cout << "Missing value for option -" << (char)optopt  << endl;
			cin.get();
			return 0;
			break;
		case '?':
		default:
			cout << "Unknown option -" << (char)optopt << endl;
			cin.get();
			return 0;
			break;
		}
	}

	if (url.empty())
	{
		cout << "URL: ";
		cin >> url;
	}

	cout << endl;

	url = validate(url);
	
	string response; //Contains HTTP response

	response = HTTPrequest(url);

	uri base(url);

	unordered_set<string> urls; //Hash table which contains the URLs found in the response
	queue<uri> todo; //Queue containing the urls left to crawl

	crawl(response, urls, todo, &base);

	thread *threads = new thread[thrnum];

	for (int i = 0; i < thrnum; i++)
	{
		threads[i] = std::thread(doWork, std::ref(urls), std::ref(todo), base);
	}

	for (int i = 0; i < thrnum; i++)
	{
		threads[i].join();
	}

	cout << "\nEnded";

	cin.ignore();
	cin.get();
	return 0;
}