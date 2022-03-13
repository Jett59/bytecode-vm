#include "assembler.h"
#include "bytecode.h"
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
using std::stringbuf;
using std::string;
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
      if (value < 65536) {
        pushInstruction(instructions, Opcode::IPUSH_IMM);
        pushInstruction<uint16_t>(instructions, value);
      } else {
        if (constants.size() >= 65535) {
          cerr << "Too many constants in program" << endl;
          return;
        }
        pushInstruction(instructions, Opcode::IPUSH_CONST);
        pushInstruction<uint16_t>(instructions, constants.size());
        constants.push_back(value);
      }
    }else if (word == "exit") {
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
