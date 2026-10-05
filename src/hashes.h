#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

enum class HashAlgo {
    Crc32,
    Md5,
};

HashAlgo parseHashAlgo(std::string_view name);

std::uint64_t hashBlock(const void* data, std::size_t size, HashAlgo algo);
