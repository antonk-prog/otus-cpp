#pragma once

#include "hashes.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

class FileHashes {
public:
    FileHashes(std::filesystem::path path, std::uint64_t size, std::uint64_t blockSize, HashAlgo algo);

    FileHashes(const FileHashes&) = delete;
    FileHashes& operator=(const FileHashes&) = delete;

    std::size_t blockCount() const noexcept { return cache_.size(); }

    std::optional<std::string> blockHash(std::size_t index);

private:
    bool ensureOpen();

    std::filesystem::path path_;
    std::uint64_t blockSize_;
    HashAlgo algo_;
    std::ifstream stream_;
    bool openAttempted_ = false;
    std::vector<char> buffer_;
    std::vector<std::optional<std::string>> cache_;
};
