#include "SimulatedAnnealing.h"
#include "load_CSV.cpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

double measure_sequential_time(int num_jobs, int num_processors, TemperatureLaw* law, int seed) {
    std::vector<uint8_t> job_times(num_jobs);

    job_times = load_jobs("jobs.csv");
    
    auto initial_solution = std::make_shared<SchedulingSolution>(
        num_jobs, num_processors, job_times, seed);
    SchedulingMutation mutation;
    
    auto start = std::chrono::high_resolution_clock::now();
    SimulatedAnnealing sa(initial_solution.get(), &mutation, law, 1000.0, seed);
    sa.run();
    auto end = std::chrono::high_resolution_clock::now();
    
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0; // в секундах
}

std::vector<int> find_heavy_inputs() {
    std::vector<int> jobs_list = {64000, 128000, 256000};
    std::vector<int> processors_list = {20, 40, 160, 320};
    std::vector<int> result;

    BoltzmannLaw boltzmann(1000.0);
    CauchyLaw cauchy(1000.0);
    LogarithmicCauchyLaw log_cauchy(1000.0);
    
    std::cout << "Поиск тяжелых входных данных (время > 60 секунд):\n";
    std::cout << "=================================================\n";
    
    for (int jobs : jobs_list) {
        for (int processors : processors_list) {
            double time_boltzmann = measure_sequential_time(jobs, processors, &boltzmann, 42);
            double time_cauchy = measure_sequential_time(jobs, processors, &cauchy, 42);
            double time_log_cauchy = measure_sequential_time(jobs, processors, &log_cauchy, 42);
            
            if (time_boltzmann > 60 || time_cauchy > 60 || time_log_cauchy > 60) {
                std::cout << "Jobs: " << jobs << ", Processors: " << processors << "\n";
                std::cout << "  Boltzmann: " << time_boltzmann << "s\n";
                std::cout << "  Cauchy: " << time_cauchy << "s\n";
                std::cout << "  Log-Cauchy: " << time_log_cauchy << "s\n";
                std::cout << "----------------------------------------\n";
                result = {jobs, processors};
            }
        }
    }
    return result;
}

// Структура для хранения информации о законе охлаждения
struct CoolingLawInfo {
    TemperatureLaw* law;
    const char* name;
};

// Сравнение качества решений для тяжелых данных
void compare_solution_quality(int heavy_jobs, int heavy_processors) {
    std::cout << "\nСравнение качества решений для тяжелых данных:\n";
    std::cout << "Jobs: " << heavy_jobs << ", Processors: " << heavy_processors << "\n";
    std::cout << "=================================================\n";
    
    std::vector<uint8_t> job_times(heavy_jobs);
    job_times = load_jobs("jobs.csv"); 
    // Создаем законы охлаждения
    BoltzmannLaw boltzmann(1000.0);
    CauchyLaw cauchy(1000.0);
    LogarithmicCauchyLaw log_cauchy(1000.0);
    
    // Массив для итерации
    CoolingLawInfo laws[] = {
        {&boltzmann, "Boltzmann"},
        {&cauchy, "Cauchy"},
        {&log_cauchy, "Log-Cauchy"}
    };
    
    const int num_runs = 5;
    
    for (const auto& law_info : laws) {
        double total_cost = 0;
        double total_time = 0;
        
        for (int run = 0; run < num_runs; ++run) {
            auto initial_solution = std::make_shared<SchedulingSolution>(
                heavy_jobs, heavy_processors, job_times, 42 + run);
            SchedulingMutation mutation;
            
            auto start = std::chrono::high_resolution_clock::now();
            SimulatedAnnealing sa(initial_solution.get(), &mutation, law_info.law, 1000.0, 42 + run);
            sa.run();
            auto end = std::chrono::high_resolution_clock::now();
            
            total_cost += sa.getLocalBestSolution()->get_cost();
            total_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        }
        
        std::cout << law_info.name << " Law:\n";
        std::cout << "  Average Cost: " << total_cost / num_runs << "\n";
        std::cout << "  Average Time: " << total_time / num_runs / 1000.0 << "s\n";
        std::cout << "----------------------------------------\n";
    }
}

// Построение тепловой карты
void generate_heatmap_data() {
    const int num_runs = 5;
    
    BoltzmannLaw law(1000.0); // Используем один закон
    
    std::ofstream file("heatmap_data.csv");
    file << "Jobs,Processors,Time" << std::endl;
    
    std::cout << "Генерация данных для тепловой карты...\n";
    std::vector jobs_list = {4000, 16000, 64000, 128000, 256000};
    std::vector processors_list = {10, 40, 160, 640};
    for (int jobs: jobs_list) {
        for (int processors: processors_list) {
            double total_time = 0;
            
            for (int run = 0; run < num_runs; ++run) {
                total_time += measure_sequential_time(jobs, processors, &law, 42 + run);
            }
            
            double avg_time = total_time / num_runs;
            file << jobs << "," << processors << "," << avg_time << std::endl;
            
            std::cout << "Jobs: " << jobs << ", Processors: " << processors 
                      << ", Avg Time: " << avg_time << "s\n";
        }
    }
    
    file.close();
    std::cout << "Данные сохранены в heatmap_data.csv\n";
}

int main() {
    // Шаг 1: Поиск тяжелых входных данных
    //std::vector<int> heavy = find_heavy_inputs();
    
    // Шаг 2: Сравнение качества решений для найденных тяжелых данных
    //compare_solution_quality(heavy[0], heavy[1]); // пример тяжелых данных
    
    // Шаг 3: Генерация данных для тепловой карты
    generate_heatmap_data();
    
    return 0;
}
