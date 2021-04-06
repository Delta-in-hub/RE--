#include "../Regex.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <vector>
using namespace std;
/*
    * [xyz] (x|y|z)
    * [0-3] (0|1|2|3)
    * [0-2a-c] (0|1|2|a|b|c)
    *
    * \w [A-Za-z0-9_]
    * \d  [0-9]
    * \n char(10)
    *
    *
    * a{3} (aaa)
    * a{3,5} (aaa|aaaa|aaaaa)
    */
std::string parse(const std::string& source)
{
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
                        throw std::invalid_argument(source + " at [" + std::to_string(i) + "]");
                    else
                    {
                        from = tmp.back();
                        now  = next(i);
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
                        tmp.push_back(now);
                        break;
                    case 'w':
                        tmp.append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
                        break;
                    case 'd':
                        tmp.append("0123456789");
                        break;
                    case 'n':
                    case 't':
                    case '*':
                    case '.':
                    case '?':
                    case '|':
                    case '(':
                    case '[':
                        tmp.append("\\");
                        tmp.push_back(now);
                        break;
                    default:
                        throw logic_error("\\... Escape error!");
                        break;
                    }
                }
                else
                    tmp.push_back(now);
            } while (true);
            if (not tmp.empty())
            {
                res.push_back('(');
                for (auto&& i : tmp)
                {
                    res.push_back(i);
                    if (i != '\\')
                        res.push_back('|');
                }
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
            case 't':
            case '*':
            case '.':
            case '?':
            case '|':
            case '(':
            case '[':
                res.append("\\");
                res.push_back(now);
                break;
            default:
                throw logic_error("\\... Escape error!");
                break;
            }
            break;
        default:
            res.push_back(now);
            break;
        }
    }
    return res;
}
int main(void)
{
    cout << parse("\\.[a-z\\.]") << endl;
    return 0;
}