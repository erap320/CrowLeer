#include "conditions.h"

rule::rule()
{
	this->protocol = ".*";
	this->domain = ".*";
	this->path = ".*";
	this->filename = ".*";
	this->extension = ".*";
	this->querystring = ".*";
	this->anchor = ".*";
}