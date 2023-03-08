#include "utils.hpp"
#include "uri.hpp"
#include "color.hpp"
#include "timestamp.hpp"
#include "conditions.hpp"

#include <iostream>

using std::cout; using std::cin;
using std::ofstream;

mutex queueMutex;
mutex consoleMutex;
Output out;
std::vector<curl_option> options;
std::map<string, CURLoption> optcode;

Output::~Output()
{
	if (log)
	{
		log << "\n\nWork ended at " << timestamp() << "\n";
		log.close();
	}
}

void Output::useLog(string name)
{
	log.open(name);
	log << "Work started at " << timestamp() << "\n\n";
}

Output& operator<<(Output &out, string s)
{
	cout << s;
	if (out.log)
		out.log << s;

	return out;
}

Output& operator<<(Output &out, int i)
{
	cout << i;
	if (out.log)
		out.log << i;

	return out;
}

void curl_options_init()
{
	optcode["CURLOPT_WRITEDATA"] = CURLOPT_WRITEDATA;
	optcode["CURLOPT_URL"] = CURLOPT_URL;
	optcode["CURLOPT_PORT"] = CURLOPT_PORT;
	optcode["CURLOPT_PROXY"] = CURLOPT_PROXY;
	optcode["CURLOPT_USERPWD"] = CURLOPT_USERPWD;
	optcode["CURLOPT_PROXYUSERPWD"] = CURLOPT_PROXYUSERPWD;
	optcode["CURLOPT_RANGE"] = CURLOPT_RANGE;
	optcode["CURLOPT_READDATA"] = CURLOPT_READDATA;
	optcode["CURLOPT_ERRORBUFFER"] = CURLOPT_ERRORBUFFER;
	optcode["CURLOPT_WRITEFUNCTION"] = CURLOPT_WRITEFUNCTION;
	optcode["CURLOPT_READFUNCTION"] = CURLOPT_READFUNCTION;
	optcode["CURLOPT_TIMEOUT"] = CURLOPT_TIMEOUT;
	optcode["CURLOPT_INFILESIZE"] = CURLOPT_INFILESIZE;
	optcode["CURLOPT_POSTFIELDS"] = CURLOPT_POSTFIELDS;
	optcode["CURLOPT_REFERER"] = CURLOPT_REFERER;
	optcode["CURLOPT_FTPPORT"] = CURLOPT_FTPPORT;
	optcode["CURLOPT_USERAGENT"] = CURLOPT_USERAGENT;
	optcode["CURLOPT_LOW_SPEED_LIMIT"] = CURLOPT_LOW_SPEED_LIMIT;
	optcode["CURLOPT_LOW_SPEED_TIME"] = CURLOPT_LOW_SPEED_TIME;
	optcode["CURLOPT_RESUME_FROM"] = CURLOPT_RESUME_FROM;
	optcode["CURLOPT_COOKIE"] = CURLOPT_COOKIE;
	optcode["CURLOPT_HTTPHEADER"] = CURLOPT_HTTPHEADER;
	optcode["CURLOPT_HTTPPOST"] = CURLOPT_HTTPPOST;
	optcode["CURLOPT_SSLCERT"] = CURLOPT_SSLCERT;
	optcode["CURLOPT_KEYPASSWD"] = CURLOPT_KEYPASSWD;
	optcode["CURLOPT_CRLF"] = CURLOPT_CRLF;
	optcode["CURLOPT_QUOTE"] = CURLOPT_QUOTE;
	optcode["CURLOPT_HEADERDATA"] = CURLOPT_HEADERDATA;
	optcode["CURLOPT_COOKIEFILE"] = CURLOPT_COOKIEFILE;
	optcode["CURLOPT_SSLVERSION"] = CURLOPT_SSLVERSION;
	optcode["CURLOPT_TIMECONDITION"] = CURLOPT_TIMECONDITION;
	optcode["CURLOPT_TIMEVALUE"] = CURLOPT_TIMEVALUE;
	optcode["CURLOPT_CUSTOMREQUEST"] = CURLOPT_CUSTOMREQUEST;
	optcode["CURLOPT_STDERR"] = CURLOPT_STDERR;
	optcode["CURLOPT_POSTQUOTE"] = CURLOPT_POSTQUOTE;
	optcode["CURLOPT_OBSOLETE40"] = CURLOPT_OBSOLETE40;
	optcode["CURLOPT_VERBOSE"] = CURLOPT_VERBOSE;
	optcode["CURLOPT_HEADER"] = CURLOPT_HEADER;
	optcode["CURLOPT_NOPROGRESS"] = CURLOPT_NOPROGRESS;
	optcode["CURLOPT_NOBODY"] = CURLOPT_NOBODY;
	optcode["CURLOPT_FAILONERROR"] = CURLOPT_FAILONERROR;
	optcode["CURLOPT_UPLOAD"] = CURLOPT_UPLOAD;
	optcode["CURLOPT_POST"] = CURLOPT_POST;
	optcode["CURLOPT_DIRLISTONLY"] = CURLOPT_DIRLISTONLY;
	optcode["CURLOPT_APPEND"] = CURLOPT_APPEND;
	optcode["CURLOPT_NETRC"] = CURLOPT_NETRC;
	optcode["CURLOPT_FOLLOWLOCATION"] = CURLOPT_FOLLOWLOCATION;
	optcode["CURLOPT_TRANSFERTEXT"] = CURLOPT_TRANSFERTEXT;
	optcode["CURLOPT_PUT"] = CURLOPT_PUT;
	optcode["CURLOPT_PROGRESSFUNCTION"] = CURLOPT_PROGRESSFUNCTION;
	optcode["CURLOPT_PROGRESSDATA"] = CURLOPT_PROGRESSDATA;
	optcode["CURLOPT_AUTOREFERER"] = CURLOPT_AUTOREFERER;
	optcode["CURLOPT_PROXYPORT"] = CURLOPT_PROXYPORT;
	optcode["CURLOPT_POSTFIELDSIZE"] = CURLOPT_POSTFIELDSIZE;
	optcode["CURLOPT_HTTPPROXYTUNNEL"] = CURLOPT_HTTPPROXYTUNNEL;
	optcode["CURLOPT_INTERFACE"] = CURLOPT_INTERFACE;
	optcode["CURLOPT_KRBLEVEL"] = CURLOPT_KRBLEVEL;
	optcode["CURLOPT_SSL_VERIFYPEER"] = CURLOPT_SSL_VERIFYPEER;
	optcode["CURLOPT_CAINFO"] = CURLOPT_CAINFO;
	optcode["CURLOPT_MAXREDIRS"] = CURLOPT_MAXREDIRS;
	optcode["CURLOPT_FILETIME"] = CURLOPT_FILETIME;
	optcode["CURLOPT_TELNETOPTIONS"] = CURLOPT_TELNETOPTIONS;
	optcode["CURLOPT_MAXCONNECTS"] = CURLOPT_MAXCONNECTS;
	optcode["CURLOPT_OBSOLETE72"] = CURLOPT_OBSOLETE72;
	optcode["CURLOPT_FRESH_CONNECT"] = CURLOPT_FRESH_CONNECT;
	optcode["CURLOPT_FORBID_REUSE"] = CURLOPT_FORBID_REUSE;
	optcode["CURLOPT_RANDOM_FILE"] = CURLOPT_RANDOM_FILE;
	optcode["CURLOPT_EGDSOCKET"] = CURLOPT_EGDSOCKET;
	optcode["CURLOPT_CONNECTTIMEOUT"] = CURLOPT_CONNECTTIMEOUT;
	optcode["CURLOPT_HEADERFUNCTION"] = CURLOPT_HEADERFUNCTION;
	optcode["CURLOPT_HTTPGET"] = CURLOPT_HTTPGET;
	optcode["CURLOPT_SSL_VERIFYHOST"] = CURLOPT_SSL_VERIFYHOST;
	optcode["CURLOPT_COOKIEJAR"] = CURLOPT_COOKIEJAR;
	optcode["CURLOPT_SSL_CIPHER_LIST"] = CURLOPT_SSL_CIPHER_LIST;
	optcode["CURLOPT_HTTP_VERSION"] = CURLOPT_HTTP_VERSION;
	optcode["CURLOPT_FTP_USE_EPSV"] = CURLOPT_FTP_USE_EPSV;
	optcode["CURLOPT_SSLCERTTYPE"] = CURLOPT_SSLCERTTYPE;
	optcode["CURLOPT_SSLKEY"] = CURLOPT_SSLKEY;
	optcode["CURLOPT_SSLKEYTYPE"] = CURLOPT_SSLKEYTYPE;
	optcode["CURLOPT_SSLENGINE"] = CURLOPT_SSLENGINE;
	optcode["CURLOPT_SSLENGINE_DEFAULT"] = CURLOPT_SSLENGINE_DEFAULT;
	optcode["CURLOPT_DNS_USE_GLOBAL_CACHE"] = CURLOPT_DNS_USE_GLOBAL_CACHE;
	optcode["CURLOPT_DNS_CACHE_TIMEOUT"] = CURLOPT_DNS_CACHE_TIMEOUT;
	optcode["CURLOPT_PREQUOTE"] = CURLOPT_PREQUOTE;
	optcode["CURLOPT_DEBUGFUNCTION"] = CURLOPT_DEBUGFUNCTION;
	optcode["CURLOPT_DEBUGDATA"] = CURLOPT_DEBUGDATA;
	optcode["CURLOPT_COOKIESESSION"] = CURLOPT_COOKIESESSION;
	optcode["CURLOPT_CAPATH"] = CURLOPT_CAPATH;
	optcode["CURLOPT_BUFFERSIZE"] = CURLOPT_BUFFERSIZE;
	optcode["CURLOPT_NOSIGNAL"] = CURLOPT_NOSIGNAL;
	optcode["CURLOPT_SHARE"] = CURLOPT_SHARE;
	optcode["CURLOPT_PROXYTYPE"] = CURLOPT_PROXYTYPE;
	optcode["CURLOPT_ACCEPT_ENCODING"] = CURLOPT_ACCEPT_ENCODING;
	optcode["CURLOPT_PRIVATE"] = CURLOPT_PRIVATE;
	optcode["CURLOPT_HTTP200ALIASES"] = CURLOPT_HTTP200ALIASES;
	optcode["CURLOPT_UNRESTRICTED_AUTH"] = CURLOPT_UNRESTRICTED_AUTH;
	optcode["CURLOPT_FTP_USE_EPRT"] = CURLOPT_FTP_USE_EPRT;
	optcode["CURLOPT_HTTPAUTH"] = CURLOPT_HTTPAUTH;
	optcode["CURLOPT_SSL_CTX_FUNCTION"] = CURLOPT_SSL_CTX_FUNCTION;
	optcode["CURLOPT_SSL_CTX_DATA"] = CURLOPT_SSL_CTX_DATA;
	optcode["CURLOPT_FTP_CREATE_MISSING_DIRS"] = CURLOPT_FTP_CREATE_MISSING_DIRS;
	optcode["CURLOPT_PROXYAUTH"] = CURLOPT_PROXYAUTH;
	optcode["CURLOPT_FTP_RESPONSE_TIMEOUT"] = CURLOPT_FTP_RESPONSE_TIMEOUT;
	optcode["CURLOPT_IPRESOLVE"] = CURLOPT_IPRESOLVE;
	optcode["CURLOPT_MAXFILESIZE"] = CURLOPT_MAXFILESIZE;
	optcode["CURLOPT_INFILESIZE_LARGE"] = CURLOPT_INFILESIZE_LARGE;
	optcode["CURLOPT_RESUME_FROM_LARGE"] = CURLOPT_RESUME_FROM_LARGE;
	optcode["CURLOPT_MAXFILESIZE_LARGE"] = CURLOPT_MAXFILESIZE_LARGE;
	optcode["CURLOPT_NETRC_FILE"] = CURLOPT_NETRC_FILE;
	optcode["CURLOPT_USE_SSL"] = CURLOPT_USE_SSL;
	optcode["CURLOPT_POSTFIELDSIZE_LARGE"] = CURLOPT_POSTFIELDSIZE_LARGE;
	optcode["CURLOPT_TCP_NODELAY"] = CURLOPT_TCP_NODELAY;
	optcode["CURLOPT_FTPSSLAUTH"] = CURLOPT_FTPSSLAUTH;
	optcode["CURLOPT_IOCTLFUNCTION"] = CURLOPT_IOCTLFUNCTION;
	optcode["CURLOPT_IOCTLDATA"] = CURLOPT_IOCTLDATA;
	optcode["CURLOPT_FTP_ACCOUNT"] = CURLOPT_FTP_ACCOUNT;
	optcode["CURLOPT_COOKIELIST"] = CURLOPT_COOKIELIST;
	optcode["CURLOPT_IGNORE_CONTENT_LENGTH"] = CURLOPT_IGNORE_CONTENT_LENGTH;
	optcode["CURLOPT_FTP_SKIP_PASV_IP"] = CURLOPT_FTP_SKIP_PASV_IP;
	optcode["CURLOPT_FTP_FILEMETHOD"] = CURLOPT_FTP_FILEMETHOD;
	optcode["CURLOPT_LOCALPORT"] = CURLOPT_LOCALPORT;
	optcode["CURLOPT_LOCALPORTRANGE"] = CURLOPT_LOCALPORTRANGE;
	optcode["CURLOPT_CONNECT_ONLY"] = CURLOPT_CONNECT_ONLY;
	optcode["CURLOPT_CONV_FROM_NETWORK_FUNCTION"] = CURLOPT_CONV_FROM_NETWORK_FUNCTION;
	optcode["CURLOPT_CONV_TO_NETWORK_FUNCTION"] = CURLOPT_CONV_TO_NETWORK_FUNCTION;
	optcode["CURLOPT_CONV_FROM_UTF8_FUNCTION"] = CURLOPT_CONV_FROM_UTF8_FUNCTION;
	optcode["CURLOPT_MAX_SEND_SPEED_LARGE"] = CURLOPT_MAX_SEND_SPEED_LARGE;
	optcode["CURLOPT_MAX_RECV_SPEED_LARGE"] = CURLOPT_MAX_RECV_SPEED_LARGE;
	optcode["CURLOPT_FTP_ALTERNATIVE_TO_USER"] = CURLOPT_FTP_ALTERNATIVE_TO_USER;
	optcode["CURLOPT_SOCKOPTFUNCTION"] = CURLOPT_SOCKOPTFUNCTION;
	optcode["CURLOPT_SOCKOPTDATA"] = CURLOPT_SOCKOPTDATA;
	optcode["CURLOPT_SSL_SESSIONID_CACHE"] = CURLOPT_SSL_SESSIONID_CACHE;
	optcode["CURLOPT_SSH_AUTH_TYPES"] = CURLOPT_SSH_AUTH_TYPES;
	optcode["CURLOPT_SSH_PUBLIC_KEYFILE"] = CURLOPT_SSH_PUBLIC_KEYFILE;
	optcode["CURLOPT_SSH_PRIVATE_KEYFILE"] = CURLOPT_SSH_PRIVATE_KEYFILE;
	optcode["CURLOPT_FTP_SSL_CCC"] = CURLOPT_FTP_SSL_CCC;
	optcode["CURLOPT_TIMEOUT_MS"] = CURLOPT_TIMEOUT_MS;
	optcode["CURLOPT_CONNECTTIMEOUT_MS"] = CURLOPT_CONNECTTIMEOUT_MS;
	optcode["CURLOPT_HTTP_TRANSFER_DECODING"] = CURLOPT_HTTP_TRANSFER_DECODING;
	optcode["CURLOPT_HTTP_CONTENT_DECODING"] = CURLOPT_HTTP_CONTENT_DECODING;
	optcode["CURLOPT_NEW_FILE_PERMS"] = CURLOPT_NEW_FILE_PERMS;
	optcode["CURLOPT_NEW_DIRECTORY_PERMS"] = CURLOPT_NEW_DIRECTORY_PERMS;
	optcode["CURLOPT_POSTREDIR"] = CURLOPT_POSTREDIR;
	optcode["CURLOPT_SSH_HOST_PUBLIC_KEY_MD5"] = CURLOPT_SSH_HOST_PUBLIC_KEY_MD5;
	optcode["CURLOPT_OPENSOCKETFUNCTION"] = CURLOPT_OPENSOCKETFUNCTION;
	optcode["CURLOPT_OPENSOCKETDATA"] = CURLOPT_OPENSOCKETDATA;
	optcode["CURLOPT_COPYPOSTFIELDS"] = CURLOPT_COPYPOSTFIELDS;
	optcode["CURLOPT_PROXY_TRANSFER_MODE"] = CURLOPT_PROXY_TRANSFER_MODE;
	optcode["CURLOPT_SEEKFUNCTION"] = CURLOPT_SEEKFUNCTION;
	optcode["CURLOPT_SEEKDATA"] = CURLOPT_SEEKDATA;
	optcode["CURLOPT_CRLFILE"] = CURLOPT_CRLFILE;
	optcode["CURLOPT_ISSUERCERT"] = CURLOPT_ISSUERCERT;
	optcode["CURLOPT_ADDRESS_SCOPE"] = CURLOPT_ADDRESS_SCOPE;
	optcode["CURLOPT_CERTINFO"] = CURLOPT_CERTINFO;
	optcode["CURLOPT_USERNAME"] = CURLOPT_USERNAME;
	optcode["CURLOPT_PASSWORD"] = CURLOPT_PASSWORD;
	optcode["CURLOPT_PROXYUSERNAME"] = CURLOPT_PROXYUSERNAME;
	optcode["CURLOPT_PROXYPASSWORD"] = CURLOPT_PROXYPASSWORD;
	optcode["CURLOPT_NOPROXY"] = CURLOPT_NOPROXY;
	optcode["CURLOPT_TFTP_BLKSIZE"] = CURLOPT_TFTP_BLKSIZE;
	optcode["CURLOPT_SOCKS5_GSSAPI_SERVICE"] = CURLOPT_SOCKS5_GSSAPI_SERVICE;
	optcode["CURLOPT_SOCKS5_GSSAPI_NEC"] = CURLOPT_SOCKS5_GSSAPI_NEC;
	optcode["CURLOPT_PROTOCOLS"] = CURLOPT_PROTOCOLS;
	optcode["CURLOPT_REDIR_PROTOCOLS"] = CURLOPT_REDIR_PROTOCOLS;
	optcode["CURLOPT_SSH_KNOWNHOSTS"] = CURLOPT_SSH_KNOWNHOSTS;
	optcode["CURLOPT_SSH_KEYFUNCTION"] = CURLOPT_SSH_KEYFUNCTION;
	optcode["CURLOPT_SSH_KEYDATA"] = CURLOPT_SSH_KEYDATA;
	optcode["CURLOPT_MAIL_FROM"] = CURLOPT_MAIL_FROM;
	optcode["CURLOPT_MAIL_RCPT"] = CURLOPT_MAIL_RCPT;
	optcode["CURLOPT_FTP_USE_PRET"] = CURLOPT_FTP_USE_PRET;
	optcode["CURLOPT_RTSP_REQUEST"] = CURLOPT_RTSP_REQUEST;
	optcode["CURLOPT_RTSP_SESSION_ID"] = CURLOPT_RTSP_SESSION_ID;
	optcode["CURLOPT_RTSP_STREAM_URI"] = CURLOPT_RTSP_STREAM_URI;
	optcode["CURLOPT_RTSP_TRANSPORT"] = CURLOPT_RTSP_TRANSPORT;
	optcode["CURLOPT_RTSP_CLIENT_CSEQ"] = CURLOPT_RTSP_CLIENT_CSEQ;
	optcode["CURLOPT_RTSP_SERVER_CSEQ"] = CURLOPT_RTSP_SERVER_CSEQ;
	optcode["CURLOPT_INTERLEAVEDATA"] = CURLOPT_INTERLEAVEDATA;
	optcode["CURLOPT_INTERLEAVEFUNCTION"] = CURLOPT_INTERLEAVEFUNCTION;
	optcode["CURLOPT_WILDCARDMATCH"] = CURLOPT_WILDCARDMATCH;
	optcode["CURLOPT_CHUNK_BGN_FUNCTION"] = CURLOPT_CHUNK_BGN_FUNCTION;
	optcode["CURLOPT_CHUNK_END_FUNCTION"] = CURLOPT_CHUNK_END_FUNCTION;
	optcode["CURLOPT_FNMATCH_FUNCTION"] = CURLOPT_FNMATCH_FUNCTION;
	optcode["CURLOPT_CHUNK_DATA"] = CURLOPT_CHUNK_DATA;
	optcode["CURLOPT_FNMATCH_DATA"] = CURLOPT_FNMATCH_DATA;
	optcode["CURLOPT_RESOLVE"] = CURLOPT_RESOLVE;
	optcode["CURLOPT_TLSAUTH_USERNAME"] = CURLOPT_TLSAUTH_USERNAME;
	optcode["CURLOPT_TLSAUTH_PASSWORD"] = CURLOPT_TLSAUTH_PASSWORD;
	optcode["CURLOPT_TLSAUTH_TYPE"] = CURLOPT_TLSAUTH_TYPE;
	optcode["CURLOPT_TRANSFER_ENCODING"] = CURLOPT_TRANSFER_ENCODING;
	optcode["CURLOPT_CLOSESOCKETFUNCTION"] = CURLOPT_CLOSESOCKETFUNCTION;
	optcode["CURLOPT_CLOSESOCKETDATA"] = CURLOPT_CLOSESOCKETDATA;
	optcode["CURLOPT_GSSAPI_DELEGATION"] = CURLOPT_GSSAPI_DELEGATION;
	optcode["CURLOPT_DNS_SERVERS"] = CURLOPT_DNS_SERVERS;
	optcode["CURLOPT_ACCEPTTIMEOUT_MS"] = CURLOPT_ACCEPTTIMEOUT_MS;
	optcode["CURLOPT_TCP_KEEPALIVE"] = CURLOPT_TCP_KEEPALIVE;
	optcode["CURLOPT_TCP_KEEPIDLE"] = CURLOPT_TCP_KEEPIDLE;
	optcode["CURLOPT_TCP_KEEPINTVL"] = CURLOPT_TCP_KEEPINTVL;
	optcode["CURLOPT_SSL_OPTIONS"] = CURLOPT_SSL_OPTIONS;
	optcode["CURLOPT_MAIL_AUTH"] = CURLOPT_MAIL_AUTH;
	optcode["CURLOPT_SASL_IR"] = CURLOPT_SASL_IR;
	optcode["CURLOPT_XFERINFOFUNCTION"] = CURLOPT_XFERINFOFUNCTION;
	optcode["CURLOPT_XOAUTH2_BEARER"] = CURLOPT_XOAUTH2_BEARER;
	optcode["CURLOPT_DNS_INTERFACE"] = CURLOPT_DNS_INTERFACE;
	optcode["CURLOPT_DNS_LOCAL_IP4"] = CURLOPT_DNS_LOCAL_IP4;
	optcode["CURLOPT_DNS_LOCAL_IP6"] = CURLOPT_DNS_LOCAL_IP6;
	optcode["CURLOPT_LOGIN_OPTIONS"] = CURLOPT_LOGIN_OPTIONS;
	optcode["CURLOPT_SSL_ENABLE_NPN"] = CURLOPT_SSL_ENABLE_NPN;
	optcode["CURLOPT_SSL_ENABLE_ALPN"] = CURLOPT_SSL_ENABLE_ALPN;
	optcode["CURLOPT_EXPECT_100_TIMEOUT_MS"] = CURLOPT_EXPECT_100_TIMEOUT_MS;
	optcode["CURLOPT_PROXYHEADER"] = CURLOPT_PROXYHEADER;
	optcode["CURLOPT_HEADEROPT"] = CURLOPT_HEADEROPT;
	optcode["CURLOPT_PINNEDPUBLICKEY"] = CURLOPT_PINNEDPUBLICKEY;
	optcode["CURLOPT_UNIX_SOCKET_PATH"] = CURLOPT_UNIX_SOCKET_PATH;
	optcode["CURLOPT_SSL_VERIFYSTATUS"] = CURLOPT_SSL_VERIFYSTATUS;
	optcode["CURLOPT_SSL_FALSESTART"] = CURLOPT_SSL_FALSESTART;
	optcode["CURLOPT_PATH_AS_IS"] = CURLOPT_PATH_AS_IS;
	optcode["CURLOPT_PROXY_SERVICE_NAME"] = CURLOPT_PROXY_SERVICE_NAME;
	optcode["CURLOPT_SERVICE_NAME"] = CURLOPT_SERVICE_NAME;
	optcode["CURLOPT_PIPEWAIT"] = CURLOPT_PIPEWAIT;
	optcode["CURLOPT_DEFAULT_PROTOCOL"] = CURLOPT_DEFAULT_PROTOCOL;
	optcode["CURLOPT_STREAM_WEIGHT"] = CURLOPT_STREAM_WEIGHT;
	optcode["CURLOPT_STREAM_DEPENDS"] = CURLOPT_STREAM_DEPENDS;
	optcode["CURLOPT_STREAM_DEPENDS_E"] = CURLOPT_STREAM_DEPENDS_E;
	optcode["CURLOPT_TFTP_NO_OPTIONS"] = CURLOPT_TFTP_NO_OPTIONS;
	optcode["CURLOPT_CONNECT_TO"] = CURLOPT_CONNECT_TO;
	optcode["CURLOPT_TCP_FASTOPEN"] = CURLOPT_TCP_FASTOPEN;
	optcode["CURLOPT_KEEP_SENDING_ON_ERROR"] = CURLOPT_KEEP_SENDING_ON_ERROR;
	optcode["CURLOPT_PROXY_CAINFO"] = CURLOPT_PROXY_CAINFO;
	optcode["CURLOPT_PROXY_CAPATH"] = CURLOPT_PROXY_CAPATH;
	optcode["CURLOPT_PROXY_SSL_VERIFYPEER"] = CURLOPT_PROXY_SSL_VERIFYPEER;
	optcode["CURLOPT_PROXY_SSL_VERIFYHOST"] = CURLOPT_PROXY_SSL_VERIFYHOST;
	optcode["CURLOPT_PROXY_SSLVERSION"] = CURLOPT_PROXY_SSLVERSION;
	optcode["CURLOPT_PROXY_TLSAUTH_USERNAME"] = CURLOPT_PROXY_TLSAUTH_USERNAME;
	optcode["CURLOPT_PROXY_TLSAUTH_PASSWORD"] = CURLOPT_PROXY_TLSAUTH_PASSWORD;
	optcode["CURLOPT_PROXY_TLSAUTH_TYPE"] = CURLOPT_PROXY_TLSAUTH_TYPE;
	optcode["CURLOPT_PROXY_SSLCERT"] = CURLOPT_PROXY_SSLCERT;
	optcode["CURLOPT_PROXY_SSLCERTTYPE"] = CURLOPT_PROXY_SSLCERTTYPE;
	optcode["CURLOPT_PROXY_SSLKEY"] = CURLOPT_PROXY_SSLKEY;
	optcode["CURLOPT_PROXY_SSLKEYTYPE"] = CURLOPT_PROXY_SSLKEYTYPE;
	optcode["CURLOPT_PROXY_KEYPASSWD"] = CURLOPT_PROXY_KEYPASSWD;
	optcode["CURLOPT_PROXY_SSL_CIPHER_LIST"] = CURLOPT_PROXY_SSL_CIPHER_LIST;
	optcode["CURLOPT_PROXY_CRLFILE"] = CURLOPT_PROXY_CRLFILE;
	optcode["CURLOPT_PROXY_SSL_OPTIONS"] = CURLOPT_PROXY_SSL_OPTIONS;
	optcode["CURLOPT_PRE_PROXY"] = CURLOPT_PRE_PROXY;
	optcode["CURLOPT_PROXY_PINNEDPUBLICKEY"] = CURLOPT_PROXY_PINNEDPUBLICKEY;
	optcode["CURLOPT_ABSTRACT_UNIX_SOCKET"] = CURLOPT_ABSTRACT_UNIX_SOCKET;
	optcode["CURLOPT_SUPPRESS_CONNECT_HEADERS"] = CURLOPT_SUPPRESS_CONNECT_HEADERS;
	optcode["CURLOPT_REQUEST_TARGET"] = CURLOPT_REQUEST_TARGET;
	optcode["CURLOPT_SOCKS5_AUTH"] = CURLOPT_SOCKS5_AUTH;
	optcode["CURLOPT_SSH_COMPRESSION"] = CURLOPT_SSH_COMPRESSION;
	optcode["CURLOPT_MIMEPOST"] = CURLOPT_MIMEPOST;
}

CURLoption curl_option_value(string option) {
	return optcode[option];
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t realsize = size * nmemb;
	string *response = (string*)userdata;
	response->append(ptr, nmemb);

	return realsize;
}

string HTTPrequest(string url)
{
	CURLoption optnum;	//Custom option value
	int type;	//Custom option type as described in the following definitions in curl.h

	/*
		#define CURLOPTTYPE_LONG          0
		#define CURLOPTTYPE_OBJECTPOINT   10000
		#define CURLOPTTYPE_STRINGPOINT   10000
		#define CURLOPTTYPE_FUNCTIONPOINT 20000
		#define CURLOPTTYPE_OFF_T         30000
	*/


	//CURL parameters
	CURL *curl;
	CURLcode res;

	string response; //Contains HTTP response

	//CURL initialization
	curl = curl_easy_init();
	if (curl) {
		//CURL options
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

		for (auto it = options.begin(); it != options.end(); it++)
		{
			optnum = curl_option_value(it->name);
			type = optnum / 10000;

			switch(type)
			{
				case 0: //Long
				{
					curl_easy_setopt(curl, optnum, std::stol(it->parameter));
					break;
				}
				case 1: //String
				{
					curl_easy_setopt(curl, optnum, it->parameter.c_str());
					break;
				}
				default: //Unsupported
				{
					out << "Unsupported custom CURL option " << it->name << ", please contact the developer at battistonelia@erap.space about this issue\n";
					break;
				}
			}
		}

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

		//URL setting
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		//HTTP request execution
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			error_out(">> Network Error: " + (string)curl_easy_strerror(res) + " on " + url);
		}

		//CURL cleanup
		curl_easy_cleanup(curl);
	}

	return response;
}

void debug_out(const unordered_set<string>& data)
{
	for (auto it = data.begin(); it != data.end(); it++)
	{
		out << *it << "\n";
	}
}

bool writeToDisk(const string& str, fs::path path)
{
	ofstream out(path, std::ios::binary);
	if (!out.is_open())
		return false;
	out << str;
	out.close();
	return true;
}

size_t findURL(const string& response, size_t offset, string attr)
{
	size_t pos; //Holds the position of the string searched for in the response
	size_t i;

	pos = response.find(attr, offset); //Find the string "href"

	while (pos != string::npos && pos < response.length())
	{
		if (pos == string::npos) //If the string is not in the response
			return pos;
		for (i = (pos + attr.size()); response[i] == ' '; i++) //Point to the first non-space char
			;
		//Check if the href is followed by =' or ="
		if (response[i] == '=')
		{
			for (i++; response[i] == ' '; i++)
				;
			if (response[i] == '\'' || response[i] == '"')
				return pos;
		}
		pos = response.find(attr, i); //Look for the next attribute
	}

	return string::npos;
}


void crawl(const string& response, unordered_set<string>& urls, queue<uri>& todo, bool saveflag, std::regex excludeCondition, int maxdepth,  uri* const parent)
{
	size_t pos; //Holds the position of the string searched for in the response
	size_t before, after; //Hold the position of opening and closing quote of the href property
	string extracted;
	uri temp;

	//Find every attribute in the page and add the URL to the urls vector
	for (unsigned int i = 0; i < URLattributesNum; i++)
	{
		pos = findURL(response, 0, URLattributes[i]);
		while (pos != string::npos && pos < response.length())
		{
			before = response.find_first_of("\"'", pos + URLattributes[i].length());
			after = response.find(response[before], before + 1);
			extracted = response.substr(before + 1, after - before - 1);
			temp = parse(extracted, parent);

			queueMutex.lock();
			//Add only if never found before
			auto search = urls.find(temp.toString());
			if (search == urls.end())
			{
				urls.insert(temp.toString());

				if (!std::regex_match(temp.toString(), excludeCondition))
				{
					todo.push(temp);
				}
			}
			queueMutex.unlock();

			pos = findURL(response, after + 1, URLattributes[i]);
		}
	}
}

void relativizeUrls(string* page, const uri& current, const rule& followCondition, const rule& saveCondition, int maxdepth)
{
	//Make the link relative only if the page will be downloaded according to rules
	//Otherwise make the link absolute to let the user open the right resource downloaded from the live website
	
	size_t pos; //Holds the position of the string searched for in the response
	size_t before, after; //Hold the position of opening and closing quote of the href property
	string extracted;
	uri temp;

	//Find every href in the page and relativize it
	for (unsigned int i = 0; i < URLattributesNum; i++)
	{
		pos = findURL(*page, 0, URLattributes[i]);
		while (pos < page->length() && pos != string::npos)
		{
			before = page->find_first_of("\"'", pos + URLattributes[i].length());
			after = page->find((*page)[before], before + 1);
			extracted = page->substr(before + 1, after - before - 1);
			temp = parse(extracted, &current, false);

			if(current.depth < maxdepth && temp.check(saveCondition))
				page->replace(before + 1, extracted.size(), relative(temp, current));
			else
				page->replace(before + 1, extracted.size(), temp.toString());

			pos = findURL(*page, after + 1, URLattributes[i]);
		}
	}
}

string validate(string url)
{
	url = trim(url);
	size_t pos;
	pos = url.find("://");
	if (pos == string::npos)
		url = "http://" + url;
	return url;
}

fs::path computePath(const uri& link, string basePath)
{
	fs::path result = basePath;
	result /= std::regex_replace(link.domain, regex(":|\\*|\\?|\"|<|>|\\|"), "");
	result /= std::regex_replace(link.path, regex(":|\\*|\\?|\"|<|>|\\|"), "");

	if (!fs::exists(result))
	{
		try {
			fs::create_directories(result);
		}
		catch (fs::filesystem_error e) {
			error_out((string)e.what() + " : Could not create folder for URL " + link.toString());
		}
	}

	string filename = "index";

	if (!link.filename.empty())
		filename = link.filename;

	if (!link.extension.empty())
		filename += "." + link.extension;
	else
		filename += ".html";

	if (!link.querystring.empty())
		filename += "_" + link.querystring;

	if (!link.anchor.empty())
		filename += "#" + link.anchor;

	filename = std::regex_replace(filename, regex(":|\\*|\\?|\"|<|>|\\|"), "");

	result /= filename;

	return result;
}

void error_out(string s)
{
	consoleMutex.lock();
		Color('r');
		out << s << "\n";
		Color('w');
	consoleMutex.unlock();
}

void special_out(string s, bool color)
{
	if(color)
		Color('g');
	out << s;
	if(color)
		Color('w');
}