#include "assembler.h"
#include "bytecode.h"
#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cerr;
using std::endl;
using std::istream;
using std::ostream;
using std::string;
using std::stringbuf;
using std::vector;

namespace bytecode {
template <typename T> void pushInstruction(stringbuf &instructions, T value) {
  instructions.sputn((const char *)&value, sizeof(T));
}
void assemble(istream &input, ostream &output) {
  Header header;
  header.magic = BYTECODE_MAGIC;
  header.version = CURRENT_BYTECODE_VERSION;
  vector<Constant> constants;
  stringbuf instructions;
  while (!input.eof()) {
    string word;
    input >> word;
    if (word == "ipush") {
      Constant value;
      input >> value;
      if (value <= INT16_MAX && value >= INT16_MIN) {
        pushInstruction(instructions, Opcode::IPUSH_IMM);
        pushInstruction<int16_t>(instructions, value);
      } else {
        if (constants.size() >= 65535) {
          cerr << "Too many constants in program" << endl;
          return;
        }
        pushInstruction(instructions, Opcode::IPUSH_CONST);
        pushInstruction<int16_t>(instructions, constants.size());
        constants.push_back(value);
      }
    }else if (word == "add") {
      pushInstruction(instructions, Opcode::ADD);
    }else if (word == "sub") {
      pushInstruction(instructions, Opcode::SUB);
    }else if (word == "mul") {
      pushInstruction(instructions, Opcode::MUL);
    }else if (word == "div") {
      pushInstruction(instructions, Opcode::DIV);
    }else if (word == "lload") {
      uint16_t index;
      input >> index;
      pushInstruction(instructions, Opcode::LLOAD);
      pushInstruction(instructions, index);
    }else if (word == "lstore") {
      uint16_t index;
      input >> index;
      pushInstruction(instructions, Opcode::LSTORE);
      pushInstruction(instructions, index);
    } else if (word == "exit") {
      pushInstruction(instructions, Opcode::EXIT);
    }
  }
  header.constantCount = constants.size();
  output.write((const char *)&header, sizeof(Header));
  for (Constant constant : constants) {
    output.write((const char *)&constant, sizeof(Constant));
  }
  output << instructions.str();
}
} // namespace bytecode
