#ifndef _BYTECODE_H
#define _BYTECODE_H

#include <cstdint>

namespace bytecode {
struct Instruction {
  uint16_t opcode : 7;
  /*bool*/ uint16_t hasImmediate : 1;
  uint16_t src1 : 3;
  /*bool*/ uint16_t src1UsesMemory : 1;
  uint16_t src2 : 3;
  /*bool*/ uint16_t src2UsesMemory : 1;
  uint16_t dst : 3;
  /*bool*/ uint16_t dstUsesMemory : 1;
  uint16_t immediate : 12;
};
static_assert(sizeof(Instruction) == 4,
              "Instruction must be packed to four bytes");
enum class Opcode { MOV, ADD, SUB, MUL, DIV };
} // namespace bytecode

#endif