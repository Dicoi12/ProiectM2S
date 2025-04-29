#ifndef X86_TIMING_GENETIC_VALUE_PREDICTOR_H
#define X86_TIMING_GENETIC_VALUE_PREDICTOR_H

#include <vector>
#include <unordered_map>
#include <random>
#include "Uop.h"

namespace x86
{

struct Chromosome {
    std::vector<int> genes;  
    double fitness;          
    
    Chromosome() : fitness(0.0) {}
};

struct PredictorParams {
    int history_size;        
    int confidence_threshold; 
    int stride_window;       
    double reuse_threshold;  
};

class GeneticValuePredictor
{
private:
    struct GeneticParams {
        int population_size = 100;
        int generations = 50;
        double mutation_rate = 0.1;
        double crossover_rate = 0.7;
        int tournament_size = 5;
    };

    struct ValueHistory {
        std::vector<long long> values;
        int stride;
        int confidence;
        long long last_prediction;
        bool has_prediction;
    };

    std::vector<Chromosome> population;
    GeneticParams params;
    PredictorParams current_params;
    std::unordered_map<long long, ValueHistory> prediction_table;
    
    std::mt19937 rng;
    
    long long total_predictions = 0;
    long long correct_predictions = 0;
    long long confident_predictions = 0;
    long long correct_confident_predictions = 0;

    void initializePopulation();
    double evaluateFitness(const Chromosome& chromosome);
    void selection();
    void crossover();
    void mutation();
    Chromosome tournamentSelection();
    PredictorParams decodeChromosome(const Chromosome& chromosome);
    bool predictWithParams(const PredictorParams& params, Uop* uop, long long& predicted_value);

public:
    GeneticValuePredictor();
    
    bool predict(Uop* uop, long long& predicted_value);
    void update(Uop* uop, long long actual_value);
    void train();
    bool isConfidentPrediction(Uop* uop);
    void dumpStats(std::ostream& os) const;
};

}  // namespace x86

#endif  // X86_TIMING_GENETIC_VALUE_PREDICTOR_H 