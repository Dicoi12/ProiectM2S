#include "GeneticValuePredictor.h"
#include <algorithm>
#include <random>
#include <chrono>

namespace x86
{

GeneticValuePredictor::GeneticValuePredictor() : ValuePredictor(), rng(std::chrono::system_clock::now().time_since_epoch().count()) {
    current_params.history_size = 4;
    current_params.confidence_threshold = 2;
    current_params.stride_window = 3;
}

void GeneticValuePredictor::initializePopulation() {
    population.resize(params.population_size);
    std::uniform_int_distribution<int> history_dist(1, 10);
    std::uniform_int_distribution<int> confidence_dist(1, 5);
    std::uniform_int_distribution<int> stride_dist(1, 5);

    for (auto& chromosome : population) {
        chromosome.genes.resize(3);
        chromosome.genes[0] = history_dist(rng);    
        chromosome.genes[1] = confidence_dist(rng); 
        chromosome.genes[2] = stride_dist(rng);     
        chromosome.fitness = 0.0;
    }
}

double GeneticValuePredictor::evaluateFitness(const Chromosome& chromosome) {
    PredictorParams params = decodeChromosome(chromosome);
    double correct = 0;
    double total = 0;

    for (const auto& entry : prediction_table) {
        if (entry.second.has_prediction) {
            total++;
            if (entry.second.last_prediction == entry.second.values.back()) {
                correct++;
            }
        }
    }

    return total > 0 ? correct / total : 0;
}

void GeneticValuePredictor::selection() {
    std::vector<Chromosome> new_population;
    new_population.reserve(params.population_size);

    auto best = std::max_element(population.begin(), population.end(),
        [](const Chromosome& a, const Chromosome& b) {
            return a.fitness < b.fitness;
        });
    new_population.push_back(*best);

    while (new_population.size() < params.population_size) {
        new_population.push_back(tournamentSelection());
    }

    population = std::move(new_population);
}

void GeneticValuePredictor::crossover() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<int> point_dist(0, 2);

    for (size_t i = 1; i < population.size(); i += 2) {
        if (dist(rng) < params.crossover_rate) {
            int crossover_point = point_dist(rng);
            for (int j = crossover_point; j < 3; j++) {
                std::swap(population[i].genes[j], population[i+1].genes[j]);
            }
        }
    }
}

void GeneticValuePredictor::mutation() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<int> history_dist(1, 10);
    std::uniform_int_distribution<int> confidence_dist(1, 5);
    std::uniform_int_distribution<int> stride_dist(1, 5);

    for (auto& chromosome : population) {
        if (dist(rng) < params.mutation_rate) {
            int gene = rand() % 3;
            switch (gene) {
                case 0: chromosome.genes[0] = history_dist(rng); break;
                case 1: chromosome.genes[1] = confidence_dist(rng); break;
                case 2: chromosome.genes[2] = stride_dist(rng); break;
            }
        }
    }
}

Chromosome GeneticValuePredictor::tournamentSelection() {
    std::uniform_int_distribution<int> dist(0, population.size() - 1);
    Chromosome best;
    best.fitness = -1;

    for (int i = 0; i < params.tournament_size; i++) {
        int idx = dist(rng);
        if (population[idx].fitness > best.fitness) {
            best = population[idx];
        }
    }

    return best;
}

PredictorParams GeneticValuePredictor::decodeChromosome(const Chromosome& chromosome) {
    PredictorParams params;
    params.history_size = chromosome.genes[0];
    params.confidence_threshold = chromosome.genes[1];
    params.stride_window = chromosome.genes[2];
    return params;
}

bool GeneticValuePredictor::predictWithParams(const PredictorParams& params, Uop* uop, long long& predicted_value) {
    long long uop_id = uop->getId();
    auto it = prediction_table.find(uop_id);
    
    if (it == prediction_table.end())
        return false;
        
    ValueHistory& history = it->second;
    
    if (history.values.size() < params.history_size)
        return false;
        
    bool has_constant_stride = true;
    int stride = history.values[1] - history.values[0];
    
    for (size_t i = 2; i < std::min(history.values.size(), static_cast<size_t>(params.stride_window)); i++) {
        if (history.values[i] - history.values[i-1] != stride) {
            has_constant_stride = false;
            break;
        }
    }
    
    if (has_constant_stride) {
        predicted_value = history.values.back() + stride;
        history.stride = stride;
    } else {
        predicted_value = history.values.back();
    }

    history.last_prediction = predicted_value;
    history.has_prediction = true;
    
    return true;
}

bool GeneticValuePredictor::predict(Uop* uop, long long& predicted_value) {
    return predictWithParams(current_params, uop, predicted_value);
}

void GeneticValuePredictor::update(Uop* uop, long long actual_value) {
    long long uop_id = uop->getId();
    ValueHistory& history = prediction_table[uop_id];
    
    if (history.has_prediction) {
        total_predictions++;  
        if (history.last_prediction == actual_value) {
            correct_predictions++;
            if (isConfidentPrediction(uop)) {
                correct_confident_predictions++;
            }
        }
        history.has_prediction = false;
    }
    
    history.values.push_back(actual_value);
    
    if (history.values.size() > current_params.history_size)
        history.values.erase(history.values.begin());
    
    if (history.values.size() >= 2) {
        int current_stride = history.values.back() - history.values[history.values.size()-2];
        if (current_stride == history.stride)
            history.confidence++;
        else
            history.confidence = 0;
    }
}

void GeneticValuePredictor::train() {
    initializePopulation();
    
    for (int gen = 0; gen < params.generations; gen++) {
        for (auto& chromosome : population) {
            chromosome.fitness = evaluateFitness(chromosome);
        }
        
        selection();
        
        crossover();
        
        mutation();
    }
    
    auto best = std::max_element(population.begin(), population.end(),
        [](const Chromosome& a, const Chromosome& b) {
            return a.fitness < b.fitness;
        });
    
    current_params = decodeChromosome(*best);
}

bool GeneticValuePredictor::isConfidentPrediction(Uop* uop) {
    long long uop_id = uop->getId();
    auto it = prediction_table.find(uop_id);
    
    if (it == prediction_table.end())
        return false;
        
    return it->second.confidence >= current_params.confidence_threshold;
}

void GeneticValuePredictor::dumpStats(std::ostream& os) const {
    os << "\nGenetic Value Predictor Statistics:\n";
    os << "--------------------------------\n";
    os << "Total Predictions: " << total_predictions << "\n";
    os << "Correct Predictions: " << correct_predictions << "\n";
    os << "Confident Predictions: " << confident_predictions << "\n";
    os << "Correct Confident Predictions: " << correct_confident_predictions << "\n";
    
    if (total_predictions > 0) {
        double accuracy = (double)correct_predictions / total_predictions * 100;
        os << "Overall Accuracy: " << accuracy << "%\n";
    }
    
    if (confident_predictions > 0) {
        double confident_accuracy = (double)correct_confident_predictions / confident_predictions * 100;
        os << "Confident Predictions Accuracy: " << confident_accuracy << "%\n";
    }
    
    os << "\nCurrent Parameters:\n";
    os << "History Size: " << current_params.history_size << "\n";
    os << "Confidence Threshold: " << current_params.confidence_threshold << "\n";
    os << "Stride Window: " << current_params.stride_window << "\n";
    os << "\n";
}

}  // namespace x86 