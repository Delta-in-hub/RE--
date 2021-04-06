#ifndef RE__
#define RE__
#include "../dfaRE--/dfaRE--.hpp"
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>

namespace RE
{
class Regex : protected RE::dfaRE
{
  public:
    /*
    * [xyz] (x|y|z)
    * [0-3] (0|1|2|3)
    * [0-2a-c] (0|1|2|a|b|c)
    * \w [A-Za-z0-9_]
    * a{3} aaa
    * a{3,5} aaa|aaaa|aaaaa
    * ||转义...
    * \d  [0-9]
    * \n char(10)
    */
    std::string parse(const std::string& source);
    Regex(const std::string& str)
        : RE::dfaRE(parse(str))
    {
        ;
    }
    bool match(const std::string& tar)
    {
        return dfaRE::match(tar);
    }
};

} // namespace RE

#endif