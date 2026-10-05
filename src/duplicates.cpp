#include "duplicates.h"

#include "file_hashes.h"

#include <map>
#include <memory>
#include <utility>

namespace {

struct Group {
    std::unique_ptr<FileHashes> representative;
    std::vector<std::filesystem::path> members;
};

bool equalContent(FileHashes& a, FileHashes& b) {
    const std::size_t count = a.blockCount();
    if (count != b.blockCount()) {
        return false;
    }
    for (std::size_t i = 0; i < count; ++i) {
        const auto ha = a.blockHash(i);
        if (!ha) {
            return false;
        }
        const auto hb = b.blockHash(i);
        if (!hb) {
            return false;
        }
        if (*ha != *hb) {
            return false;
        }
    }
    return true;
}

}  // namespace

std::vector<std::vector<std::filesystem::path>> findDuplicates(
    const std::vector<ScannedFile>& files, std::uint64_t blockSize, HashAlgo algo) {
    std::map<std::uint64_t, std::vector<Group>> bySize;

    for (const auto& file : files) {
        auto& groups = bySize[file.size];
        auto candidate = std::make_unique<FileHashes>(file.path, file.size, blockSize, algo);
        bool joined = false;
        for (auto& group : groups) {
            if (equalContent(*candidate, *group.representative)) {
                group.members.push_back(file.path);
                joined = true;
                break;
            }
        }
        if (!joined) {
            Group group;
            group.representative = std::move(candidate);
            group.members.push_back(file.path);
            groups.push_back(std::move(group));
        }
    }

    std::vector<std::vector<std::filesystem::path>> result;
    for (auto& entry : bySize) {
        for (auto& group : entry.second) {
            if (group.members.size() > 1) {
                result.push_back(std::move(group.members));
            }
        }
    }
    return result;
}
