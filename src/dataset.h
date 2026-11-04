#pragma once

#include <string>
#include <vector>

constexpr int kImageSize = 28;
constexpr int kPixels = kImageSize * kImageSize;

struct Sample {
    int label;
    std::vector<unsigned char> pixels;
};

std::vector<Sample> loadTestData(const std::string& path);
