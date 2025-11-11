#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <memory>

class Solution {
public:
    virtual double get_cost() const  = 0;
    virtual void print() const = 0;
    virtual std::shared_ptr<Solution> clone_new_seed(unsigned int seed) const = 0;
    virtual std::shared_ptr<Solution> clone() const = 0;
};

class SchedulingSolution : public Solution {
private:
    int num_jobs;
    int num_processors;
    std::vector<uint8_t> job_times;
    mutable std::mt19937 rng;
    std::vector<std::vector<uint8_t>> schedule;
    std::uniform_int_distribution<int> distribution;
    std::vector<int> processor_loads;

public:
    SchedulingSolution(int jobs, int processors,
                       std::vector<uint8_t> &times, unsigned int seed) :
                       num_jobs(jobs), num_processors(processors),
                       job_times(times), distribution(0, processors - 1){
        rng.seed(seed);
        schedule.resize(num_jobs, std::vector<uint8_t>(num_processors, 0));
        processor_loads.resize(num_processors, 0);
        for (int i = 0; i < num_jobs; ++i) {
            int processor = distribution(rng);
            schedule[i][processor] = 1;
            processor_loads[processor] += job_times[i];
        }
    }

    double get_cost() const override {
        int Tmax = *std::max_element(processor_loads.begin(), processor_loads.end());
        int Tmin = *std::min_element(processor_loads.begin(), processor_loads.end());
        return static_cast<double>(Tmax - Tmin);
    }

    std::shared_ptr<Solution> clone() const override {
        return std::make_shared<SchedulingSolution>(*this);
    }

    std::shared_ptr<Solution> clone_new_seed(unsigned int seed) const override {
        auto cloned = std::make_shared<SchedulingSolution>(*this);
        cloned->rng.seed(seed);
        cloned->distribution = std::uniform_int_distribution<int>(0, num_processors - 1);
        return cloned;
    }

    void print() const override{
        for (int i = 0; i < num_processors; ++i) {
            std::cout << "Processor " << i << ": Load = " << processor_loads[i] << std::endl;
        }
    }

    std::mt19937 &get_rng() { return rng; }

    std::uniform_int_distribution<int> &get_distribution() { return distribution; }

    int get_num_processors() const { return num_processors; }

    int get_num_jobs() const { return num_jobs; }

    int get_job_processor(int job_index) const {
        for (int j = 0; j < num_processors; ++j) {
            if (schedule[job_index][j] == 1) {
                return j;
            }
        }
        return -1;
    }

    void update_schedule(int job_index, int old_processor, int new_processor) {
        schedule[job_index][old_processor] = 0;
        schedule[job_index][new_processor] = 1;
        processor_loads[old_processor] -= job_times[job_index];
        processor_loads[new_processor] += job_times[job_index];
    }
};
