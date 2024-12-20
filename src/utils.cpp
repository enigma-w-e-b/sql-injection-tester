#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <regex>
#include <sys/stat.h>
#include <curl/curl.h>
#include <sql_injection_tester.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    static_cast<std::string *>(userp)->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}

std::string get_domain_from_url(const std::string& url) {
    const std::regex domain_regex(R"((?:https?://)?([^/]+))");
    if (std::smatch match; std::regex_search(url, match, domain_regex)) {
        return match[1].str();
    }
    return "";
}

std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

bool is_json(const std::string& str) {
    return str.find('{') != std::string::npos && str.find('}') != std::string::npos;
}

void create_directory_if_not_exists(const std::string& dir) {
    struct stat info{};
    if (stat(dir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        if (mkdir(dir.c_str(), 0777) != 0) {
            std::cerr << "could not create directory: " << dir << std::endl;
        }
    }
}

void log_message(const std::vector<std::string>& messages, const std::string& log_file) {
    create_directory_if_not_exists("logs");
    if (std::ofstream log(log_file, std::ios_base::app); log.is_open()) {
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        for (const auto& message : messages) {
            log << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
            std::cout << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
        }
        log.close();
    } else {
        std::cerr << "could not open log file: " << log_file << std::endl;
    }
}

std::vector<std::tuple<std::string, std::string, std::string>> read_urls_from_csv(const std::string& filename) {
    std::vector<std::tuple<std::string, std::string, std::string>> urls;
    if (std::ifstream file(filename); file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string method, body;
            if (std::string url; std::getline(ss, url, '|') && std::getline(ss, method, '|') && std::getline(ss, body, '|')) {
                urls.emplace_back(url, method, body);
            }
        }
        file.close();
    } else {
        std::cerr << "could not open file: " << filename << std::endl;
    }
    return urls;
}

void write_results_to_csv(const std::string& filename, const std::vector<TestResult>& results) {
    create_directory_if_not_exists("results");
    if (std::ofstream file(filename); file.is_open()) {
        file << "status,url,method,body,details\n";
        for (const auto&[url, method, body, status, details] : results) {
            file << status << "," << url << "," << method << "," << body << "," << details << "\n";
        }
        file.close();
    } else {
        std::cerr << "could not open file for writing: " << filename << std::endl;
    }
}

void write_results_to_html(const std::string& filename, const std::vector<TestResult>& results) {
    create_directory_if_not_exists("results");
    if (std::ofstream file(filename); file.is_open()) {
        file << "<html><head><title>SQL Injection Test Results</title></head><body>";
        file << "<h1>SQL Injection Test Results</h1>";
        file << "<table border='1'><tr><th>Status</th><th>URL</th><th>Method</th><th>Body</th><th>Details</th></tr>";
        for (const auto&[url, method, body, status, details] : results) {
            file << "<tr>";
            file << "<td>" << status << "</td>";
            file << "<td>" << url << "</td>";
            file << "<td>" << method << "</td>";
            file << "<td>" << body << "</td>";
            file << "<td>" << details << "</td>";
            file << "</tr>";
        }
        file << "</table></body></html>";
        file.close();
    } else {
        std::cerr << "could not open file for writing: " << filename << std::endl;
    }
}

void load_additional_payloads(const std::string& filename) {
    if (std::ifstream file(filename); file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                payloads.push_back(line);
            }
        }
        file.close();
    } else {
        std::cerr << "could not open payload file: " << filename << std::endl;
    }
}

curl_slist* setup_curl_headers(CURL* curl, const std::vector<std::string>& headers) {
    curl_slist* header_list = nullptr;
    for (const auto& header : headers) {
        header_list = curl_slist_append(header_list, header.c_str());
    }
    return header_list;
}
