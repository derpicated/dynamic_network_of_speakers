#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cctype>
#include <string>
#include <vector>

int isForwardSlash(int c);
// using a default input parameter: std::isspace
std::vector<std::string> split(const std::string& str, 
                               int delimiter(int) = std::isspace);

#endif
