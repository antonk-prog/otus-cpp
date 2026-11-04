#include "io.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void die(const std::string& msg) {
    std::fprintf(stderr, "error: %s\n", msg.c_str());
    std::exit(EXIT_FAILURE);
}

std::vector<char> readFile(const std::string& path) {
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) die("cannot open " + path + ": " + std::strerror(errno));
    std::fseek(f, 0, SEEK_END);
    long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (size < 0) die("cannot determine size of " + path);
    std::vector<char> buf(static_cast<size_t>(size) + 1);
    size_t read = std::fread(buf.data(), 1, static_cast<size_t>(size), f);
    std::fclose(f);
    if (read != static_cast<size_t>(size)) die("failed to read " + path);
    buf[size] = '\0';
    return buf;
}

char* skipSeparators(char* q) {
    while (*q == ' ' || *q == '\t' || *q == ',') ++q;
    return q;
}

std::vector<float> loadMatrix(const std::string& path, int rows, int cols) {
    std::vector<char> buf = readFile(path);
    std::vector<float> w(static_cast<size_t>(rows) * cols);
    char* p = buf.data();
    for (int r = 0; r < rows; ++r) {
        char* nl = std::strchr(p, '\n');
        char* next = nullptr;
        if (nl) {
            *nl = '\0';
            next = nl + 1;
        }
        size_t len = std::strlen(p);
        if (len > 0 && p[len - 1] == '\r') p[--len] = '\0';
        float* dst = w.data() + static_cast<size_t>(r) * cols;
        char* q = p;
        for (int c = 0; c < cols; ++c) {
            q = skipSeparators(q);
            char* end = nullptr;
            float v = std::strtof(q, &end);
            if (end == q) die("failed to parse value at row " + std::to_string(r) + " col " + std::to_string(c) + " in " + path);
            dst[c] = v;
            q = end;
        }
        p = next;
        if (r + 1 < rows && (!p || *p == '\0')) die("unexpected end of file " + path);
    }
    return w;
}
