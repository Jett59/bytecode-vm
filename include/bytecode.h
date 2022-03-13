#ifndef _BYTECODE_H
#define _BYTECODE_H

#include <cstdint>

namespace bytecode {
enum class Opcode : uint8_t { IPUSH, EXIT };

#define BYTECODE_MAGIC 0xD74EF7F3
#define CURRENT_BYTECODE_VERSION 0

struct Header {
  uint32_t magic;
  uint16_t version;
  uint16_t constantCount;
};
} // namespace bytecode

typedef uint64_t Constant;

#endif