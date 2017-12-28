#include <iostream>
#include <thread>

//Other project headers
#include "uri.hpp"
#include "utils.hpp"
#include "getopt.h"

#define HELP_MSG "CrowLeer v0.1\nFast and reliable CLI web crawler with focus on pages download\n\n>>USAGE: crowleer [options]\n\n>>OPTIONS:\n  -h --help\t\tView this help page\n  -u --url\t\tURL used to start crawling\n  -t --thread\t\tNumber of threads used\n  -d --depth\t\tMaximum crawling depth (the starting URL spcified in -u is at depth 0)\n  -x --same-domain\t\tQuick flag to only follow URLs with the same domain as the starting URL, overrides the --f-domain option\n  -S --save\t\tActivates the download functionality of CrowLeer. If not used nothing will be saved on the disk\n  -o --output\t\tChoose a directory where the selected files will be saved. The default value is the current directory\n  --f-global\t\tFollow rule to be tested on the whole parsed URL\t\t\n  --f-protocol\t\tFollow rules on single parts of parsed URLs\n  --f-domain\n  --f-path\n  --f--filename\n  --f-extension\n  --f-querystring\t\n  --f-anchor\n  --s-global\t\tSave rule to be tested on the whole parsed URL\t\t\n  --s-protocol\t\tSave rules on single parts of parsed URLs\n  --s-domain\n  --s-path\n  --s--filename\n  --s-extension\n  --s-querystring\n  --s-anchor\n\n>>RULES:\nCrowLeer uses Regular Expressions (https://en.wikipedia.org/wiki/Regular_expression) to apply conditions to URLs or parts of URLs.\nBoth rules have a global component, that matches the Completed URL (see the URL section) and one for every URL part.\nThere are two rules: Follow Rule and Save Rule.\n  - Follow Rule: describes pages to follow while crawling\n  - Save Rule: describes pages to download and save locally\nIf not specified every rule is set to match everything. You can set every possible composition of rules to describe the exact scenario you need, including global rule and parts rules together.\n\n>>URLs:\nCrowLeer completes the URLs found in the crawled pages to make its and your work easier.\nEvery URL is split in parts and completed with parts from the URL of the page it was found in if necessary.\nThe parts in which a URL is split are: protocol, domain, path, filename, extension, querystring and anchor.\n\nExample: the URL \"/example/one/file.txt\" was found while running on \"https://erap.space\"\n  The Completed URL will be \"https://erap.space/example/one/file.txt\", and its parts will be:\n  - protocol: \"https\"\n  - domain: \"erap.space\"\n  - path: \"example/one\"\n  - filename: \"file\"\n  - extension: \"txt\"\n  - querystring: \"\"\n  - anchor: \"\"\n\nExample: the URL \"https://en.wikipedia.org/wiki/Dog?s=canis#Origin\" will be split in parts this way:\n  - protocol: \"https\"\n  - domain: \"en.wikipedia.org\"\n  - path: \"wiki/Dog\"\n  - filename: \"\"\n  - extension: \"\"\n  - querystring: \"s=canis\"\n  - anchor: \"Origin\""

using std::cout; using std::cin; using std::endl;
using std::thread;

//Number of threads used for crawling initialized with its default value
int thrnum = 10;

//Variables to initialize
string url;
int maxdepth = 0;
rule followCondition; //conditions to choose what to crawl
rule saveCondition; //condition to choose what to download
bool save = false; //flag to activate the saving of files, changed with the -S option
//String of the path where to save files
string pathString;

void doWork(unordered_set<string>& urls, queue<uri>& todo, uri base)
{
	string url;
	string response;
	uri current;
	bool oktoread = true;
	bool follow;
	bool download;
	fs::path directory;

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
					consoleMutex.lock();
						cout << todo.size() << " >> " << url << " : " << current.depth << endl;
					consoleMutex.unlock();
				}
				todo.pop();
			}
		queueMutex.unlock();

		download = save && current.check(saveCondition);

		//Save the file in the correct directory
		if (follow || download)
		{
			response = HTTPrequest(url);

			if (follow)
				crawl(response, urls, todo, save, &current);
			if (download)
			{
				directory = pathString;
				directory /= std::regex_replace(current.domain, regex(":|\\*|\\?|\"|<|>|\\|"), "");
				directory /= std::regex_replace(current.path, regex(":|\\*|\\?|\"|<|>|\\|"), "");
				if (!fs::exists(directory))
				{
					if (fs::create_directories(directory))
					{
						if (current.filename.empty())
							directory /= "index.html";
						else
							directory /= current.filename + "." + current.extension;
						writeToDisk(response, directory);
					}
					else
					{
						error_out("Could not create folder for URL " + url);
					}
				}
			}
		}

	}
}

int main(int argc, char *argv[])
{
	//Condition to use the -s flag
	bool sameDomain = false;

	//Variable for the command line options management
	char opt=0;

	//Default value of the saved files path
	fs::path directory;
	pathString = fs::current_path().string();

	/* getopt_long stores the option index here. */
	static struct option long_options[] =
	{
		{ "help",			no_argument,		0,	'h' },
		{ "url",			required_argument,	0,	'u' },
		{ "threads",		required_argument,	0,	't' },
		{ "depth",			required_argument,	0,	'd' },
		{ "same-domain",	required_argument,	0,	'x' },
		{ "save",			required_argument,	0,	'S' },
		{ "output",			required_argument,	0,	'o' },
		{ "f-global",		required_argument,	0,	'f' },
		{ "f-protocol",		required_argument,	0,	'f' },
		{ "f-domain",		required_argument,	0,	'f' },
		{ "f-path",			required_argument,	0,	'f' },
		{ "f-filename",		required_argument,	0,	'f' },
		{ "f-extension",	required_argument,	0,	'f' },
		{ "f-querystring",	required_argument,	0,	'f' },
		{ "f-anchor",		required_argument,	0,	'f' },
		{ "s-global",		required_argument,	0,	's' },
		{ "s-protocol",		required_argument,	0,	's' },
		{ "s-domain",		required_argument,	0,	's' },
		{ "s-path",			required_argument,	0,	's' },
		{ "s-filename",		required_argument,	0,	's' },
		{ "s-extension",	required_argument,	0,	's' },
		{ "s-querystring",	required_argument,	0,	's' },
		{ "s-anchor",		required_argument,	0,	's' },
		{ 0, 0, 0, 0 }
	};

	while (opt != -1)
	{
		int option_index = 0;

		opt = getopt_long(argc, argv, "hu:xSo:t:d:f:s:", long_options, &option_index);

		/* Detect the end of the options. */
		if (opt == -1)
			break;

		switch (opt)
		{
		case 'h':
		{
			cout << HELP_MSG << endl;
			return 0;
			break;
		}
		case 'u':
		{
			cout << "Selected URL: " << optarg << endl;
			url.append(optarg);
			break;
		}
		case 't':
		{
			cout << "Threads number: " << optarg << endl;
			thrnum = atoi(optarg);
			break;
		}
		case 'd':
		{
			cout << "Maximum depth: " << optarg << endl;
			maxdepth = atoi(optarg);
			break;
		}
		case 'x':
		{
			sameDomain = true;
			cout << "Same domain rule applied" << endl;
			break;
		}
		case 'S':
		{
			save = true;
			cout << "Activate Save rule applied" << endl;
			break;
		}
		case 'o':
		{
			pathString.clear();
			pathString.append(optarg);
			cout << "Output directory for saved files changed to " << optarg << endl;
			break;
		}
		case 'f':
		{
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
		}
		case 's':
		{
			if (long_options[option_index].name == "s-global")
			{
				saveCondition.global = optarg;
				cout << "Global Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "s-protocol")
			{
				saveCondition.protocol = optarg;
				cout << "Protocol save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "s-domain")
			{
				saveCondition.domain = optarg;
				cout << "Domain Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "s-path")
			{
				saveCondition.path = optarg;
				cout << "Path Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "s-filename")
			{
				saveCondition.filename = optarg;
				cout << "Filename Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "s-extension")
			{
				saveCondition.extension = optarg;
				cout << "Extension Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "s-querystring")
			{
				saveCondition.querystring = optarg;
				cout << "Querystring Save rule: " << optarg << endl;
			}
			else if (long_options[option_index].name == "s-anchor")
			{
				saveCondition.anchor = optarg;
				cout << "Anchor Save rule: " << optarg << endl;
			}
			break;
		}
		case ':':
		{
			cout << "Missing value for option -" << (char)optopt << endl;
			return 0;
			break;
		}
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
	if (sameDomain)
		followCondition.domain = std::regex_replace(base.domain, regex("\\."), "\\.");

	unordered_set<string> urls; //Hash table which contains the URLs found in the response
	queue<uri> todo; //Queue containing the urls left to crawl

	cout << todo.size() << " >> " << url << " : " << base.depth << endl;

	//Check if the starting url has to be saved
	if (save && base.check(saveCondition))
	{
		directory = pathString;
		directory /= base.domain;
		directory /= base.path;
		if (!fs::exists(directory))
			fs::create_directories(directory);
		if (base.filename.empty())
			directory /= "index.html";
		else
			directory /= base.filename + "." + base.extension;
		writeToDisk(response, directory);
	}

	crawl(response, urls, todo, save, &base);

	thread *threads = new thread[thrnum];

	for (int i = 0; i < thrnum; i++)
	{
		threads[i] = std::thread(doWork, std::ref(urls), std::ref(todo), base);
	}

	for (int i = 0; i < thrnum; i++)
	{
		threads[i].join();
	}

	cout << "\nCrawling completed\n";

	return 0;
}