#include "SimulatedAnnealing.h"
#include "load_CSV.cpp"
#include <thread>
#include <chrono>

int main(int argc, char *argv[]) {
    std::shared_ptr<Solution> global_best_solution;
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <num_threads>" << std::endl;
            return 1;
        }

        int num_threads = std::stoi(argv[1]);
        std::vector<uint8_t> job_durations = load_jobs("jobs.csv");
        int num_jobs = job_durations.size();
        int num_processors = 40;

        SchedulingMutation mutationOperation;
        BoltzmannLaw coolingSchedule(100.0);
        double initialTemperature = 100.0;

        int globalNoImprovementCount = 0;

        
        if (!global_best_solution) {
            global_best_solution = std::make_shared<SchedulingSolution>(num_jobs, num_processors, job_durations, std::chrono::system_clock::now().time_since_epoch().count());
        }
        

        while (globalNoImprovementCount < 10) {
            std::vector<std::thread> threads;
            std::vector<std::shared_ptr<Solution>> local_best_solutions(num_threads);

            for (int i = 0; i < num_threads; ++i) {
                threads.emplace_back([&, i]() {
                    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count() + i;
                    std::shared_ptr<Solution> initialSolution;

                    
                    initialSolution = global_best_solution->clone_new_seed(seed);
                    

                    SimulatedAnnealing sa(initialSolution.get(), &mutationOperation, &coolingSchedule, initialTemperature, seed);
                    sa.run();

                    local_best_solutions[i] = sa.getLocalBestSolution();
                });
            }

            for (auto &t : threads) {
                t.join();
            }

            bool improved = false;
            for (const auto &localBest : local_best_solutions) {
                if (localBest->get_cost() < global_best_solution->get_cost()) {
                    global_best_solution = localBest;
                    improved = true;
                }
            }
            if (improved) {
                globalNoImprovementCount = 0;
            } else {
                globalNoImprovementCount++;
            }

            std::cout << "Current best solution cost: " << global_best_solution->get_cost() << std::endl;
        }
        std::cout << "Current best solution cost: " << global_best_solution->get_cost() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
