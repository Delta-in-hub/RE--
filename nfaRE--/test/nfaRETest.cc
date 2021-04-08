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

void searchTest(const std::string& rex, const std::string& source)
{
    RE::nfaRE re2(rex);
    string tar = (source);
    auto res   = re2.search(tar);
    cout << res.size() << endl;
    for (auto&& i : res)
    {
        cout << i.first << " " << i.second << endl;
        cout << tar << endl;
        for (size_t j = 0; j < tar.length(); j++)
        {
            if (j < i.first)
                cout << ' ';
            else if (j == i.first)
                cout << '^';
            else if (j < i.second)
                cout << '-';
            else if (j == i.second)
                cout << '^';
        }
        cout << endl;
    }
}

signed main(void)
{
    if (true)
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
                cnt2++;
            }
        }
        _end = clock();
        cout << cnt2 << '/' << cnt1 << endl;
        debug(_end - _start);
    }
    searchTest("ab|cd", "abcdabcd");
    return 0;
}
