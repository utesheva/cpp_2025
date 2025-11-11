#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>
std::vector<uint8_t> load_jobs(const std::string &filename) {
    std::vector<uint8_t> job_durations;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file " + filename);
    }

    std::string line;
    bool is_header = true;
    while (std::getline(file, line)) {
        if (is_header) {
            is_header = false;
            continue;
        }
        std::stringstream ss(line);
        std::string job_id;
        std::string duration_str;

        std::getline(ss, job_id, ',');
        std::getline(ss, duration_str, ',');

        uint8_t duration = std::stoi(duration_str);
        job_durations.push_back(duration);
    }

    file.close();
    return job_durations;
}
