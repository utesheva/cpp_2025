// parallel_research.cpp
#include "SimulatedAnnealing.h"
#include "load_CSV.cpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

class ParallelResearch {
private:
    std::mutex best_solution_mutex;
    std::shared_ptr<Solution> global_best;
    
public:
    // Запуск параллельного алгоритма с заданным количеством потоков
    double run_parallel_experiment(int num_threads, int num_jobs, int num_processors, 
                                  const std::vector<uint8_t>& job_times, int seed_base) {
        global_best.reset();
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Создание начального решения
        if (!global_best) {
            global_best = std::make_shared<SchedulingSolution>(
                num_jobs, num_processors, const_cast<std::vector<uint8_t>&>(job_times), seed_base);
        }
        
        // ИСПРАВЛЕНИЕ: Уменьшаем общее количество итераций при параллельной работе
        int total_iterations = 1000;  // Общий бюджет итераций
        int iterations_per_thread = std::max(100, total_iterations / num_threads);
        
        int global_no_improvement = 0;
        const int max_no_improvement = 10;
        
        BoltzmannLaw cooling(1000.0);
        SchedulingMutation mutation;
        
        while (global_no_improvement < max_no_improvement) {
            std::vector<std::thread> threads;
            std::vector<std::shared_ptr<Solution>> local_bests(num_threads);
            
            for (int i = 0; i < num_threads; ++i) {
                threads.emplace_back([&, i, iterations_per_thread]() {
                    unsigned int seed = seed_base + i + std::chrono::system_clock::now().time_since_epoch().count();
                    auto initial_solution = global_best->clone_new_seed(seed);
                    
                    // ИСПРАВЛЕНИЕ: Каждый поток делает меньше итераций
                    SimulatedAnnealingLimited sa(initial_solution.get(), &mutation, &cooling, 
                                                 1000.0, seed, iterations_per_thread);
                    sa.run();
                    
                    local_bests[i] = sa.getLocalBestSolution();
                });
            }
            
            for (auto &t : threads) {
                t.join();
            }
            
            bool improved = false;
            for (const auto &local_best : local_bests) {
                if (local_best->get_cost() < global_best->get_cost()) {
                    std::lock_guard<std::mutex> lock(best_solution_mutex);
                    global_best = local_best;
                    improved = true;
                }
            }
            
            if (improved) {
                global_no_improvement = 0;
            } else {
                global_no_improvement++;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    }
    
    std::shared_ptr<Solution> get_best_solution() const {
        return global_best;
    }

private:
    // НОВЫЙ КЛАСС: SimulatedAnnealing с ограничением итераций
    class SimulatedAnnealingLimited {
    private:
        std::shared_ptr<Solution> solution;
        std::shared_ptr<Solution> best_solution;
        Mutation* mutation;
        TemperatureLaw* temp_law;
        double initial_temp;
        double temperature;
        std::mt19937 rng;
        int max_iterations;
    
    public:
        SimulatedAnnealingLimited(Solution *sol,
                           Mutation *mut,
                           TemperatureLaw* law,
                           double t,
                           unsigned int seed,
                           int max_iter
        ):
            solution(sol->clone_new_seed(seed)),
            mutation(mut),
            temp_law(law),
            initial_temp(t),
            rng(seed),
            max_iterations(max_iter)
        {}

        void run() {
            int iter = 0;
            int iter_no_impr = 0;
            double best_cost = solution->get_cost();
            best_solution = solution->clone();
            temperature = initial_temp;

            // ИСПРАВЛЕНИЕ: Ограничиваем общее количество итераций
            while (iter_no_impr < max_iterations && iter < max_iterations * 2) {
                auto new_solution = best_solution->clone();
                mutation->apply(*new_solution);

                double new_cost = new_solution->get_cost();

                if (new_cost < best_cost) {
                    best_solution = new_solution;
                    best_cost = new_cost;
                    iter_no_impr = 0;
                }
                else {
                    double acceptanceProbability = std::exp(-(new_cost - best_cost) / temperature);
                    if (acceptanceProbability >= static_cast<double>(rand()) / RAND_MAX) {
                        iter_no_impr = 0;
                        best_solution = new_solution;
                    } 
                    else {
                        iter_no_impr++;
                    }
                }
                temperature = temp_law->get_next_temperature(iter);
                iter++;
            }
        }

        std::shared_ptr<Solution> getLocalBestSolution() const {
            return best_solution;
        }
    };
};

// Исследование масштабируемости параллельного алгоритма
void parallel_scaling_study() {
    const int num_jobs = 500;
    const int num_processors = 32;
    const int num_runs = 3;
    
    // Генерация тестовых данных
    std::vector<uint8_t> job_times(num_jobs);
    std::mt19937 gen(42);
    std::uniform_int_distribution<> distrib(1, 100);
    for (int i = 0; i < num_jobs; ++i) {
        job_times[i] = distrib(gen);
    }
    
    std::vector<int> thread_counts = {1, 2, 4, 8};
    ParallelResearch research;
    
    std::ofstream file("parallel_scaling.csv");
    file << "Threads,Time,Cost,Speedup,Efficiency" << std::endl;
    
    std::cout << "Исследование масштабируемости параллельного алгоритма:\n";
    std::cout << "Jobs: " << num_jobs << ", Processors: " << num_processors << "\n";
    std::cout << "=================================================\n";
    
    double sequential_time = 0;
    
    for (int threads : thread_counts) {
        double total_time = 0;
        double total_cost = 0;
        
        for (int run = 0; run < num_runs; ++run) {
            double time = research.run_parallel_experiment(threads, num_jobs, num_processors, job_times, 100 + run);
            double cost = research.get_best_solution()->get_cost();
            
            total_time += time;
            total_cost += cost;
        }
        
        double avg_time = total_time / num_runs;
        double avg_cost = total_cost / num_runs;
        
        if (threads == 1) {
            sequential_time = avg_time;
        }
        
        double speedup = sequential_time / avg_time;
        double efficiency = speedup / threads;
        
        file << threads << "," << avg_time << "," << avg_cost << "," 
             << speedup << "," << efficiency << std::endl;
        
        std::cout << "Threads: " << threads << "\n";
        std::cout << "  Avg Time: " << avg_time << "s\n";
        std::cout << "  Avg Cost: " << avg_cost << "\n";
        std::cout << "  Speedup: " << speedup << "x\n";
        std::cout << "  Efficiency: " << efficiency * 100 << "%\n";
        
        if (threads > 1) {
            double improvement = (sequential_time - avg_time) / sequential_time * 100;
            std::cout << "  Improvement: " << improvement << "%\n";
            
            if (improvement < 10) {
                std::cout << "  WARNING: Improvement less than 10%\n";
            }
        }
        std::cout << "----------------------------------------\n";
    }
    
    file.close();
    std::cout << "Данные сохранены в parallel_scaling.csv\n";
}

// Определение оптимального количества потоков
void find_optimal_threads() {
    std::cout << "\nОпределение оптимального количества потоков:\n";
    std::cout << "=================================================\n";
    
    std::ifstream file("parallel_scaling.csv");
    if (!file.is_open()) {
        std::cerr << "Cannot open parallel_scaling.csv\n";
        return;
    }
    
    std::string line;
    std::getline(file, line); // skip header
    
    int best_threads = 1;
    double best_speedup = 1.0;
    int saturation_point = 1;
    
    while (std::getline(file, line)) {
        size_t pos = 0;
        int threads = std::stoi(line, &pos);
        line = line.substr(pos + 1);
        double time = std::stod(line, &pos);
        line = line.substr(pos + 1);
        double cost = std::stod(line, &pos);
        line = line.substr(pos + 1);
        double speedup = std::stod(line, &pos);
        
        if (speedup > best_speedup) {
            best_speedup = speedup;
            best_threads = threads;
        }
        
        double efficiency = speedup / threads;
        if (efficiency < 0.7 && saturation_point == 1 && threads > 1) {
            saturation_point = threads;
        }
        
        std::cout << "Threads: " << threads << ", Speedup: " << speedup 
                  << "x, Efficiency: " << efficiency * 100 << "%\n";
    }
    
    std::cout << "\nРЕЗУЛЬТАТЫ:\n";
    std::cout << "Лучшее ускорение: " << best_speedup << "x при " << best_threads << " потоках\n";
    std::cout << "Точка насыщения: " << saturation_point << " потоков\n";
    std::cout << "Рекомендуемое количество потоков: " << std::min(best_threads, saturation_point) << "\n";
    
    file.close();
}

int main() {
    parallel_scaling_study();
    find_optimal_threads();
    
    return 0;
}
