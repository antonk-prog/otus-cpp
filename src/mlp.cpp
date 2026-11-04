#include "mlp.h"

#include <cmath>
#include <filesystem>

#include "io.h"

void Mlp::load(const std::string& modelDir) {
    std::filesystem::path dir(modelDir);
    if (!std::filesystem::is_directory(dir)) die(modelDir + " is not a directory with w1.txt and w2.txt");
    w1_ = loadMatrix((dir / "w1.txt").string(), kPixels, kHidden);
    w2_ = loadMatrix((dir / "w2.txt").string(), kHidden, kClasses);
}

int Mlp::predict(const Sample& s) const {
    float hidden[kHidden] = {0.0f};
    for (int i = 0; i < kPixels; ++i) {
        float xi = s.pixels[i] / 255.0f;
        if (xi == 0.0f) continue;
        const float* row = w1_.data() + static_cast<size_t>(i) * kHidden;
        for (int j = 0; j < kHidden; ++j) hidden[j] += xi * row[j];
    }
    for (int j = 0; j < kHidden; ++j) hidden[j] = 1.0f / (1.0f + std::exp(-hidden[j]));

    float out[kClasses] = {0.0f};
    for (int j = 0; j < kHidden; ++j) {
        const float* row = w2_.data() + static_cast<size_t>(j) * kClasses;
        float hj = hidden[j];
        for (int k = 0; k < kClasses; ++k) out[k] += hj * row[k];
    }

    int best = 0;
    for (int k = 1; k < kClasses; ++k) {
        if (out[k] > out[best]) best = k;
    }
    return best;
}

double Mlp::evaluate(const std::vector<Sample>& samples) const {
    size_t correct = 0;
    for (const Sample& s : samples) {
        if (predict(s) == s.label) ++correct;
    }
    return static_cast<double>(correct) / static_cast<double>(samples.size());
}
