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
#define MAX_LOCALS 256

namespace bytecode {
struct VmContext {
  uint8_t *instructions;
  size_t ip;
  Constant stack[STACK_SIZE];
  size_t stackPointer;
  Constant locals[MAX_LOCALS];
};
template <typename T> static inline T readInstruction(VmContext &vm) {
  T &result = *((T *)(vm.instructions + vm.ip));
  vm.ip += sizeof(T);
  return result;
}
static inline void push(VmContext &vm, Constant value) {
  vm.stack[vm.stackPointer++] = value;
}
static inline Constant pop(VmContext &vm) {
  return vm.stack[--vm.stackPointer];
}

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
  size_t instructionBytes = programSize - (instructions - (uint8_t *)program);
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
    case Opcode::DUP: {
      Constant value = pop(context);
      push(context, value);
      push(context, value);
      break;
    }
    case Opcode::DROP: {
      pop(context);
      break;
    }
    case Opcode::ADD: {
      Constant right = pop(context);
      Constant left = pop(context);
      push(context, left + right);
      break;
    }
    case Opcode::IADD: {
      int16_t right = readInstruction<int16_t>(context);
      Constant left = pop(context);
      push(context, left + right);
      break;
    }
    case Opcode::SUB: {
      Constant right = pop(context);
      Constant left = pop(context);
      push(context, left - right);
      break;
    }
    case Opcode::ISUB: {
      int16_t right = readInstruction<int16_t>(context);
      Constant left = pop(context);
      push(context, left - right);
      break;
    }
    case Opcode::MUL: {
      Constant right = pop(context);
      Constant left = pop(context);
      push(context, left * right);
      break;
    }
    case Opcode::IMUL: {
      int16_t right = readInstruction<int16_t>(context);
      Constant left = pop(context);
      push(context, left * right);
      break;
    }
    case Opcode::DIV: {
      Constant right = pop(context);
      Constant left = pop(context);
      if (right != 0) {
        push(context, left / right);
      } else {
        cerr << "Divide by zero" << endl;
        return;
      }
      break;
    }
    case Opcode::IDIV: {
      int16_t right = readInstruction<int16_t>(context);
      Constant left = pop(context);
      if (right != 0) {
        push(context, left / right);
      }else {
        cerr << "Divide by zero" << endl;
        return;
      }
      break;
    }
    case Opcode::LSHIFT: {
      Constant right = pop(context);
      Constant left = pop(context);
      push(context, left << right);
      break;
    }
    case Opcode::ILSHIFT: {
      int16_t right = readInstruction<int16_t>(context);
      Constant left = pop(context);
      push(context, left << right);
      break;
    }
    case Opcode::RSHIFT: {
      Constant right = pop(context);
      Constant left = pop(context);
      push(context, left >> right);
      break;
    }
    case Opcode::IRSHIFT: {
      int16_t right = readInstruction<int16_t>(context);
      Constant left = pop(context);
      push(context, left >> right);
      break;
    }
    case Opcode::LLOAD: {
      uint16_t index = readInstruction<uint16_t>(context);
      push(context, context.locals[index]);
      break;
    }
    case Opcode::LSTORE: {
      uint16_t index = readInstruction<uint16_t>(context);
      context.locals[index] = pop(context);
      break;
    }
    case Opcode::GOTO: {
      uint16_t offset = readInstruction<uint16_t>(context);
      context.ip = offset;
      break;
    }
    case Opcode::CGOTO_EQ: {
      uint16_t offset = readInstruction<uint16_t>(context);
      Constant right = pop(context);
      Constant left = pop(context);
      if (left == right) {
        context.ip = offset;
      }
      break;
    }
    case Opcode::CGOTO_NEQ: {
      uint16_t offset = readInstruction<uint16_t>(context);
      Constant right = pop(context);
      Constant left = pop(context);
      if (left != right) {
        context.ip = offset;
      }
      break;
    }
    case Opcode::CGOTO_GT: {
      uint16_t offset = readInstruction<uint16_t>(context);
      Constant right = pop(context);
      Constant left = pop(context);
      if (left > right) {
        context.ip = offset;
      }
      break;
    }
    case Opcode::CGOTO_LT: {
      uint16_t offset = readInstruction<uint16_t>(context);
      Constant right = pop(context);
      Constant left = pop(context);
      if (left < right) {
        context.ip = offset;
      }
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
