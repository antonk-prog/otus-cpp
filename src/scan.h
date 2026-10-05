#pragma once

#include "hashes.h"

#include <cstdint>
#include <filesystem>
#include <vector>

struct ScanOptions {
    std::vector<std::filesystem::path> paths;
    std::vector<std::filesystem::path> exclude;
    int level = -1;
    std::uint64_t minSize = 1;
    std::vector<std::string> masks;
    std::uint64_t blockSize = 4096;
    HashAlgo algo = HashAlgo::Crc32;
};

struct ScannedFile {
    std::filesystem::path path;
    std::uint64_t size;
};

std::vector<ScannedFile> scanFiles(const ScanOptions& options);
