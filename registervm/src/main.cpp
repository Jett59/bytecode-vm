#include "bytecode.h"
#include "assembler.h"
#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::endl;
using std::ios;
using std::ifstream;
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
  }
  return 0;
}
