#ifndef SQL_INJECTION_TESTER_H
#define SQL_INJECTION_TESTER_H

#include <string>
#include <vector>

struct TestResult {
    std::string url;
    std::string method;
    std::string body;
    std::string status;
    std::string details;
};
extern std::vector<std::string> payloads;

void test_sql_injection_parallel(const std::string& url, const std::string& method, const std::string& body, const std::vector<std::string>& headers, std::vector<TestResult>& results, bool debug, const std::string& output_dir);

void test_sql_injection_get(const std::string& url, const std::vector<std::string>& headers, std::vector<TestResult>& results, bool debug, const std::string& output_dir);
void test_sql_injection_post(const std::string& url, const std::string& body, const std::vector<std::string>& headers, std::vector<TestResult>& results, bool debug, const std::string& output_dir);
void test_sql_injection_timing(const std::string& url, const std::string& body, const std::vector<std::string>& headers, std::vector<TestResult>& results, bool debug, const std::string& output_dir);

#endif // SQL_INJECTION_TESTER_H
