#include "log.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sys/stat.h>

void create_directory_if_not_exists(const std::string& dir) {
    struct stat info;
    if (stat(dir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        if (mkdir(dir.c_str(), 0777) != 0) {
            std::cerr << "could not create directory: " << dir << std::endl;
        }
    }
}

void log_message(const std::vector<std::string>& messages, const std::string& log_file) {
    create_directory_if_not_exists("logs");
    std::ofstream log(log_file, std::ios_base::app);
    if (log.is_open()) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        for (const auto& message : messages) {
            log << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
            std::cout << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
        }
        log.close();
    } else {
        std::cerr << "could not open log file: " << log_file << std::endl;
    }
}
