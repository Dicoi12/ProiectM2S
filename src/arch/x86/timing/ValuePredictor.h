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
    // Structura pentru stocarea istoricului valorilor
    struct ValueHistory
    {
        std::vector<long long> values;  // Istoricul valorilor
        int stride;                     // Diferența constantă între valori (dacă există)
        int confidence;                 // Nivelul de încredere în predicție
        long long last_prediction;      // Ultima predicție făcută
        bool has_prediction;            // Flag care indică dacă am făcut o predicție
    };

    // Tabela de predicție
    std::unordered_map<long long, ValueHistory> prediction_table;
    
    // Parametrii predictorului
    static const int HISTORY_SIZE = 4;  // Câte valori istorice să păstrăm
    static const int CONFIDENCE_THRESHOLD = 2;  // Prag pentru predicții sigure

    // Statistici pentru predicții
    long long total_predictions = 0;
    long long correct_predictions = 0;
    long long confident_predictions = 0;
    long long correct_confident_predictions = 0;

public:
    ValuePredictor();
    
    // Prezice valoarea pentru o instrucțiune
    bool predict(Uop* uop, long long& predicted_value);
    
    // Actualizează predictorul cu valoarea reală
    void update(Uop* uop, long long actual_value);
    
    // Verifică dacă o predicție este suficient de sigură
    bool isConfidentPrediction(Uop* uop);

    // Funcții pentru statistici
    void dumpStats(std::ostream& os) const;
};

}  // namespace x86

#endif  // X86_TIMING_VALUE_PREDICTOR_H 