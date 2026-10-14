#pragma once

#include <cstddef>
#include <string>
#include <string_view>

enum class HashAlgo {
    Crc32,
    Md5,
};

HashAlgo parseHashAlgo(std::string_view name);

std::string hashBlock(const void* data, std::size_t size, HashAlgo algo);
