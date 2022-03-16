#include "assembler.h"
#include "bytecode.h"
#include <cctype>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using std::cerr;
using std::endl;
using std::exception;
using std::invalid_argument;
using std::istream;
using std::map;
using std::ostream;
using std::range_error;
using std::stoi;
using std::string;
using std::vector;

#define MAXLINE 256

namespace bytecode {
static inline char *skipIndent(char *str) {
  while (isspace(*str)) {
    str++;
  }
  return str;
}

string nextWord(char *&str) {
  char c = *str++;
  while (isspace(c)) {
    c = *str++;
  }
  string word;
  while (!isspace(c) && c != 0) {
    word += c;
    c = *str++;
  }
  return word;
}

enum class OperandType { REGISTER, IMMEDIATE };

struct Operand {
  OperandType type;
  bool memoryOperand;
  union {
    uint8_t registerNumber;
    int16_t immediateValue;
  } value;
};

vector<Operand> getOperands(char *&line) {
  vector<Operand> result;
  string word = nextWord(line);
  if (word.length() > 0) {
    bool moreOperands = true;
    while (moreOperands) {
      moreOperands = word.at(word.length() - 1) == ',';
      if (moreOperands) {
        word = word.substr(0, word.length() - 1);
      }
      Operand operand;
      if (word.at(0) == '*') {
        operand.memoryOperand = true;
        word = word.substr(1);
      } else {
        operand.memoryOperand = false;
      }
      if (word.at(0) == 'r') {
        operand.type = OperandType::REGISTER;
        operand.value.registerNumber = stoi(word.substr(1));
      } else {
        if (operand.memoryOperand) {
          throw invalid_argument("Dereferenced immediate");
        }
        operand.type = OperandType::IMMEDIATE;
        operand.value.immediateValue = stoi(word);
      }
      result.push_back(operand);
      if (moreOperands) {
        word = nextWord(line);
      }
    }
  }
  if (nextWord(line).length() != 0) {
    cerr << "Warning: Characters at end of line" << endl;
  }
  return result;
}

static map<string, Opcode> mnemonics = {{"mov", Opcode::MOV},
                                        {"add", Opcode::ADD},
                                        {"sub", Opcode::SUB},
                                        {"mul", Opcode::MUL},
                                        {"div", Opcode::DIV}};

void assemble(istream &input, ostream &output) {
  try {
    vector<Instruction> instructions;
    char inputLine[MAXLINE];
    input.getline(inputLine, MAXLINE);
    while (!input.eof()) {
      char *line = skipIndent(inputLine);
      int lineLength = strlen(line);
      if (lineLength != 0 && *line != '#') {
        string mnemonic = nextWord(line);
        Opcode opcode = mnemonics[mnemonic];
        vector<Operand> operands = getOperands(line);
        Instruction instruction = {0};
        instruction.opcode = (uint8_t)opcode;
        switch (operands.size()) {
        case 0: {
          break;
        }
        case 1: {
          Operand operand = operands.at(0);
          if (operand.type == OperandType::IMMEDIATE) {
            instruction.hasImmediate = true;
            instruction.immediate = operand.value.immediateValue;
          } else {
            instruction.hasImmediate = false;
            instruction.dst = operand.value.registerNumber;
            instruction.dstUsesMemory = operand.memoryOperand;
          }
          break;
        }
        case 2: {
          Operand srcOperand = operands.at(0);
          Operand dstOperand = operands.at(1);
          if (dstOperand.type != OperandType::REGISTER) {
            throw invalid_argument("Destination operand is immediate");
          }
          if (srcOperand.type == OperandType::IMMEDIATE) {
            instruction.hasImmediate = true;
            instruction.immediate = srcOperand.value.immediateValue;
          } else {
            instruction.hasImmediate = false;
            instruction.src1 = srcOperand.value.registerNumber;
            instruction.src1UsesMemory = srcOperand.memoryOperand;
          }
          instruction.dst = dstOperand.value.registerNumber;
          instruction.dstUsesMemory = dstOperand.memoryOperand;
          break;
        }
        case 3: {
          Operand src1Operand = operands.at(0);
          Operand src2Operand = operands.at(1);
          Operand dstOperand = operands.at(2);
          if (src2Operand.type != OperandType::REGISTER) {
            throw invalid_argument("Source operand two is immediate");
          }
          if (dstOperand.type != OperandType::REGISTER) {
            throw invalid_argument("Destination operand is immediate");
          }
          if (src1Operand.type == OperandType::IMMEDIATE) {
            instruction.hasImmediate = true;
            instruction.immediate = src1Operand.value.immediateValue;
          } else {
            instruction.hasImmediate = false;
            instruction.src1 = src1Operand.value.registerNumber;
            instruction.src1UsesMemory = src1Operand.memoryOperand;
          }
          instruction.src2 = src2Operand.value.registerNumber;
          instruction.src2UsesMemory = src2Operand.memoryOperand;
          instruction.dst = dstOperand.value.registerNumber;
          instruction.dstUsesMemory = dstOperand.memoryOperand;
          break;
        }
        default:
          throw invalid_argument("More than three operands");
        }
        instructions.push_back(instruction);
      }
      input.getline(inputLine, MAXLINE);
    }
    for (Instruction &instruction : instructions) {
      output.write((char *)&instruction, sizeof(Instruction));
    }
  } catch (exception &e) {
    cerr << "Error: " << e.what() << endl;
  }
}
} // namespace bytecode
