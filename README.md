# CrowLeer, the fast and reliable CLI web crawler with focus on pages download

## Options:

| Option                                                                                  | Description                                                                                              |
|-----------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------|
| -h --help                                                                               | View this help page                                                                                      |
| -u --url                                                                                | URL used to start crawling                                                                               |
| -t --threads                                                                            | Number of threads used                                                                                   |
| -d --depth                                                                              | Maximum crawling depth (the starting URL spcified in -u is at depth 0)                                   |
| -x --same-domain                                                                        | Quick flag to only follow URLs with the same domain as the starting URL, overrides the --f-domain option |
| -a --save                                                                               | Activates the download functionality of CrowLeer. If not used nothing will be saved on the disk          |
| --f-global                                                                              | Follow rule to be tested on the whole parsed URL                                                         |
| --f-protocol --f-domain --f-path --f--filename --f-extension --f-querystring --f-anchor | Follow rules on single parts of parsed URLs                                                              |
| --s-global                                                                              | Save rule to be tested on the whole parsed URL                                                           |
| --s-protocol --s-domain --s-path --s--filename --s-extension --s-querystring --s-anchor | Save rules on single parts of parsed URLs                                                                |

## Rules:

CrowLeer uses Regular Expressions (https://en.wikipedia.org/wiki/Regular_expression) to apply conditions to URLs or parts of URLs.

Both rules have a global component, that matches the Completed URL (see the URL section) and one for every URL part.

There are two rules: Follow Rule and Save Rule.

* Follow Rule: describes pages to follow while crawling
* Save Rule: describes pages to download and save locally

If not specified every rule is set to match anything. You can set every possible composition of rules to describe the exact scenario you need, including global rule and parts rules together.

## URLs:

CrowLeer completes the URLs found in the crawled pages to make its and your work easier.

Every URL is split in parts and completed with parts from the URL of the page it was found in if necessary.

The parts in which a URL is split are: protocol, domain, path, filename, extension, querystring and anchor.

#### Example

The URL ```"/example/one/file.txt"``` was found while running on ```"https://erap.space"```

The Completed URL will be ```"https://erap.space/example/one/file.txt"```, and its parts will be:

| Part        | Value         |
|-------------|---------------|
| protocol    | "https"       |
| domain      | "erap.space"  |
| path        | "example/one" |
| filename    | "file"        |
| extension   | "txt"         |
| querystring | ""            |
| anchor      | ""            |

#### Example

The URL ```"https://en.wikipedia.org/wiki/Dog?s=canis#Origin"``` will be split in parts this way:

| Part        | Value              |
|-------------|--------------------|
| protocol    | "https"            |
| domain      | "en.wikipedia.org" |
| path        | "wiki/Dog"         |
| filename    | ""                 |
| extension   | ""                 |
| querystring | "s=canis"          |
| anchor      | "Origin"           |