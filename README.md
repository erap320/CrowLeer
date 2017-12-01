CrowLeer, the Fast and reliable CLI web crawler

####Options:

*-h --help\tView this help page
*-u --url\tURL used to start crawling
*-t --thread\tNumber of threads used
*-d --depth\tMaximum crawling depth

####Rules:

CrowLeer uses Regular Expressions (https://en.wikipedia.org/wiki/Regular_expression) to apply conditions to URLs or parts of URLs.

Both rules have a global component, that matches the Completed URL (see the URL section) and one for every URL part.

There are two rules: Follow Rule and Save Rule.

* Follow Rule: describes pages to follow while crawling
* Save Rule: describes pages to download and save locally

If not specified every rule is set to match anything. You can set every possible composition of rules to describe the exact scenario you need, including global rule and parts rules together.

####URLs:

CrowLeer completes the URLs found in the crawled pages to make its and your work easier.

Every URL is split in parts and completed with parts from the URL of the page it was found in if necessary.

The parts in which a URL is split are: protocol, domain, path, filename, extension, querystring and anchor.



**Example**: the URL ```"/example/one/file.txt"``` was found while running on ```"https://erap.space"```

The Completed URL will be ```"https://erap.space/example/one/file.txt"```, and its parts will be:

* protocol: ```"https"```
* domain: ```"erap.space"```
* path: ```"example/one"```
* filename: ```"file"```
* extension: ```"txt"```
* querystring: ```""```
* anchor: ```""```



**Example**: the URL ```"https://en.wikipedia.org/wiki/Dog?s=canis#Origin"``` will be split in parts this way:

* protocol: ```"https"```
* domain: ```"en.wikipedia.org"```
* path: ```"wiki/Dog"```
* filename: ```""```
* extension: ```""```
* querystring: ```"s=canis"```
* anchor: ```"Origin"```