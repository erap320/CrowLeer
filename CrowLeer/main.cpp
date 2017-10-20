#include <iostream>

//Other project headers
#include "uri.h"
#include "utils.h"

using std::cout; using std::cin; using std::endl;

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

	uri newbase;
	for (auto it=urls.begin(); it!=urls.end(); it++)
	{
		newbase = parse(*it, &base);
		response = HTTPrequest( newbase.tostring() );
		crawl(response, urls, todo, &newbase);
	}

	/*while (!todo.empty())
	{
		url = todo.front();
		todo.pop();
		cout << url << endl;
		response = HTTPrequest(url);
		crawl(response, urls, todo);
	}*/

	/*while (!todo.empty())
	{
		cout << todo.front().tostring() << endl;
		todo.pop();
	}*/

	cout << "\nEnded";
	cin.ignore();
	cin.get();
	return 0;
}