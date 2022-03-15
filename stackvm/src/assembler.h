#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include <fstream>

namespace bytecode {
void assemble(std::istream &input, std::ostream &output);
}

#endif