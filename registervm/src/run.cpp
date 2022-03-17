#include "run.h"
#include "bytecode.h"
#include <cstddef>
#include <cstdint>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

namespace bytecode {
typedef int64_t Word;
#define MEMORY_SIZE 2048
struct Vm {
  Instruction *instructions;
  size_t ip;
  Word registers[NUM_REGISTERS];
  Word memory[MEMORY_SIZE];
};
static Vm vm;

static inline Word readImmediate(Instruction &instruction) {
  int16_t immediate = instruction.immediate;
  return immediate;
}
static inline Word readSrc1(Instruction &instruction) {
  if (instruction.src1UsesMemory) {
    return vm.memory[vm.registers[instruction.src1]];
  } else {
    return vm.registers[instruction.src1];
  }
}
static inline Word readSrc2(Instruction &instruction) {
  if (instruction.src2UsesMemory) {
    return vm.memory[vm.registers[instruction.src2]];
  } else {
    return vm.registers[instruction.src2];
  }
}
static inline Word readDst(Instruction &instruction) {
    if (instruction.dstUsesMemory) {
      return vm.memory[vm.registers[instruction.dst]];
    }else {
      return vm.registers[instruction.dst];
    }
}

static inline Word readSingleOperand(Instruction &instruction) {
    if (instruction.hasImmediate) {
      return readImmediate(instruction);
    }else {
      return readDst(instruction);
    }
}
static inline void read1Source(Instruction &instruction, Word *src) {
  if (instruction.hasImmediate) {
    *src = readImmediate(instruction);
  } else {
    *src = readSrc1(instruction);
  }
}
static inline void read2Sources(Instruction &instruction, Word *src1,
                                Word *src2) {
  if (instruction.hasImmediate) {
    *src1 = readImmediate(instruction);
  } else {
    *src1 = readSrc1(instruction);
  }
  *src2 = readSrc2(instruction);
}
static inline void writeDestination(Instruction &instruction, Word value) {
    if (instruction.dstUsesMemory) {
      vm.memory[vm.registers[instruction.dst]] = value;
    }else {
      vm.registers[instruction.dst] = value;
    }
}

void run(void *program, size_t programSize) {
  (void)programSize;
  Header *header = (Header *)program;
  if (header->magic != BYTECODE_MAGIC) {
    cerr << "Not an rvm file" << endl;
    return;
  }
  vm.instructions = (Instruction *)(header + 1);
  while (true) {
    Instruction instruction = vm.instructions[vm.ip++];
    switch ((Opcode)instruction.opcode) {
    case Opcode::MOV: {
      Word src;
      read1Source(instruction, &src);
      writeDestination(instruction, src);
      break;
    }
    case Opcode::ADD: {
      Word src1;
      Word src2;
      read2Sources(instruction, &src1, &src2);
      writeDestination(instruction, src1 + src2);
      break;
    }
    case Opcode::SUB: {
      Word src1;
      Word src2;
      read2Sources(instruction, &src1, &src2);
      writeDestination(instruction, src2 - src1);
      break;
    }
    case Opcode::MUL: {
      Word src1;
      Word src2;
      read2Sources(instruction, &src1, &src2);
      writeDestination(instruction, src1 * src2);
      break;
    }
    case Opcode::DIV: {
      Word src1;
      Word src2;
      read2Sources(instruction, &src1, &src2);
      if (src1 == 0) {
        cerr << "Divide by zero" << endl;
        return;
      }
      writeDestination(instruction, src2 / src1);
      break;
    }
    case Opcode::GOTO: {
      Word offset = readSingleOperand(instruction);
      vm.ip = offset;
      break;
    }
    case Opcode::PRINT: {
      Word value = readSingleOperand(instruction);
      cout << value << endl;
      break;
    }
    case Opcode::EXIT: {
      return;
    }
    }
  }
}
} // namespace bytecode
