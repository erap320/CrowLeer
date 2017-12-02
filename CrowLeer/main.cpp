#include <iostream>
#include <thread>

//Other project headers
#include "uri.h"
#include "utils.h"
#include "getopt.h"

#define HELP_MSG "CrowLeer v0.1\nFast and reliable CLI web crawler with focus on pages download\n\n>>USAGE: crowleer [options]\n\n>>OPTIONS:\n  -h --help\tView this help page\n  -u --url\tURL used to start crawling\n  -t --thread\tNumber of threads used\n  -d --depth\tMaximum crawling depth\n\n>>RULES:\nCrowLeer uses Regular Expressions (https://en.wikipedia.org/wiki/Regular_expression) to apply conditions to URLs or parts of URLs.\nBoth rules have a global component, that matches the Completed URL (see the URL section) and one for every URL part.\nThere are two rules: Follow Rule and Save Rule.\n  - Follow Rule: describes pages to follow while crawling\n  - Save Rule: describes pages to download and save locally\nIf not specified every rule is set to match everything. You can set every possible composition of rules to describe the exact scenario you need, including global rule and parts rules together.\n\n>>URLs:\nCrowLeer completes the URLs found in the crawled pages to make its and your work easier.\nEvery URL is split in parts and completed with parts from the URL of the page it was found in if necessary.\nThe parts in which a URL is split are: protocol, domain, path, filename, extension, querystring and anchor.\n\nExample: the URL \"/example/one/file.txt\" was found while running on \"https://erap.space\"\n  The Completed URL will be \"https://erap.space/example/one/file.txt\", and its parts will be:\n  - protocol: \"https\"\n  - domain: \"erap.space\"\n  - path: \"example/one\"\n  - filename: \"file\"\n  - extension: \"txt\"\n  - querystring: \"\"\n  - anchor: \"\"\n\nExample: the URL \"https://en.wikipedia.org/wiki/Dog?s=canis#Origin\" will be split in parts this way:\n  - protocol: \"https\"\n  - domain: \"en.wikipedia.org\"\n  - path: \"wiki/Dog\"\n  - filename: \"\"\n  - extension: \"\"\n  - querystring: \"s=canis\"\n  - anchor: \"Origin\""

using std::cout; using std::cin; using std::endl;
using std::thread;

//Number of threads used for crawling initialized with its default value
int thrnum = 10;

//Variables to initialize
string url;
int maxdepth = 0;
rule followCondition; //conditions to choose what to crawl
rule saveCondition; //condition to choose what to download

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
				if (current.check(followCondition) && (maxdepth==0 ? true : (current.depth<=maxdepth)) )
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
		{ "help",			no_argument,		0,	'h' },
		{ "url",			required_argument,	0,	'u' },
		{ "threads",		required_argument,	0,	't' },
		{ "depth",			required_argument,	0,	'd' },
		{ "f-global",		required_argument,	0,	'f' },
		{ "f-protocol",		required_argument,	0,	'f' },
		{ "f-domain",		required_argument,	0,	'f' },
		{ "f-path",			required_argument,	0,	'f' },
		{ "f-filename",		required_argument,	0,	'f' },
		{ "f-extension",	required_argument,	0,	'f' },
		{ "f-querystring",	required_argument,	0,	'f' },
		{ "f-anchor",		required_argument,	0,	'f' },
		{ "g-global",		required_argument,	0,	'g' },
		{ "g-protocol",		required_argument,	0,	'g' },
		{ "g-domain",		required_argument,	0,	'g' },
		{ "g-path",			required_argument,	0,	'g' },
		{ "g-filename",		required_argument,	0,	'g' },
		{ "g-extension",	required_argument,	0,	'g' },
		{ "g-querystring",	required_argument,	0,	'g' },
		{ "g-anchor",		required_argument,	0,	'g' },
		{ 0, 0, 0, 0 }
	};

	while (opt != -1)
	{
		int option_index = 0;

		opt = getopt_long(argc, argv, "hu:t:d:f:g:",
			long_options, &option_index);

		/* Detect the end of the options. */
		if (opt == -1)
			break;

		switch (opt)
		{
		case 'h':
			cout << "\n\n" << HELP_MSG << endl;
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
		case 'f':
			if (long_options[option_index].name == "f-global")
			{
				followCondition.global = optarg;
				cout << "Global Follow rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "f-protocol")
			{
				followCondition.protocol = optarg;
				cout << "Protocol Follow rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "f-domain")
			{
				followCondition.domain = optarg;
				cout << "Domain Follow rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "f-path")
			{
				followCondition.path = optarg;
				cout << "Path Follow rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "f-filename")
			{
				followCondition.filename = optarg;
				cout << "Filename Follow rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "f-extension")
			{
				followCondition.extension = optarg;
				cout << "Extension Follow rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "f-querystring")
			{
				followCondition.querystring = optarg;
				cout << "Querystring Follow rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "f-anchor")
			{
				followCondition.anchor = optarg;
				cout << "Anchor Follow rule: " << optarg << endl;
			}
			break;
		case 'g':
			if (long_options[option_index].name == "g-global")
			{
				saveCondition.global = optarg;
				cout << "Global Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "g-protocol")
			{
				saveCondition.protocol = optarg;
				cout << "Protocol save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "g-domain")
			{
				saveCondition.domain = optarg;
				cout << "Domain Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "g-path")
			{
				saveCondition.path = optarg;
				cout << "Path Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "g-filename")
			{
				saveCondition.filename = optarg;
				cout << "Filename Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "g-extension")
			{
				saveCondition.extension = optarg;
				cout << "Extension Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "g-querystring")
			{
				saveCondition.querystring = optarg;
				cout << "Querystring Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "g-anchor")
			{
				saveCondition.anchor = optarg;
				cout << "Anchor Save rule: " << optarg << endl;
			}
			break;
		case ':':
			cout << "Missing value for option -" << (char)optopt << endl;
			return 0;
			break;

		case '?':
		default:
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
	return 0;
}