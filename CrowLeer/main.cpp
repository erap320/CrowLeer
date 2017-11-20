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

		response = HTTPrequest(url);
		crawl(response, urls, todo, &actual);
		
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

	while ((opt = getopt(argc, argv, ":t:")) != -1)
	{
		switch (opt)
		{
		case 't':
			thrnum = atoi(optarg);
			break;
		default: 
			cout << "Arguments error" << endl;
			cin.get();
			return 0;
			break;
		}

		for (index = optind; index < argc; index++)
			cout << "Default argument: " << argv[index] << endl;
	}

	cout << "Thread number: " << thrnum << endl;


	//string url;
	//int depth;
	//cout << "URL: ";
	//cin >> url;
	//url = validate(url);
	//
	//string response; //Contains HTTP response

	//response = HTTPrequest(url);

	//uri base(url);

	//unordered_set<string> urls; //Hash table which contains the URLs found in the response
	//queue<uri> todo; //Queue containing the urls left to crawl

	//crawl(response, urls, todo, &base);

	//thread threads[THRNUM];

	//for (int i = 0; i < THRNUM; i++)
	//{
	//	threads[i] = std::thread(doWork, std::ref(urls), std::ref(todo), base);
	//}

	//for (int i = 0; i < THRNUM; i++)
	//{
	//	threads[i].join();
	//}

	//cout << "\nEnded";

	cin.ignore();
	cin.get();
	return 0;
}