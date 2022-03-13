#include "run.h"
#include "bytecode.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::istream;

#define STACK_SIZE 256

namespace bytecode {
struct VmContext {
  uint8_t *instructions;
  size_t ip;
  Constant stack[STACK_SIZE];
  size_t stackPointer;
};
template<typename T> T readInstruction(VmContext &vm) {
  T &result = *((T *)(vm.instructions + vm.ip));
  vm.ip += sizeof(T);
  return result;
}
static void push(VmContext &vm, Constant value) { vm.stack[vm.stackPointer++] = value; }
static Constant pop(VmContext &vm) { return vm.stack[--vm.stackPointer]; }
void run(void *program, size_t programSize) {
  Header *header = (Header *)program;
  if (header->magic != BYTECODE_MAGIC) {
    cerr << "This is not a bytecode file" << endl;
    return;
  }
  if (header->version != CURRENT_BYTECODE_VERSION) {
    cerr << "Get the right version of the interpreter" << endl;
    return;
  }
  Constant *constants = (Constant *)((uint8_t *)program + sizeof(Header));
  uint8_t *instructions = (uint8_t *)(constants + header->constantCount);
  size_t instructionBytes = programSize - (instructions - program);
  VmContext context;
  context.instructions = instructions;
  context.ip = 0;
  context.stackPointer = 0;
  while (context.ip < instructionBytes) {
    Opcode opcode = readInstruction<Opcode>(context);
    switch (opcode) {
        case Opcode::IPUSH_CONST: {
          uint16_t index = readInstruction<uint16_t>(context);
          push(context, constants[index]);
          break;
        }
        case Opcode::IPUSH_IMM: {
          int16_t immediate = readInstruction<int16_t>(context);
          push(context, immediate);
          break;
        }
        case Opcode::EXIT: {
          Constant result = pop(context);
          cout << "Finished with " << result << endl;
          return;
        }
        default:
          cerr << "Unknown instruction" << endl;
          return;
    }
  }
  cerr << "End of instruction stream" << endl;
}
} // namespace bytecode
