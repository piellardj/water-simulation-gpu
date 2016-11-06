#include "Utilities.hpp"

#include <stdexcept>
#include <fstream>
#include <sstream>

void loadFile(std::string const& filePath,
              std::string& container)
{
    std::ifstream t(filePath);
    if (!t.is_open())
        throw std::runtime_error("unable to open file " + filePath + ".");

    std::stringstream buffer;
    buffer << t.rdbuf();
    container = buffer.str();
}

void searchAndReplace (std::string const& tobeReplaced,
                       std::string const& replacement,
                       std::string& container)
{
  std::string::size_type pos = 0u;
  while((pos = container.find(tobeReplaced, pos)) != std::string::npos){
     container.replace(pos, tobeReplaced.length(), replacement);
     pos += replacement.length();
  }
}
