#include <iostream>
#include <thread>

//Other project headers
#include "uri.h"
#include "utils.h"
#include "getopt.h"

#define HELP_MSG "CrowLeer v0.1\nFast and reliable CLI web crawler\n\n>>USAGE: crowleer [options]\n\n>>OPTIONS:\n  -h --help\tView this help page\n  -u --url\tURL used to start crawling\n  -t --thread\tNumber of threads used\n  -d --depth\tMaximum crawling depth\n\n>>RULES:\nCrowLeer uses Regular Expressions (https://en.wikipedia.org/wiki/Regular_expression) to apply conditions to URLs or parts of URLs.\nBoth rules have a global component, that matches the Completed URL (see the URL section) and one for every URL part.\nThere are two rules: Follow Rule and Save Rule.\n  - Follow Rule: describes pages to follow while crawling\n  - Save Rule: describes pages to download and save locally\nIf not specified every rule is set to match everything. You can set every possible composition of rules to describe the exact scenario you need, including global rule and parts rules together.\n\n>>URLs:\nCrowLeer completes the URLs found in the crawled pages to make its and your work easier.\nEvery URL is split in parts and completed with parts from the URL of the page it was found in if necessary.\nThe parts in which a URL is split are: protocol, domain, path, filename, extension, querystring and anchor.\n\nExample: the URL \"/example/one/file.txt\" was found while running on \"https://erap.space\"\n  The Completed URL will be \"https://erap.space/example/one/file.txt\", and its parts will be:\n  - protocol: \"https\"\n  - domain: \"erap.space\"\n  - path: \"example/one\"\n  - filename: \"file\"\n  - extension: \"txt\"\n  - querystring: \"\"\n  - anchor: \"\"\n\nExample: the URL \"https://en.wikipedia.org/wiki/Dog?s=canis#Origin\" will be split in parts this way:\n  - protocol: \"https\"\n  - domain: \"en.wikipedia.org\"\n  - path: \"wiki/Dog\"\n  - filename: \"\"\n  - extension: \"\"\n  - querystring: \"s=canis\"\n  - anchor: \"Origin\""

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
			cout << "\n\n" << HELP_MSG << endl;
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
	if (optind < argc)
    {
		cout << "Illegal non-option arguments: ";
		while (optind < argc)
			cout << argv[optind++] << " ";
		cout << "\n\n" << HELP_MSG << endl;
		cin.get();
		return 0;
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