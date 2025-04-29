#ifndef ARCH_X86_TIMING_VALUE_PREDICTOR_H
#define ARCH_X86_TIMING_VALUE_PREDICTOR_H

#include "Predictor.h"

namespace x86
{

class ValuePredictor : public Predictor
{
private:
    static const int HISTORY_SIZE = 4;
    static const int CONFIDENCE_THRESHOLD = 2;

public:
    ValuePredictor();
    bool predict(Uop* uop, long long& predicted_value) override;
    void update(Uop* uop, long long actual_value) override;
    bool isConfidentPrediction(Uop* uop) override;
    void dumpStats(std::ostream& os) const override;
};

}  // namespace x86

#endif  // ARCH_X86_TIMING_VALUE_PREDICTOR_H 