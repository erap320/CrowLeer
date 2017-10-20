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

	unordered_set<string> urls; //Hash table which contains the URLs found in the response
	queue<string> todo; //Queue containing the urls left to crawl

	crawl(response, urls, todo);

	/*while (!todo.empty())
	{
		url = todo.front();
		todo.pop();
		cout << url << endl;
		response = HTTPrequest(url);
		crawl(response, urls, todo);
	}*/

	uri test(url);
	test.debug();
	cout << endl << endl;

	for (auto i = urls.begin(); i != urls.end(); i++)
	{
		if (i->find("photos") != string::npos)
		{
			cout << *i << endl;
			parse(*i).debug();
			cout << endl;
			cout << parse(*i, &test).tostring() << endl;
		}
	}


	cout << "\nEnded";
	cin.ignore();
	cin.get();
	return 0;
}