#include "dataset.h"

#include <cstring>
#include <utility>

#include "io.h"

std::vector<Sample> loadTestData(const std::string& path) {
    std::vector<char> buf = readFile(path);
    std::vector<Sample> samples;
    char* p = buf.data();
    while (p && *p) {
        char* nl = std::strchr(p, '\n');
        char* next = nullptr;
        if (nl) {
            *nl = '\0';
            next = nl + 1;
        }
        size_t len = std::strlen(p);
        if (len > 0 && p[len - 1] == '\r') p[--len] = '\0';
        if (len > 0) {
            Sample s;
            s.pixels.resize(kPixels);
            char* q = skipSeparators(p);
            char* numEnd = nullptr;
            long label = std::strtol(q, &numEnd, 10);
            if (numEnd == q) die("failed to parse label in " + path);
            s.label = static_cast<int>(label);
            q = numEnd;
            for (int c = 0; c < kPixels; ++c) {
                q = skipSeparators(q);
                long v = std::strtol(q, &numEnd, 10);
                if (numEnd == q) die("failed to parse pixel " + std::to_string(c) + " in " + path);
                q = numEnd;
                if (v < 0 || v > 255) die("pixel value out of range in " + path);
                s.pixels[static_cast<size_t>(c)] = static_cast<unsigned char>(v);
            }
            samples.push_back(std::move(s));
        }
        p = next;
    }
    if (samples.empty()) die("no samples in " + path);
    return samples;
}
