#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <sstream>

void generate_csv_output(int num_jobs, int min_duration, int max_duration, const std::string& output_file = "jobs.csv") {
    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min_duration, max_duration);
    
    // Открытие файла для записи
    std::ofstream file(output_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << output_file << std::endl;
        return;
    }
    
    // Запись заголовка
    file << "Job ID,Duration\n";
    
    // Запись данных о работах
    for (int job_id = 1; job_id <= num_jobs; ++job_id) {
        int duration = distrib(gen);
        file << "Job_" << job_id << "," << duration << "\n";
    }
    
    file.close();
    std::cout << "CSV data generated and saved to " << output_file << std::endl;
}

int main() {
    // Параметры генерации данных
    int num_jobs, min_duration, max_duration;
    
    std::cout << "Enter the number of jobs: ";
    std::cin >> num_jobs;
    
    std::cout << "Enter the minimum job duration: ";
    std::cin >> min_duration;
    
    std::cout << "Enter the maximum job duration: ";
    std::cin >> max_duration;
    
    // Проверка корректности введенных данных
    if (num_jobs <= 0 || min_duration < 0 || max_duration < min_duration) {
        std::cerr << "Error: Invalid input parameters" << std::endl;
        return 1;
    }
    
    // Генерация CSV файла
    generate_csv_output(num_jobs, min_duration, max_duration);
    
    return 0;
}
