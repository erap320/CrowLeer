#include <iostream>

//Other project headers
#include "uri.h"
#include "utils.h"

using std::cout; using std::cin; using std::endl;

int main(void)
{
	//string url;
	//cout << "URL: ";
	//cin >> url;

	//string response; //Contains HTTP response

	//response = HTTPrequest(url);

	//unordered_set<string> urls; //Hash table which contains the URLs found in the response
	//queue<string> todo; //Queue containing the urls left to crawl

	//crawl(response, urls, todo);

	///*while (!todo.empty())
	//{
	//	url = todo.front();
	//	todo.pop();
	//	cout << url << endl;
	//	response = HTTPrequest(url);
	//	crawl(response, urls, todo);
	//}*/

	//debug_file(response, "C:\\Users\\Elia\\Desktop\\CrowLeerDebug.html");
	//debug_out(urls);

	//cout << "\nEnded";
	//cin.ignore();
	//cin.get();
	//return 0;

	string test = "http://erap.space/kb/blog/index.php?prova=2#paolo";
	uri temp(test);

	temp.debug();

	return 0;
}