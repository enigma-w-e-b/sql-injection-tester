#include "http_requests.h"


size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    static_cast<std::string *>(userp)->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}


void setup_curl_headers(CURL* curl, const std::vector<std::string>& headers) {
    struct curl_slist *header_list = nullptr;
    for (const auto& header : headers) {
        header_list = curl_slist_append(header_list, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
}
