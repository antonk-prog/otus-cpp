#pragma once

#include "hashes.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

class FileHashes {
public:
    FileHashes(std::filesystem::path path, std::uint64_t size, std::uint64_t blockSize, HashAlgo algo);

    FileHashes(const FileHashes&) = delete;
    FileHashes& operator=(const FileHashes&) = delete;

    ~FileHashes();

    std::size_t blockCount() const noexcept { return cache_.size(); }

    std::optional<std::uint64_t> blockHash(std::size_t index);

private:
    bool ensureOpen();

    std::filesystem::path path_;
    std::uint64_t blockSize_;
    HashAlgo algo_;
    int fd_ = -1;
    bool openAttempted_ = false;
    std::vector<char> buffer_;
    std::vector<std::optional<std::uint64_t>> cache_;
};
