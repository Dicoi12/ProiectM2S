#ifndef ARCH_X86_TIMING_GENETIC_VALUE_PREDICTOR_H
#define ARCH_X86_TIMING_GENETIC_VALUE_PREDICTOR_H

#include "ValuePredictor.h"
#include <vector>
#include <unordered_map>
#include <random>
#include <chrono>
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
};

struct GeneticParams {
    int population_size = 50;
    int generations = 100;
    double crossover_rate = 0.8;
    double mutation_rate = 0.1;
    int tournament_size = 3;
};

class GeneticValuePredictor : public ValuePredictor
{
private:
    std::vector<Chromosome> population;
    PredictorParams current_params;
    GeneticParams params;
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
    
    bool predict(Uop* uop, long long& predicted_value) override;
    void update(Uop* uop, long long actual_value) override;
    bool isConfidentPrediction(Uop* uop) override;
    void dumpStats(std::ostream& os) const override;
    void train();
};

}  // namespace x86

#endif  // ARCH_X86_TIMING_GENETIC_VALUE_PREDICTOR_H 