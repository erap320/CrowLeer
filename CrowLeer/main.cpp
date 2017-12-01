#include <iostream>
#include <thread>

//Other project headers
#include "uri.h"
#include "utils.h"
#include "getopt.h"

using std::cout; using std::cin; using std::endl;
using std::thread;

//Number of threads used for crawling initialized with its default value
int thrnum = 10;

//Variables to initialize
string url;
int maxdepth = 0;
rule crawlCondition; //conditions to choose what to crawl

void doWork(unordered_set<string>& urls, queue<uri>& todo, uri base)
{
	string url;
	string response;
	uri current;
	bool oktoread = true;
	bool follow;

	while (oktoread)
	{
		follow = false;

		queueMutex.lock();
			oktoread = !todo.empty();
			if (oktoread)
			{
				current = todo.front();
				if (current.check(crawlCondition) && (maxdepth==0 ? true : (current.depth<=maxdepth)) )
				{
					follow = true;
					url = current.tostring();
					cout << todo.size() << " >> " << url << " : " << current.depth << endl;
				}
				todo.pop();
			}
		queueMutex.unlock();

		if (follow)
		{
			response = HTTPrequest(url);
			crawl(response, urls, todo, &current);
		}
	}
}

int main(int argc, char *argv[])
{
	//Variable for the command line options management
	char opt=0;

	/* getopt_long stores the option index here. */
	static struct option long_options[] =
	{
		{ "help",		no_argument,		0,	'h' },
		{ "url",		required_argument,	0,	'u' },
		{ "threads",	required_argument,	0,	't' },
		{ "depth",		required_argument,	0,	'd' },
		{ 0, 0, 0, 0 }
	};

	while (opt != -1)
	{
		int option_index = 0;

		opt = getopt_long(argc, argv, "hu:t:d:",
			long_options, &option_index);

		/* Detect the end of the options. */
		if (opt == -1)
			break;

		switch (opt)
		{
		case 'h':
			cout << "CrowLeer v0.1\nFast and reliable CLI web crawler\n\nUsage: crowleer [options]\n\nOptions:\n  -h --help\tView this help page\n  -u --url\tURL used to start crawling\n  -t --thread\tNumber of threads used\n  -d --depth\tMaximum crawling depth" << endl;
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
			maxdepth = atoi(optarg);
			break;
		case ':':
			cout << "Missing value for option -" << (char)optopt << endl;
			cin.get();
			return 0;
			break;

		case '?':
		default:
			cin.ignore();
			return 0;
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

	cout << "\nCrawling completed";

	cin.ignore();
	cin.get();
	return 0;
}