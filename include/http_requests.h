#ifndef HTTP_REQUESTS_H
#define HTTP_REQUESTS_H

#include <string>
#include <vector>
#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
void setup_curl_headers(CURL* curl, const std::vector<std::string>& headers);

#endif // HTTP_REQUESTS_H
