#ifndef X86_TIMING_VALUE_PREDICTOR_H
#define X86_TIMING_VALUE_PREDICTOR_H

#include <unordered_map>
#include <vector>
#include "Uop.h"

namespace x86
{

class ValuePredictor
{
private:
    struct ValueHistory
    {
        std::vector<long long> values;  
        int stride;                     
        int confidence;                 
        long long last_prediction;      
        bool has_prediction;            
    };

    std::unordered_map<long long, ValueHistory> prediction_table;
    
    static const int HISTORY_SIZE = 4;  
    static const int CONFIDENCE_THRESHOLD = 2;  

    long long total_predictions = 0;
    long long correct_predictions = 0;
    long long confident_predictions = 0;
    long long correct_confident_predictions = 0;

public:
    ValuePredictor();
    
    bool predict(Uop* uop, long long& predicted_value);
    
    void update(Uop* uop, long long actual_value);
    
    bool isConfidentPrediction(Uop* uop);

    void dumpStats(std::ostream& os) const;
};

}  // namespace x86

#endif  // X86_TIMING_VALUE_PREDICTOR_H 