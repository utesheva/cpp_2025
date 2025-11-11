#include "Mutation.h"
#include "Cooling.h"

class SimulatedAnnealing {
private:
    std::shared_ptr<Solution> solution;
    std::shared_ptr<Solution> best_solution;
    std::shared_ptr<Solution> local_best_solution;
    Mutation* mutation;
    TemperatureLaw* temp_law;
    double initial_temp;
    double temperature;
    std::mt19937 rng;
public:
    SimulatedAnnealing(Solution *sol,
                       Mutation *mut,
                       TemperatureLaw* law,
                       double t,
                       unsigned int seed
    ):
        solution(sol->clone_new_seed(seed)),
        mutation(mut),
        temp_law(law),
        initial_temp(t),
        rng(seed)
    {}

    void run() {
        int iter = 0;
        int iter_no_impr = 0;
        double best_cost = solution->get_cost();
        best_solution = solution->clone();
        temperature = initial_temp;

        while (iter_no_impr < 100) {
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
