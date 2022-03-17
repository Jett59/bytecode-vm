#include "assembler.h"
#include "bytecode.h"
#include "run.h"
#include <fstream>
#include <iostream>
#include <string>


using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::ofstream;
using std::string;

static void usage(char *programName) {
  cout << "Usage: " << programName << " asm|run file" << endl;
  cout << endl;
  cout << "asm Assemble the file." << endl;
  cout << "run Run the file." << endl;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    usage(argv[0]);
    return -1;
  }
  string action = argv[1];
  if (action == "asm") {
    string inputFile = argv[2];
    ifstream input(inputFile);
    ofstream output(inputFile + ".rvm", ios::binary);
    bytecode::assemble(input, output);
  } else if (action == "run") {
    string file = argv[2];
    ifstream input(file, ios::binary);
    input.seekg(0, ios::end);
    size_t fileSize = input.tellg();
    input.seekg(0, ios::beg);
    char *program = new char[fileSize];
    input.read(program, fileSize);
    bytecode::run(program, fileSize);
  }
  return 0;
}
