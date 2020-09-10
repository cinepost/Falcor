#ifndef LAVA_UTILS_STRING_H_
#define LAVA_UTILS_STRING_H_

//#include <regex>
#include <string>

namespace lava { namespace ut { namespace string {

std::string replace(const std::string& str, const std::string& from, const std::string& to);

}}} // namespace lava::ut::string

#endif // LAVA_UTILS_STRING_H_