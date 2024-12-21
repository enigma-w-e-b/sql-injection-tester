#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <thread>
#include <mutex>
#include "sql_injection_tester.h"
#include "utils.h"

std::vector<std::string> payloads;
std::mutex results_mutex;

void test_sql_injection_parallel(const std::string& url, const std::string& method, const std::string& body, const std::vector<std::string>& headers, std::vector<TestResult>& results, bool debug, const std::string& output_dir) {
    std::vector<TestResult> local_results;
    if (method == "GET") {
        test_sql_injection_get(url, headers, local_results, debug, output_dir);
    } else if (method == "POST") {
        test_sql_injection_post(url, body, headers, local_results, debug, output_dir);
        test_sql_injection_timing(url, body, headers, local_results, debug, output_dir);
    } else if (method == "PUT") {
        test_sql_injection_put(url, body, headers, local_results, debug, output_dir);
    } else if (method == "DELETE") {
        test_sql_injection_get(url, headers, local_results, debug, output_dir);
    } else {
        std::cerr << "unsupported method: " << method << std::endl;
    }

    std::lock_guard lock(results_mutex);
    results.insert(results.end(), local_results.begin(), local_results.end());
}

void test_sql_injection_get(const std::string& url, const std::vector<std::string>& headers, std::vector<TestResult>& results, const bool debug, const std::string& output_dir) {
    long response_code;
    if(CURL *curl = curl_easy_init()) {
        for(const auto& payload : payloads) {
            std::string test_url = replace_all(url, "[tests]", curl_easy_escape(curl, payload.c_str(), payload.length()));
            std::string readBuffer;

            curl_easy_setopt(curl, CURLOPT_URL, test_url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            curl_slist *header_list = setup_curl_headers(curl, headers);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

            const CURLcode res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            std::vector log_messages = {
                "Testing URL: " + test_url,
                "Response Code: " + std::to_string(response_code),
                "Headers: " + (header_list ? std::string(header_list->data) : "None")
            };

            TestResult result = {test_url, "GET", "", "", ""};

            if(res != CURLE_OK) {
                log_messages.push_back("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
                if (debug) log_message(log_messages, output_dir + "/sql_injection_tester.log");
                continue;
            }

            if(response_code >= 500 || readBuffer.find("syntax error") != std::string::npos ||
               readBuffer.find("sql") != std::string::npos) {
                result.status = "DANGER";
                result.details = "Possible SQL injection found: " + test_url;
                log_messages.push_back(result.status + " - " + result.details);
            } else {
                result.status = "PASS";
                result.details = "No vulnerability detected for: " + test_url;
                log_messages.push_back(result.status + " - " + result.details);
            }

            if (debug) log_message(log_messages, output_dir + "/sql_injection_tester.log");
            results.push_back(result);
            curl_slist_free_all(header_list);
        }
        curl_easy_cleanup(curl);
    }
}

void test_sql_injection_post(const std::string& url, const std::string& body, const std::vector<std::string>& headers, std::vector<TestResult>& results, const bool debug, const std::string& output_dir) {
    long response_code;
    if(CURL *curl = curl_easy_init()) {
        const bool json_body = is_json(body);
        for(const auto& payload : payloads) {
            std::string test_body = replace_all(body, "[tests]", payload);
            std::string readBuffer;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, test_body.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            curl_slist *header_list = setup_curl_headers(curl, headers);
            if (json_body) {
                header_list = curl_slist_append(header_list, "Content-Type: application/json");
            } else {
                header_list = curl_slist_append(header_list, "Content-Type: application/x-www-form-urlencoded");
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

            const CURLcode res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            std::vector log_messages = {
                "Testing URL: " + url,
                "Payload: " + payload,
                "Response Code: " + std::to_string(response_code),
                "Headers: " + (header_list ? std::string(header_list->data) : "None")
            };

            TestResult result = {url, "POST", test_body, "", ""};

            if(res != CURLE_OK) {
                log_messages.push_back("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
                if (debug) log_message(log_messages, output_dir + "/sql_injection_tester.log");
                continue;
            }

            if(response_code >= 500 || readBuffer.find("syntax error") != std::string::npos ||
               readBuffer.find("sql") != std::string::npos) {
                result.status = "DANGER";
                result.details = "Possible SQL injection found: " + url + " with payload: " + payload + " and body: " + test_body;
                log_messages.push_back(result.status + " - " + result.details);
            } else {
                result.status = "PASS";
                result.details = "No vulnerability detected for: " + url + " with payload: " + payload + " and body: " + test_body;
                log_messages.push_back(result.status + " - " + result.details);
            }

            if (debug) log_message(log_messages, output_dir + "/sql_injection_tester.log");
            results.push_back(result);
            curl_slist_free_all(header_list);
        }
        curl_easy_cleanup(curl);
    }
}

void test_sql_injection_put(const std::string &url, const std::string &body, const std::vector<std::string> &headers, std::vector<TestResult> &results, const bool debug, const std::string &output_dir) {
    long response_code;
    if(CURL *curl = curl_easy_init()) {
        const bool json_body = is_json(body);
        for(const auto& payload : payloads) {
            std::string test_body = replace_all(body, "[tests]", payload);
            std::string readBuffer;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, test_body.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            curl_slist *header_list = setup_curl_headers(curl, headers);
            if (json_body) {
                header_list = curl_slist_append(header_list, "Content-Type: application/json");
            } else {
                header_list = curl_slist_append(header_list, "Content-Type: application/x-www-form-urlencoded");
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

            const CURLcode res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            std::vector log_messages = {
                "Testing URL: " + url,
                "Payload: " + payload,
                "Response Code: " + std::to_string(response_code),
                "Headers: " + (header_list ? std::string(header_list->data) : "None")
            };

            TestResult result = {url, "PUT", test_body, "", ""};

            if(res != CURLE_OK) {
                log_messages.push_back("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
                if (debug) log_message(log_messages, output_dir + "/sql_injection_tester.log");
                continue;
            }

            if(response_code >= 500 || readBuffer.find("syntax error") != std::string::npos ||
               readBuffer.find("sql") != std::string::npos) {
                result.status = "DANGER";
                result.details = "Possible SQL injection found: " + url + " with payload: " + payload + " and body: " + test_body;
                log_messages.push_back(result.status + " - " + result.details);
               } else {
                   result.status = "PASS";
                   result.details = "No vulnerability detected for: " + url + " with payload: " + payload + " and body: " + test_body;
                   log_messages.push_back(result.status + " - " + result.details);
               }
            if (debug) log_message(log_messages, output_dir + "/sql_injection_tester.log");
            results.push_back(result);
            curl_slist_free_all(header_list);
        }
        curl_easy_cleanup(curl);
    }
}


void test_sql_injection_timing(const std::string& url, const std::string& body, const std::vector<std::string>& headers, std::vector<TestResult>& results, bool debug, const std::string& output_dir) {
    long response_code;
    if(CURL *curl = curl_easy_init()) {
        for(const auto& payload : payloads) {
            std::string test_body = replace_all(body, "[tests]", payload + " SLEEP(5)");
            std::string readBuffer;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, test_body.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            curl_slist *header_list = setup_curl_headers(curl, headers);
            if (is_json(body)) {
                header_list = curl_slist_append(header_list, "Content-Type: application/json");
            } else {
                header_list = curl_slist_append(header_list, "Content-Type: application/x-www-form-urlencoded");
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

            const CURLcode res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            std::vector log_messages = {
                "Testing URL: " + url,
                "Payload: " + payload + " SLEEP(5)",
                "Response Code: " + std::to_string(response_code),
                "Headers: " + (header_list ? std::string(header_list->data) : "None")
            };

            TestResult result = {url, "POST", test_body, "", ""};

            if(res == CURLE_OPERATION_TIMEDOUT) {
                result.status = "DANGER";
                result.details = "Possible blind SQL injection found: " + url + " with payload: " + payload + " and body: " + test_body;
                log_messages.push_back(result.status + " - " + result.details);
            } else {
                result.status = "PASS";
                result.details = "No vulnerability detected for: " + url + " with payload: " + payload + " and body: " + test_body;
                log_messages.push_back(result.status + " - " + result.details);
            }

            if (debug) log_message(log_messages, output_dir + "/sql_injection_tester.log");
            results.push_back(result);
            curl_slist_free_all(header_list);
        }
        curl_easy_cleanup(curl);
    }
}
