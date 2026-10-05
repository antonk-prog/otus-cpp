#include "file_hashes.h"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

FileHashes::FileHashes(std::filesystem::path path, std::uint64_t size, std::uint64_t blockSize,
                       HashAlgo algo)
    : path_(std::move(path)),
      blockSize_(blockSize),
      algo_(algo),
      buffer_(static_cast<std::size_t>(blockSize)) {
    const std::uint64_t count = size == 0 ? 0 : (size + blockSize_ - 1) / blockSize_;
    cache_.resize(static_cast<std::size_t>(count));
}

FileHashes::~FileHashes() {
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

bool FileHashes::ensureOpen() {
    if (fd_ >= 0) {
        return true;
    }
    if (openAttempted_) {
        return false;
    }
    openAttempted_ = true;
    fd_ = ::open(path_.c_str(), O_RDONLY | O_CLOEXEC);
    return fd_ >= 0;
}

std::optional<std::uint64_t> FileHashes::blockHash(std::size_t index) {
    if (index >= cache_.size()) {
        return std::nullopt;
    }
    if (cache_[index]) {
        return cache_[index];
    }
    if (!ensureOpen()) {
        return std::nullopt;
    }

    const off_t offset = static_cast<off_t>(index * blockSize_);
    std::size_t received = 0;
    while (received < buffer_.size()) {
        const ssize_t n =
            ::pread(fd_, buffer_.data() + received, buffer_.size() - received, offset + received);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return std::nullopt;
        }
        if (n == 0) {
            break;
        }
        received += static_cast<std::size_t>(n);
    }
    if (received < buffer_.size()) {
        std::memset(buffer_.data() + received, 0, buffer_.size() - received);
    }

    cache_[index] = hashBlock(buffer_.data(), buffer_.size(), algo_);
    return cache_[index];
}
