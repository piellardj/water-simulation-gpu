#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <string>
#include <algorithm>


/* Loads the file to container.
 * Throws an exception if a problem occured. */
void loadFile(std::string const& filePath,
              std::string& container);

/* Replaces every instance of toBeReplaced by replacement in container. */
void searchAndReplace (std::string const& tobeReplaced,
                       std::string const& replacement,
                       std::string& container);

template<typename T>
inline T clamp (T lowest, T heighest, T value)
{
    return std::min(heighest, std::max(lowest, value));
}
#endif // UTILITIES_HPP_INCLUDED
