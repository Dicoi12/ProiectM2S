#ifndef ARCH_X86_TIMING_PREDICTOR_H
#define ARCH_X86_TIMING_PREDICTOR_H

#include <map>
#include <vector>
#include <iostream>
#include "Uop.h"

namespace x86
{

class Uop;

struct ValueHistory
{
    std::vector<long long> values;
    long long last_prediction;
    bool has_prediction;
    int stride;
    int confidence;
};

class Predictor
{
protected:
    std::map<long long, ValueHistory> prediction_table;
    long long total_predictions;
    long long correct_predictions;
    long long confident_predictions;
    long long correct_confident_predictions;

public:
    Predictor();
    virtual ~Predictor() = default;

    virtual bool predict(Uop* uop, long long& predicted_value) = 0;
    virtual void update(Uop* uop, long long actual_value) = 0;
    virtual bool isConfidentPrediction(Uop* uop) = 0;
    virtual void dumpStats(std::ostream& os) const = 0;
};

}  // namespace x86

#endif  // ARCH_X86_TIMING_PREDICTOR_H 