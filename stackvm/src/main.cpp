#include "assembler.h"
#include "helper.h"
#include "run.h"
#include <chrono>
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
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

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
  } else {
    ifstream input(file, ios::binary);
    size_t fileSize = getFileSize(input);
    char *fileBytes = new char[fileSize];
    input.read(fileBytes, fileSize);
    auto startTime = high_resolution_clock::now();
    bytecode::run(fileBytes, fileSize);
    auto timeTaken = high_resolution_clock::now() - startTime;
    cout << "It took " << duration_cast<milliseconds>(timeTaken).count() / 1000.0
         << " seconds" << endl;
  }
}
