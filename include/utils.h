#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <curl/curl.h>
#include <sql_injection_tester.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
std::string replace_all(std::string str, const std::string& from, const std::string& to);
bool is_json(const std::string& str);
void log_message(const std::vector<std::string>& messages, const std::string& log_file);
std::vector<std::tuple<std::string, std::string, std::string>> read_urls_from_csv(const std::string& filename);
void write_results_to_csv(const std::string& filename, const std::vector<TestResult>& results);
void write_results_to_html(const std::string& filename, const std::vector<TestResult>& results);
void load_additional_payloads(const std::string& filename);
std::string get_domain_from_url(const std::string& url);
curl_slist* setup_curl_headers(CURL* curl, const std::vector<std::string>& headers);

#endif // UTILS_H
