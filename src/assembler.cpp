#include "assembler.h"
#include "bytecode.h"
#include "helper.h"
#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using std::cerr;
using std::endl;
using std::istream;
using std::map;
using std::numeric_limits;
using std::ostream;
using std::streamsize;
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
  map<string, uint16_t> labels;
  map<string, vector<uint16_t>> labelReferences;
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
    } else if (word == "dup") {
      pushInstruction(instructions, Opcode::DUP);
    } else if (word == "drop") {
      pushInstruction(instructions, Opcode::DROP);
    } else if (word == "add") {
      pushInstruction(instructions, Opcode::ADD);
    }else if (word == "iadd") {
      int16_t value;
      input >> value;
      pushInstruction(instructions, Opcode::IADD);
      pushInstruction(instructions, value);
    } else if (word == "sub") {
      pushInstruction(instructions, Opcode::SUB);
    }else if (word == "isub") {
      int16_t value;
      input >> value;
      pushInstruction(instructions, Opcode::ISUB);
      pushInstruction(instructions, value);
    } else if (word == "mul") {
      pushInstruction(instructions, Opcode::MUL);
    }else if (word == "imul") {
      int16_t value;
      input >> value;
      pushInstruction(instructions, Opcode::IMUL);
      pushInstruction(instructions, value);
    } else if (word == "div") {
      pushInstruction(instructions, Opcode::DIV);
    }else if (word == "idiv") {
      int16_t value;
      input >> value;
      pushInstruction(instructions, Opcode::IDIV);
      pushInstruction(instructions, value);
    }else if (word == "lshift") {
      pushInstruction(instructions, Opcode::LSHIFT);
    }else if (word == "ilshift") {
      int16_t value;
      input >> value;
      pushInstruction(instructions, Opcode::ILSHIFT);
      pushInstruction(instructions, value);
    }else if (word == "rshift") {
      pushInstruction(instructions, Opcode::RSHIFT);
    }else if (word == "irshift") {
      int16_t value;
      input >> value;
      pushInstruction(instructions, Opcode::IRSHIFT);
      pushInstruction(instructions, value);
    }else if (word == "and") {
      pushInstruction(instructions, Opcode::AND);
    }else if (word == "iand") {
      int16_t value;
      input >> value;
      pushInstruction(instructions, Opcode::IAND);
      pushInstruction(instructions, value);
    }else if (word == "or") {
      pushInstruction(instructions, Opcode::OR);
    }else if (word == "ior") {
      int16_t value;
      input >> value;
      pushInstruction(instructions, Opcode::IOR);
      pushInstruction(instructions, value);
    }else if (word == "xor") {
      pushInstruction(instructions, Opcode::XOR);
    }else if (word == "ixor") {
      int16_t value;
      input >> value;
      pushInstruction(instructions, Opcode::IXOR);
      pushInstruction(instructions, value);
    } else if (word == "lload") {
      uint16_t index;
      input >> index;
      pushInstruction(instructions, Opcode::LLOAD);
      pushInstruction(instructions, index);
    } else if (word == "lstore") {
      uint16_t index;
      input >> index;
      pushInstruction(instructions, Opcode::LSTORE);
      pushInstruction(instructions, index);
    } else if (word == "goto") {
      string label;
      input >> label;
      pushInstruction(instructions, Opcode::GOTO);
      labelReferences[label].push_back(instructions.str().length());
      pushInstruction<uint16_t>(instructions, 0);
    } else if (word == "cgoto_eq") {
      string label;
      input >> label;
      pushInstruction(instructions, Opcode::CGOTO_EQ);
      labelReferences[label].push_back(instructions.str().length());
      pushInstruction<uint16_t>(instructions, 0);
    } else if (word == "cgoto_neq") {
      string label;
      input >> label;
      pushInstruction(instructions, Opcode::CGOTO_NEQ);
      labelReferences[label].push_back(instructions.str().length());
      pushInstruction<uint16_t>(instructions, 0);
    } else if (word == "cgoto_gt") {
      string label;
      input >> label;
      pushInstruction(instructions, Opcode::CGOTO_GT);
      labelReferences[label].push_back(instructions.str().length());
      pushInstruction<uint16_t>(instructions, 0);
    } else if (word == "cgoto_lt") {
      string label;
      input >> label;
      pushInstruction(instructions, Opcode::CGOTO_LT);
      labelReferences[label].push_back(instructions.str().length());
      pushInstruction<uint16_t>(instructions, 0);
    } else if (word == "exit") {
      pushInstruction(instructions, Opcode::EXIT);
    } else if (startsWith(word, "#")) {
      input.ignore(numeric_limits<streamsize>::max(), input.widen('\n'));
    } else if (endsWith(word, ":")) {
      labels[word.substr(0, word.length() - 1)] = instructions.str().length();
    } else if (word == "") {
      continue;
    } else {
      cerr << "Unknown opcode " << word << endl;
      return;
    }
  }
  header.constantCount = constants.size();
  output.write((const char *)&header, sizeof(Header));
  for (Constant constant : constants) {
    output.write((const char *)&constant, sizeof(Constant));
  }
  string instructionString = instructions.str();
  for (const auto &label : labelReferences) {
    uint16_t labelLocation = labels[label.first];
    uint8_t labelLocationLow = labelLocation & 0xff;
    uint8_t labelLocationHigh = (uint8_t)(labelLocation >> 8);
    for (const auto &reference : label.second) {
      // Little endian - low byte first
      instructionString[reference] = labelLocationLow;
      instructionString[reference + 1] = labelLocationHigh;
    }
  }
  output << instructionString;
}
} // namespace bytecode
