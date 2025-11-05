#pragma once
#include <cstdint>

namespace Types {

static const std::uint32_t MAGIC_NUMBER = 0xDEADBEAF;

struct Header {
    std::uint32_t magic;
    std::uint32_t version;
    std::uint64_t manifestOffset;
    std::uint64_t manifestSize;
    std::uint64_t manifestCompressedSize;
};

}