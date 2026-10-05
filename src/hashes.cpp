#include "hashes.h"

#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>

#include <algorithm>
#include <cctype>
#include <stdexcept>

HashAlgo parseHashAlgo(std::string_view name) {
    std::string lower(name);
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (lower == "crc32") {
        return HashAlgo::Crc32;
    }
    if (lower == "md5") {
        return HashAlgo::Md5;
    }
    throw std::invalid_argument("unknown hash algorithm: " + std::string(name));
}

std::uint64_t hashBlock(const void* data, std::size_t size, HashAlgo algo) {
    switch (algo) {
        case HashAlgo::Crc32: {
            boost::crc_32_type crc;
            crc.process_bytes(data, size);
            return static_cast<std::uint64_t>(crc.checksum());
        }
        case HashAlgo::Md5: {
            boost::uuids::detail::md5 md;
            md.process_bytes(data, size);
            boost::uuids::detail::md5::digest_type digest;
            md.get_digest(digest);
            std::uint64_t result = 0;
            for (int i = 0; i < 2; ++i) {
                result |= static_cast<std::uint64_t>(digest[i]) << (32 * i);
            }
            return result;
        }
    }
    return 0;
}
