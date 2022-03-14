#ifndef _BYTECODE_H
#define _BYTECODE_H

#include <cstdint>

namespace bytecode {
enum class Opcode : uint8_t {
  IPUSH_CONST,
  IPUSH_IMM,
  DUP,
  DROP,
  ADD,
  IADD,
  SUB,
  ISUB,
  MUL,
  IMUL,
  DIV,
  IDIV,
  LSHIFT,
  ILSHIFT,
  RSHIFT,
  IRSHIFT,
  LLOAD,
  LSTORE,
  GOTO,
  CGOTO_EQ,
  CGOTO_NEQ,
  CGOTO_GT,
  CGOTO_LT,
  EXIT
};

#define BYTECODE_MAGIC 0xD74EF7F3
#define CURRENT_BYTECODE_VERSION 1

struct Header {
  uint32_t magic;
  uint16_t version;
  uint16_t constantCount;
};
typedef int64_t Constant;
} // namespace bytecode

#endif