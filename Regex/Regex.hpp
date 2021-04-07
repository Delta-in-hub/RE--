#ifndef __RE__
#define __RE__
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
    std::string parse2(const std::string& src);

  public:
    Regex(const size_t maxdstate = 256)
    {
        RE::dfaRE::MAXDSTATELIMIT = maxdstate;
    }
    Regex(const std::string& str, const size_t maxdstate = 256)
        : RE::dfaRE(parse2(parse(str)), maxdstate)
    {
        ;
    }
    void assign(const std::string& str)
    {
        RE::dfaRE::assign(parse2(parse(str)));
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
            case '{':
            case '}':
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
// process {1,3} {1,} {3}
std::string Regex::parse2(const std::string& src)
{
    using namespace std;
    auto error = [&src](int i) { throw std::invalid_argument(src + " at [" + std::to_string(i) + "]"); };
    auto next  = [&src](size_t& i) -> char {
        if (i + 1 < src.size())
            return src[++i];
        else
            throw std::invalid_argument(src + " at [" + std::to_string(i) + "]");
    };
    string res, tmp;
    int n1 = 0, n2 = 0;
    char ch;
    int l = 0, r = 0;
    int bracNum = 0;
    for (size_t i = 0; i < src.length(); i++)
    {
        if (src[i] == '{')
        {
            n1 = n2 = 0;
            r       = i;
            ch      = next(i);
            while (isdigit(ch))
            {
                n1 = n1 * 10 + ch - '0';
                ch = next(i);
            }
            if (ch == ',')
            {
                ch = next(i);
                if (ch == '}')
                { //a{3,}
                    switch (src.at(r - 1))
                    {
                    case ')':
                        for (l = res.size() - 1; l >= 0; l--)
                        {
                            if (res[l] == ')')
                                bracNum++;
                            else if (res[l] == '(')
                                bracNum--;
                            if (bracNum == 0)
                                break;
                        }
                        tmp = res.substr(l, res.size() - l);
                        for (int j = 0; j < n1; j++)
                        {
                            res.append(tmp);
                        }
                        res.push_back('*');
                        break;
                    case '*':
                    case '|':
                    case '?':
                    case '{':
                        error(r - 1);
                        break;
                    default:
                        for (int j = 0; j < n1; j++)
                        {
                            res.push_back(src.at(r - 1));
                        }
                        res.push_back('*');
                        break;
                    }
                    continue;
                }
                while (isdigit(ch))
                {
                    n2 = n2 * 10 + ch - '0';
                    ch = next(i);
                }
                if (ch == '}')
                {
                    if (n1 >= n2)
                        error(i);
                    switch (src.at(r - 1))
                    {
                    case ')':
                        for (l = res.size() - 1; l >= 0; l--)
                        {
                            if (res[l] == ')')
                                bracNum++;
                            else if (res[l] == '(')
                                bracNum--;
                            if (bracNum == 0)
                                break;
                        }
                        if (res.at(l) != '(')
                            error(l);
                        tmp = res.substr(l, res.size() - l);
                        res.erase(l, res.size() - l);
                        if (n1 == 0)
                        {
                            res.push_back('(');
                            res.append(tmp);
                            res.push_back('?');
                            std::string tmp2 = tmp;
                            for (int j = 1; j < n2; j++)
                            {
                                tmp2.append(tmp);
                                res.push_back('|');
                                res.append(tmp2);
                            }
                            res.push_back(')');
                        }
                        else
                        {
                            std::string tmp2;
                            for (int j = 0; j < n1; j++)
                            {
                                tmp2.append(tmp);
                            }
                            res.push_back('(');
                            res.append(tmp2);
                            while (n1++ < n2)
                            {
                                res.push_back('|');
                                tmp2.append(tmp);
                                res.append(tmp2);
                            }
                            res.push_back(')');
                        }
                        break;
                    case '*':
                    case '|':
                    case '?':
                    case '{':
                        error(r - 1);
                        break;
                    default:
                        res.pop_back();
                        if (n1 == 0)
                        {
                            res.push_back('(');
                            res.push_back(src.at(r - 1));
                            res.push_back('?');
                            std::string tmp2;
                            tmp2.push_back(src.at(r - 1));
                            for (int j = 1; j < n2; j++)
                            {
                                res.push_back('|');
                                tmp2.push_back(src.at(r - 1));
                                res.append(tmp2);
                            }
                            res.push_back(')');
                        }
                        else
                        {
                            std::string tmp2;
                            for (int j = 0; j < n1; j++)
                            {
                                tmp2.push_back(src.at(r - 1));
                            }
                            res.push_back('(');
                            res.append(tmp2);
                            while (n1++ < n2)
                            {
                                res.push_back('|');
                                tmp2.push_back(src.at(r - 1));
                                res.append(tmp2);
                            }
                            res.push_back(')');
                        }
                        break;
                    }
                }
                else
                    error(i);
            }
            else if (ch == '}')
            { //a{3}
                switch (src.at(r - 1))
                {
                case ')':
                    for (l = res.size() - 1; l >= 0; l--)
                    {
                        if (res[l] == ')')
                            bracNum++;
                        else if (res[l] == '(')
                            bracNum--;
                        if (bracNum == 0)
                            break;
                    }
                    tmp = res.substr(l, res.size() - l);
                    res.erase(l, res.size() - l);
                    for (int j = 0; j < n1; j++)
                    {
                        res.append(tmp);
                    }
                    break;
                case '*':
                case '|':
                case '?':
                case '{':
                    error(r - 1);
                    break;
                default:
                    res.pop_back();
                    for (int j = 0; j < n1; j++)
                    {
                        res.push_back(src.at(r - 1));
                    }
                    break;
                }
            }
            else
            {
                error(i);
            }
        }
        else if (src[i] == '\\')
        {
            if (i + 1 < src.length())
            {
                ch = next(i);
                switch (ch)
                {
                case '[':
                case ']':
                case '{':
                case '}':
                    res.push_back(ch);
                    break;
                default:
                    res.push_back('\\');
                    res.push_back(ch);
                    break;
                }
            }
            else
                res.push_back('\\');
        }
        else
        {
            res.push_back(src[i]);
        }
    }
    // cout << res << endl;
    return res;
}
} // namespace RE

#endif