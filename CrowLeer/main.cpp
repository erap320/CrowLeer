#include <iostream>
#include <thread>

//Other project headers
#include "uri.h"
#include "utils.h"

using std::cout; using std::cin; using std::endl;
using std::thread;

void doWork(unordered_set<string>& urls, queue<uri>& todo, uri actual)
{
	string url = actual.tostring();
	lock.lock();
	cout << todo.size() << " >> " << url << endl;
	lock.unlock();
	string response = HTTPrequest(url);
	crawl(response, urls, todo, &todo.front());
}

int main(void)
{
	string url;
	int depth;
	cout << "URL: ";
	cin >> url;
	url = validate(url);



	string response; //Contains HTTP response

	response = HTTPrequest(url);

	uri base(url);

	unordered_set<string> urls; //Hash table which contains the URLs found in the response
	queue<uri> todo; //Queue containing the urls left to crawl

	crawl(response, urls, todo, &base);

	thread threads[3];

	while (!todo.empty())
	{
		lock.lock();
		if (todo.front().domain == base.domain)
			threads[1] = std::thread(doWork, std::ref(urls), std::ref(todo), todo.front());
		todo.pop();
		lock.unlock();
		lock.lock();
		if (todo.front().domain == base.domain)
			threads[2] = std::thread(doWork, std::ref(urls), std::ref(todo), todo.front());
		todo.pop();
		lock.unlock();
		lock.lock();
		if (todo.front().domain == base.domain)
			threads[3] = std::thread(doWork, std::ref(urls), std::ref(todo), todo.front());
		todo.pop();
		lock.unlock();

		threads[1].join();
		threads[2].join();
		threads[3].join();
	}

	cout << "\nEnded";
	cin.ignore();
	cin.get();
	return 0;
}