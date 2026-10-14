#include "hashes.h"

#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <stdexcept>

namespace {

std::string toHex(const unsigned char* bytes, std::size_t count) {
    static constexpr char kDigits[] = "0123456789abcdef";
    std::string out;
    out.reserve(count * 2);
    for (std::size_t i = 0; i < count; ++i) {
        out.push_back(kDigits[bytes[i] >> 4]);
        out.push_back(kDigits[bytes[i] & 0x0f]);
    }
    return out;
}

}  // namespace

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

std::string hashBlock(const void* data, std::size_t size, HashAlgo algo) {
    switch (algo) {
        case HashAlgo::Crc32: {
            boost::crc_32_type crc;
            crc.process_bytes(data, size);
            const std::uint32_t value = crc.checksum();
            const unsigned char bytes[4] = {
                static_cast<unsigned char>((value >> 24) & 0xff),
                static_cast<unsigned char>((value >> 16) & 0xff),
                static_cast<unsigned char>((value >> 8) & 0xff),
                static_cast<unsigned char>(value & 0xff),
            };
            return toHex(bytes, 4);
        }
        case HashAlgo::Md5: {
            boost::uuids::detail::md5 md;
            md.process_bytes(data, size);
            boost::uuids::detail::md5::digest_type digest;
            md.get_digest(digest);
            unsigned char bytes[16];
            for (int w = 0; w < 4; ++w) {
                const auto word = static_cast<std::uint32_t>(digest[w]);
                bytes[w * 4 + 0] = static_cast<unsigned char>((word >> 24) & 0xff);
                bytes[w * 4 + 1] = static_cast<unsigned char>((word >> 16) & 0xff);
                bytes[w * 4 + 2] = static_cast<unsigned char>((word >> 8) & 0xff);
                bytes[w * 4 + 3] = static_cast<unsigned char>(word & 0xff);
            }
            return toHex(bytes, 16);
        }
    }
    return {};
}
