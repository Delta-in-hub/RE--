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
  protected:
    std::string parse(const std::string& source);

  public:
    Regex(const std::string& str, const size_t maxdstate = 32)
        : RE::dfaRE(parse(str), maxdstate)
    {
        ;
    }
    bool match(const std::string& tar)
    {
        return dfaRE::match(tar);
    }
};
std::string Regex::parse(const std::string& source)
{
    using namespace std;
    auto next = [&source](size_t& i) -> char {
        if (i + 1 < source.size())
            return source[++i];
        else
            throw std::invalid_argument(source + " at [" + std::to_string(i) + "]");
    };

    std::string res;
    for (size_t i = 0; i < source.length(); i++)
    {
        char now = source[i];
        std::string tmp;
        char from;
        bool hasEscape = false;
        switch (now)
        {
        case '[':
            do
            {
                now = next(i);
                if (now == ']')
                    break;
                else if (now == '-')
                {
                    if (tmp.empty())
                        // throw std::invalid_argument(source + " at [" + std::to_string(i) + "]");
                        tmp.push_back('-');
                    else
                    {
                        from = tmp.back();
                        now  = next(i);
                        if (now == ']')
                        {
                            tmp.push_back('-');
                            break;
                        }
                        while (from < now)
                        {
                            tmp.push_back(++from);
                        }
                    }
                }
                else if (now == '\\')
                {
                    now = next(i);
                    switch (now)
                    {
                    case '\\':
                        hasEscape = true;
                        break;
                    case 'w':
                        tmp.append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
                        break;
                    case 'd':
                        tmp.append("0123456789");
                        break;
                    case 'n':
                        tmp.push_back(10);
                        break;
                    case 't':
                        tmp.push_back(9);
                        break;
                    case '*':
                        tmp.push_back(18);
                        break;
                    case '.':
                        tmp.push_back(19);
                        break;
                    case '+':
                        tmp.push_back(20);
                        break;
                    case '?':
                        tmp.push_back(21);
                        break;
                    case '|':
                        tmp.push_back(22);
                        break;
                    case '(':
                        tmp.push_back(23);
                        break;
                    case ')':
                        tmp.push_back(24);
                        break;
                    case '[':
                        hasEscape = true;
                        tmp.push_back('[');
                        break;
                    case ']':
                        hasEscape = true;
                        tmp.push_back(']');
                        break;
                    default:
                        throw std::logic_error("\\... Escape error!");
                        break;
                    }
                }
                else
                    tmp.push_back(now);
            } while (true);
            if (not tmp.empty())
            {
                std::sort(tmp.begin(), tmp.end());
                tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());
                res.push_back('(');
                for (auto&& i : tmp)
                {
                    res.push_back(i);
                    if (i != '\\')
                        res.push_back('|');
                }
                if (hasEscape)
                    res.append("\\)"), hasEscape = false;
                else
                    res.back() = ')';
                tmp.clear();
            }
            break;
        case '\\':
            now = next(i);
            switch (now)
            {
            case '\\':
                res.push_back(now);
                break;
            case 'w':
                res.append("(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9|_)");
                break;
            case 'd':
                res.append("(0|1|2|3|4|5|6|7|8|9)");
                break;
            case 'n':
                res.push_back(10);
                break;
            case 't':
                res.push_back(9);
                break;
            case '*':
                res.push_back(18);
                break;
            case '.':
                res.push_back(19);
                break;
            case '+':
                res.push_back(20);
                break;
            case '?':
                res.push_back(21);
                break;
            case '|':
                res.push_back(22);
                break;
            case '(':
                res.push_back(23);
                break;
            case ')':
                res.push_back(24);
                break;
            case '[':
            case ']':
                res.push_back('\\');
                res.push_back(now);
                break;
            default:
                throw std::logic_error("\\... Escape error!");
                break;
            }
            break;
        default:
            res.push_back(now);
            break;
        }
    }
    // cout << res << endl;
    return res;
}

} // namespace RE

#endif