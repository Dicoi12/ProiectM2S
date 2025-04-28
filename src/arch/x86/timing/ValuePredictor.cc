#include "ValuePredictor.h"
#include <algorithm>

namespace x86
{

ValuePredictor::ValuePredictor()
{
    // Inițializare
}

bool ValuePredictor::predict(Uop* uop, long long& predicted_value)
{
    long long uop_id = uop->getId();
    auto it = prediction_table.find(uop_id);
    
    if (it == prediction_table.end())
        return false;  // Nu avem istoric pentru această instrucțiune
        
    ValueHistory& history = it->second;
    
    if (history.values.size() < 2)
        return false;  // Nu avem suficiente date pentru predicție
        
    // Verificăm dacă există un stride constant
    bool has_constant_stride = true;
    int stride = history.values[1] - history.values[0];
    
    for (size_t i = 2; i < history.values.size(); i++)
    {
        if (history.values[i] - history.values[i-1] != stride)
        {
            has_constant_stride = false;
            break;
        }
    }
    
    if (has_constant_stride)
    {
        // Prezicem următoarea valoare bazată pe stride
        predicted_value = history.values.back() + stride;
        history.stride = stride;
    }
    else
    {
        // Dacă nu avem stride constant, folosim ultima valoare
        predicted_value = history.values.back();
    }

    total_predictions++;
    if (isConfidentPrediction(uop)) {
        confident_predictions++;
    }
    
    return true;
}

void ValuePredictor::update(Uop* uop, long long actual_value)
{
    long long uop_id = uop->getId();
    ValueHistory& history = prediction_table[uop_id];
    
    // Verificăm dacă predicția anterioară a fost corectă
    if (!history.values.empty()) {
        long long last_prediction = history.values.back();
        if (last_prediction == actual_value) {
            correct_predictions++;
            if (isConfidentPrediction(uop)) {
                correct_confident_predictions++;
            }
        }
    }
    
    // Adăugăm noua valoare la istoric
    history.values.push_back(actual_value);
    
    // Păstrăm doar ultimele HISTORY_SIZE valori
    if (history.values.size() > HISTORY_SIZE)
        history.values.erase(history.values.begin());
    
    // Actualizăm încrederea
    if (history.values.size() >= 2)
    {
        int current_stride = history.values.back() - history.values[history.values.size()-2];
        if (current_stride == history.stride)
            history.confidence++;
        else
            history.confidence = 0;
    }
}

bool ValuePredictor::isConfidentPrediction(Uop* uop)
{
    long long uop_id = uop->getId();
    auto it = prediction_table.find(uop_id);
    
    if (it == prediction_table.end())
        return false;
        
    return it->second.confidence >= CONFIDENCE_THRESHOLD;
}

void ValuePredictor::dumpStats(std::ostream& os) const
{
    os << "\nValue Predictor Statistics:\n";
    os << "-------------------------\n";
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
    os << "\n";
}

}  // namespace x86 