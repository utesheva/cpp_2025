#include "Solution.h"

class Mutation {
public:
    virtual void apply(Solution& solution) = 0;
};

class SchedulingMutation : public Mutation {
public:
    void apply(Solution& solution) override {
        SchedulingSolution &sched_solution = dynamic_cast<SchedulingSolution &>(solution);
        std::mt19937 &rng = sched_solution.get_rng();
        std::uniform_int_distribution<int> &distribution = sched_solution.get_distribution();
        std::uniform_int_distribution<int> job_dist(0, sched_solution.get_num_jobs() - 1);
        int jobIndex = job_dist(rng);
        int oldProcessor = sched_solution.get_job_processor(jobIndex);
        int newProcessor = distribution(rng);
        while (newProcessor == oldProcessor) {
            newProcessor = distribution(rng);
        }
        sched_solution.update_schedule(jobIndex, oldProcessor, newProcessor);
    }
};
