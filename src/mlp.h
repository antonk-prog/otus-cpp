#pragma once

#include <string>
#include <vector>

#include "dataset.h"

constexpr int kHidden = 128;
constexpr int kClasses = 10;

class Mlp {
public:
    void load(const std::string& modelDir);

    int predict(const Sample& s) const;

    double evaluate(const std::vector<Sample>& samples) const;

private:
    std::vector<float> w1_;
    std::vector<float> w2_;
};
