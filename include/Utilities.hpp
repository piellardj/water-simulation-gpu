#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <string>

void loadFile(std::string const& filePath,
              std::string& container);

void searchAndReplace (std::string const& tobeReplaced,
                       std::string const& replacement,
                       std::string& container);

#endif // UTILITIES_HPP_INCLUDED
