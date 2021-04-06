#include "../nfaRE--.hpp"
#include <cassert>
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

std::string parse(const std::string& source);

signed main(void)
{
    if (false)
    {
        vector<string> test{
            "bbbbccccdddddfh",
            "abbbefh",
            "abcccccccddddddddfh",
            "abcccccccdddddddddfh",
            "bcdfh",
            "bbbbbbccccccccdddddfh",
            "bccfh",
            "acccddfh", // not match cases as follows
            "bbbbedf",
            "abbbeh",
            "sadcasd",
            "acccdddfh",
            "bcddhf",
            "abcddddd",
        };
        time_t _start = clock(), _end;
        RE::nfaRE re("a?b+c*((d+|e?))fh");
        int cnt1 = 0, cnt2 = 0;
        for (auto&& i : test)
        {
            cnt1++;
            if (not re.match(i))
            {
                // debug(i);
                cnt2++;
            }
        }
        _end = clock();
        cout << cnt2 << '/' << cnt1 << endl;
        debug(_end - _start);

        RE::nfaRE re2("ab.?d");
        assert(re2.match("abcd"));
        assert(re2.match("abd"));
        assert(re2.match("abf"));
        assert(re2.match("abccd"));
    }
    string s("\\.\\*\\|[a-c]");
    RE::nfaRE re4(parse(s));
    assert(re4.match(".*|b"));
    return 0;
}

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
        int cnt = 0;
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
                        // tmp.push_back(now);
                        cnt++;
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
                        cnt++;
                        tmp.push_back('[');
                        break;
                    case ']':
                        cnt++;
                        tmp.push_back(']');
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
                if (cnt)
                    res.append("\\)"), cnt = 0;
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