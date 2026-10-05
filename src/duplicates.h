#pragma once

#include "hashes.h"
#include "scan.h"

#include <cstdint>
#include <filesystem>
#include <vector>

std::vector<std::vector<std::filesystem::path>> findDuplicates(
    const std::vector<ScannedFile>& files, std::uint64_t blockSize, HashAlgo algo);
