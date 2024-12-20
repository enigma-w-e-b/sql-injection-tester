#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <filesystem>
#include "sql_injection_tester.h"
#include "utils.h"

namespace fs = std::filesystem;

void usage(const std::string& arg) {
    std::cout << std::endl << "SQL Injection Tester" << std::endl << std::endl;
    std::cout << "Usage: " << arg << " --csv <csv_file> --method <GET|POST> --body <body> [--payloads <payload_file>] [--output <output_dir>] [--debug] [-H <header1> -H <header2> ...]" << std::endl;
    std::cout << "Or: " << arg << " --url <url> --method <GET|POST> --body <body> [--payloads <payload_file>] [--output <output_dir>] [--debug] [-H <header1> -H <header2> ...]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --csv <csv_file>     CSV file containing URLs to tests" << std::endl;
    std::cout << "  --url <url>          URL to tests" << std::endl;
    std::cout << "  --method <GET|POST>  HTTP method to use" << std::endl;
    std::cout << "  --body <body>        HTTP body to use" << std::endl;
    std::cout << "  --payloads <payload_file>  File containing additional payloads" << std::endl;
    std::cout << "  --output <output_dir> Output directory for results" << std::endl;
    std::cout << "  --debug              Enable debug mode" << std::endl;
    std::cout << "  -H <header>          Additional HTTP header to send" << std::endl;
    std::cout << std::endl << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << arg << R"( --csv /var/lib/sql-injection-tester/files/urls.csv --method GET --body "" --payloads /var/lib/sql-injection-tester/files/payloads.txt --output /var/lib/sql-injection-tester/results --debug -H "Authorization: Bearer token")" << std::endl;
    std::cout << "  " << arg << R"( --url "https://example.com/api/v1/users" --method POST --body '{"username": "admin", "password": "password"}' --payloads /var/lib/sql-injection-tester/files/payloads.txt --output /var/lib/sql-injection-tester/results --debug -H "Authorization: Bearer token")" << std::endl << std::endl;
    std::cout << "Payloads and Urls example files can be found in /var/lib/sql-injection-tester/examples" << std::endl;
    std::cout << "Credits:" << std::endl;
    std::cout << "  Enigma Web, Ciphers Systems - https://ciphers.systems" << std::endl << std::endl;
}

int main(const int argc, char** argv) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    std::string csv_file, method, body, url, output_dir = ".", payload_file = "/var/lib/sql-injection-tester/files/payloads.txt";
    std::vector<std::string> headers;
    std::vector<TestResult> results;
    bool debug = false;

    for (int i = 1; i < argc; ++i) {
        if (std::string arg = argv[i]; arg == "--csv" && i + 1 < argc) {
            csv_file = argv[i + 1];
        } else if (arg == "--method" && i + 1 < argc) {
            method = argv[i + 1];
        } else if (arg == "--body" && i + 1 < argc) {
            body = argv[i + 1];
        } else if (arg == "--payloads" && i + 1 < argc) {
            payload_file = argv[i + 1];
        } else if (arg == "--url" && i + 1 < argc) {
            url = argv[i + 1];
        } else if (arg == "--output" && i + 1 < argc) {
            output_dir = argv[i + 1];
        } else if (arg == "--debug") {
            debug = true;
        } else if (arg == "-H" && i + 1 < argc) {
            headers.emplace_back(argv[i + 1]);
        }
    }

    load_additional_payloads(payload_file);

    std::vector<std::thread> threads;

    const std::string domain = get_domain_from_url(url.empty() ? csv_file : url);
    std::string results_dir = output_dir + "/" + domain;

    fs::create_directories(results_dir);
    const std::string output_file_date = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    if (!csv_file.empty()) {
        auto urls = read_urls_from_csv(csv_file);
        for (const auto& [url, method_in_csv, body_in_csv] : urls) {
            threads.emplace_back(test_sql_injection_parallel, url, method_in_csv, body_in_csv, headers, std::ref(results), debug, results_dir);
        }
        for (auto& thread : threads) {
            thread.join();
        }
        write_results_to_csv(results_dir + "/sql_injection_test_results_" + output_file_date + ".csv", results);
        write_results_to_html(results_dir + "/sql_injection_test_results_" + output_file_date + ".html", results);
        std::cout << "results written to " << results_dir << "/sql_injection_test_results_" + output_file_date + ".csv and " << results_dir << "/sql_injection_test_results_" + output_file_date + ".html" << std::endl;
    } else if (!method.empty()) {
        if (url.empty()) {
            usage(argv[0]);
            return 1;
        }
        threads.emplace_back(test_sql_injection_parallel, url, method, body, headers, std::ref(results), debug, results_dir);
        for (auto& thread : threads) {
            thread.join();
        }
        write_results_to_csv(results_dir + "/sql_injection_test_results.csv" + output_file_date + ".csv", results);
        write_results_to_html(results_dir + "/sql_injection_test_results.html" + output_file_date + ".html", results);
        std::cout << "results written to " << results_dir << "/sql_injection_test_results" + output_file_date + ".csv and " << results_dir << "/sql_injection_test_results" + output_file_date + ".html" << std::endl;
    } else {
        usage(argv[0]);
        return 1;
    }
    return 0;
}
