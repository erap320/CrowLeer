#include <iostream>
#include <thread>

#include <fstream>

//Other project headers
#include "uri.hpp"
#include "utils.hpp"
#include "getopt.h"

#define HELP_MSG "\
Fast and reliable CLI web crawler with focus on pages download\n\
For more information visit https://github.com/ERap320/CrowLeer\n\n\
>>USAGE: crowleer [options]\n\
\n\
>>OPTIONS:\n\
  -h --help\t\tView this help page\n\
  -u --url\t\tURL used to start crawling\n\
  -t --thread\t\tNumber of threads used\n\
  -d --depth\t\tMaximum crawling depth (the starting URL spcified in -u is at depth 0)\n\
  -x --same-domain\tQuick flag to only follow URLs with the same domain as the starting URL, overrides the --f-domain option\n\
  -S --save\t\tActivates the download functionality of CrowLeer. If not used nothing will be saved on the disk\n\
  -o --output\t\tChoose a directory where the selected files will be saved. The default value is the current directory\n\
  -l --log\t\tOutputs progress details to the specified log file. Works best if it's the first specified option, as it makes possible to log further settings of the current job\n\
  -c --curl-opt\t\tName of the custom CURL option to use when downloading pages. Only to use before the -p flag that specifies the parameter for the option. Can be used multiple times to set more than one option\n\
  -p --curl-param\tValue of the custom CURL option specified before it with the -c flag\n\
  --f-global\t\tFollow rule to be tested on the whole parsed URL\t\t\n\
  --f-protocol\t\tFollow rules on single parts of parsed URLs\n\
  --f-domain\n\
  --f-path\n\
  --f-filename\n\
  --f-extension\n\
  --f-querystring\t\n\
  --f-anchor\n\
  --s-global\t\tSave rule to be tested on the whole parsed URL\t\t\n\
  --s-protocol\t\tSave rules on single parts of parsed URLs\n\
  --s-domain\n\
  --s-path\n\
  --s-filename\n\
  --s-extension\n\
  --s-querystring\n\
  --s-anchor\n\
\n\
>>RULES:\n\
CrowLeer uses Regular Expressions (https://en.wikipedia.org/wiki/Regular_expression) to apply conditions to URLs or parts of URLs.\n\
Both rules have a global component, that matches the Completed URL (see the URL section) and one for every URL part.\n\
There are two rules: Follow Rule and Save Rule.\n\
  - Follow Rule: describes pages to follow while crawling\n\
  - Save Rule: describes pages to download and save locally\n\
If not specified every rule is set to match everything. You can set every possible composition of rules to describe the exact scenario you need, including global rule and parts rules together.\n\
\n\
>>URLs:\n\
CrowLeer completes the URLs found in the crawled pages to make its and your work easier.\n\
Every URL is split in parts and completed with parts from the URL of the page it was found in if necessary.\n\
The parts in which a URL is split are: protocol, domain, path, filename, extension, querystring and anchor.\n\
\n\
Example: the URL \"/example/one/file.txt\" was found while running on \"https://erap.space\"\n\
  The Completed URL will be \"https://erap.space/example/one/file.txt\", and its parts will be:\n\
  - protocol: \"https\"\n\
  - domain: \"erap.space\"\n\
  - path: \"example/one\"\n\
  - filename: \"file\"\n\
  - extension: \"txt\"\n\
  - querystring: \"\"\n\
  - anchor: \"\"\n\
\n\
Example: the URL \"https://en.wikipedia.org/wiki/Dog?s=canis#Origin\" will be split in parts this way:\n\
  - protocol: \"https\"\n\
  - domain: \"en.wikipedia.org\"\n\
  - path: \"wiki/Dog\"\n\
  - filename: \"\"\n\
  - extension: \"\"\n\
  - querystring: \"s=canis\"\n\
  - anchor: \"Origin\""

using std::cin;
using std::thread;

//Number of threads used for crawling initialized with its default value
int thrnum = 10;

//Variables to initialize
string url;
unsigned int maxdepth = -1; //Set to the biggest int possible because of two's complement representation
rule followCondition; //conditions to choose what to crawl
rule saveCondition; //condition to choose what to download
regex excludeCondition; //condition to exclude certain URLs, like a negative global follow condition
bool save = false; //flag to activate the saving of files, changed with the -S option

//String of the path where to save files
string pathString;

//Path of the log file set with -l
string logPath;

//Option struct used to store curl options until pushed
curl_option temp_option;

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
		download = false;

		queueMutex.lock();
			oktoread = !todo.empty();
			if (oktoread)
			{
				current = todo.front();
				if (current.check(followCondition)) //Other rules are checked before inserting in the todo queue
				{
					follow = true;
					url = current.tostring();
					download = save && current.check(saveCondition);
					consoleMutex.lock();
						out << todo.size() << " >> ";
						special_out(url, download);
						out << " : " << current.depth << "\n";
					consoleMutex.unlock();
				}
				todo.pop();
			}
		queueMutex.unlock();

		//Save the file in the correct directory
		if (oktoread && (follow || download) )
		{
			response = HTTPrequest(url);

			crawl(response, urls, todo, save, excludeCondition, maxdepth, &current);

			if (download)
			{
				directory = pathString;
				directory /= std::regex_replace(current.domain, regex(":|\\*|\\?|\"|<|>|\\|"), "");
				directory /= std::regex_replace(current.path, regex(":|\\*|\\?|\"|<|>|\\|"), "");

				if (!fs::exists(directory))
				{
					try {
						fs::create_directories(directory);

						if (current.filename.empty())
							directory /= "index.html";
						else
							directory /= current.filename + "." + current.extension;

						writeToDisk(response, directory);
					}
					catch (fs::filesystem_error e) {
						error_out( (string)e.what() + " : Could not create folder for URL " + url);
					}
				}
				else
				{
					if (current.filename.empty())
						directory /= "index.html";
					else
						directory /= current.filename + "." + current.extension;

					writeToDisk(response, directory);
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	out << "CrowLeer 1.6 by ERap320 [battistonelia@erap.space]\n\n";

	//Used to initialize custom curl options map
	curl_options_init();

	//Condition to use the -s flag
	bool sameDomain = false;

	//Variable for the command line options management
	char opt=0;

	//Default value of the saved files path
	fs::path directory;
	pathString = fs::current_path().string();

	//Default value for the excludeCondition, not to match anything
	excludeCondition = "(?!)";

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
		{ "log",			required_argument,	0,	'l' },
		{ "exclude",		required_argument,	0,	'e' },
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
		{ "curl-opt",		required_argument,	0,	'c' },
		{ "curl-param",		required_argument,	0,	'p' },
		{ 0, 0, 0, 0 }
	};

	while (opt != -1)
	{
		int option_index = 0;

		opt = getopt_long(argc, argv, "hu:xSo:l:e:t:d:f:s:c:p:", long_options, &option_index);

		/* Detect the end of the options. */
		if (opt == -1)
			break;

		switch (opt)
		{
		case 'h':
		{
			out << HELP_MSG << "\n";
			return 0;
			break;
		}
		case 'u':
		{
			out << "Selected URL: " << optarg << "\n";
			url.append(optarg);
			break;
		}
		case 't':
		{
			out << "Threads number: " << optarg << "\n";
			thrnum = atoi(optarg);
			break;
		}
		case 'd':
		{
			out << "Maximum depth: " << optarg << "\n";
			maxdepth = atoi(optarg);
			break;
		}
		case 'x':
		{
			sameDomain = true;
			out << "Same domain rule applied\n";
			break;
		}
		case 'S':
		{
			save = true;
			out << "Activate Save rule applied\n";
			break;
		}
		case 'o':
		{
			pathString.clear();
			pathString.append(optarg);
			pathString = std::regex_replace(pathString, regex("\\*|\\?|\"|<|>|\\|"), "");
			out << "Output directory for saved files changed to " << std::regex_replace(optarg, regex("\\*|\\?|\"|<|>|\\|"), "") << "\n";
			break;
		}
		case 'l':
		{
			logPath = optarg;
			logPath = std::regex_replace(logPath, regex("\\*|\\?|\"|<|>|\\|"), "");
			out.useLog(logPath);
			out << "Log file set to " << std::regex_replace(optarg, regex("\\*|\\?|\"|<|>|\\|"), "") << "\n";
			break;
		}
		case 'e':
		{
			excludeCondition = optarg;
			out << "Exclude rule: " << optarg << "\n";
			break;
		}
		case 'f':
		{
			if (long_options[option_index].name == "f-global")
			{
				followCondition.global = optarg;
				out << "Global Follow rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "f-protocol")
			{
				followCondition.protocol = optarg;
				out << "Protocol Follow rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "f-domain")
			{
				followCondition.domain = optarg;
				out << "Domain Follow rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "f-path")
			{
				followCondition.path = optarg;
				out << "Path Follow rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "f-filename")
			{
				followCondition.filename = optarg;
				out << "Filename Follow rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "f-extension")
			{
				followCondition.extension = optarg;
				out << "Extension Follow rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "f-querystring")
			{
				followCondition.querystring = optarg;
				out << "Querystring Follow rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "f-anchor")
			{
				followCondition.anchor = optarg;
				out << "Anchor Follow rule: " << optarg << "\n";
			}
			break;
		}
		case 's':
		{
			if (long_options[option_index].name == "s-global")
			{
				saveCondition.global = optarg;
				out << "Global Save rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "s-protocol")
			{
				saveCondition.protocol = optarg;
				out << "Protocol save rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "s-domain")
			{
				saveCondition.domain = optarg;
				out << "Domain Save rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "s-path")
			{
				saveCondition.path = optarg;
				out << "Path Save rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "s-filename")
			{
				saveCondition.filename = optarg;
				out << "Filename Save rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "s-extension")
			{
				saveCondition.extension = optarg;
				out << "Extension Save rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "s-querystring")
			{
				saveCondition.querystring = optarg;
				out << "Querystring Save rule: " << optarg << "\n";
			}
			else if (long_options[option_index].name == "s-anchor")
			{
				saveCondition.anchor = optarg;
				out << "Anchor Save rule: " << optarg << "\n";
			}
			break;
		}
		case 'c':
		{
			if (!temp_option.name.empty())
			{
				out << "Trying to set the " << optarg << "option without giving a parameter for " << temp_option.name  << "\n";
				return 0;
			}
			if (optcode.find(string(optarg)) == optcode.end())
			{
				out << optarg << " is not a CURL option\n";
				return 0;
			}
			if(curl_option_value(string(optarg))/1000 > 1 ) //See HTTPrequest definition in utils.cpp
			{
				out << "Unsupported custom CURL option " << optarg << ", please contact the developer at battistonelia@erap.space about this issue\n";
				return 0;
			}
				
			temp_option.name = optarg;

			break;
		}
		case 'p':
		{
			if (temp_option.name.empty())
			{
				out << "No option name specified before '" << optarg << "'\n";
				return 0;
			}

			temp_option.parameter = optarg;
			options.push_back(temp_option);
			out << "CURL option: " << temp_option.name << "='" << temp_option.parameter << "'\n";
			temp_option.name.clear();
			temp_option.parameter.clear();
			break;
		}
		case ':':
		{
			out << "Missing value for option -" << (char)optopt << "\n";
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
		out << "Illegal non-option arguments: ";
		while (optind < argc)
			out << argv[optind++] << " ";
		out << "\n\n" << HELP_MSG << "\n";
		return 0;
    }

	if (url.empty())
	{
		out << "\n\n" << HELP_MSG << "\n";
		return 0;
	}

	out << "\n";

	url = validate(url);
	
	string response; //Contains HTTP response

	response = HTTPrequest(url);

	uri base(url);
	if (sameDomain) {
		followCondition.domain = std::regex_replace(base.domain, regex("\\."), "\\.");
		saveCondition.domain = followCondition.domain;
	}

	unordered_set<string> urls; //Hash table which contains the URLs found in the response
	queue<uri> todo; //Queue containing the urls left to crawl

	out << todo.size() << " >> ";
	special_out(url, save && base.check(saveCondition));
	out << " : " << base.depth << "\n";

	//Check if the starting url has to be saved
	if (save && base.check(saveCondition))
	{
		directory = pathString;
		directory /= base.domain;
		directory /= base.path;

		if (!fs::exists(directory))
		{
			try {
				fs::create_directories(directory);
			}
			catch(fs::filesystem_error e){
				error_out( (string)e.what() + " : An error occurred while creating the output directory. Make sure the path is valid and check the folders' permissions");
				return 1;
			}
		}
		if (base.filename.empty())
			directory /= "index.html";
		else
			directory /= base.filename + "." + base.extension;
		writeToDisk(response, directory);
	}

	crawl(response, urls, todo, save, excludeCondition, maxdepth, &base);

	thread *threads = new thread[thrnum];

	for (int i = 0; i < thrnum; i++)
	{
		threads[i] = std::thread(doWork, std::ref(urls), std::ref(todo), base);
	}

	for (int i = 0; i < thrnum; i++)
	{
		threads[i].join();
	}

	out << "\nCrawling completed\n";

	return 0;
}