#include "Topic.h"
#include "Tokenizer.h"

namespace {
int isSlash(int c)
{
  return c == '/';
}
}

Topic::Topic()
  : _topic {""}
{}

Topic::Topic(const std::string& root)
  : _topic {root}
{}

Topic::~Topic()
{}

std::string Topic::operator[] (const int nIndex)
{
  return split(_topic, isSlash)[nIndex];
}
