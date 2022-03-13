#include "assembler.h"
#include "helper.h"
#include <fstream>
#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::ofstream;
using std::string;

static void usage(char *filePath) {
  cout << "Usage: " << filePath << " <file>" << endl;
  cout << endl;
  cout << "Arguments:" << endl;
  cout << "<file> the file to use. If the file ends in \".vasm\", it is "
          "assembled, otherwise it is run."
       << endl;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage(argv[0]);
    return -1;
  }
  string file(argv[1]);
  if (endsWith(file, ".vasm")) {
    ifstream input(file);
    ofstream output(file + ".bin", ios::binary);
    bytecode::assemble(input, output);
  }  else {
    cerr << "Running is not currently supported" << endl;
    return -1;
  }
}
