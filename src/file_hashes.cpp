#include "file_hashes.h"

#include <cstring>
#include <ios>

FileHashes::FileHashes(std::filesystem::path path, std::uint64_t size, std::uint64_t blockSize,
                       HashAlgo algo)
    : path_(std::move(path)),
      blockSize_(blockSize),
      algo_(algo),
      buffer_(static_cast<std::size_t>(blockSize)) {
    const std::uint64_t count = size == 0 ? 0 : (size + blockSize_ - 1) / blockSize_;
    cache_.resize(static_cast<std::size_t>(count));
}

bool FileHashes::ensureOpen() {
    if (stream_.is_open()) {
        return true;
    }
    if (openAttempted_) {
        return false;
    }
    openAttempted_ = true;
    stream_.open(path_, std::ios::binary);
    return stream_.is_open();
}

std::optional<std::string> FileHashes::blockHash(std::size_t index) {
    if (index >= cache_.size()) {
        return std::nullopt;
    }
    if (cache_[index]) {
        return cache_[index];
    }
    if (!ensureOpen()) {
        return std::nullopt;
    }

    const auto offset = static_cast<std::streamoff>(index * blockSize_);
    stream_.clear();
    stream_.seekg(offset, std::ios::beg);
    if (!stream_) {
        return std::nullopt;
    }

    stream_.read(buffer_.data(), static_cast<std::streamsize>(buffer_.size()));
    const std::streamsize received = stream_.gcount();
    if (received < 0) {
        return std::nullopt;
    }
    const auto got = static_cast<std::size_t>(received);
    if (got < buffer_.size()) {
        std::memset(buffer_.data() + got, 0, buffer_.size() - got);
    }

    cache_[index] = hashBlock(buffer_.data(), buffer_.size(), algo_);
    return cache_[index];
}
