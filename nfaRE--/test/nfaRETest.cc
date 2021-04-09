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
    auto res   = re2.search(tar,false);
    cout << res.size() << endl;
    for (auto&& i : res)
    {
        // cout << i.first << " " << i.second << endl;
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
    searchTest("ab|cd", "abcdhfcd");
    searchTest("aa*", "abcaaaaaaa");
    searchTest("a+", "abcaaaaaaa");
    return 0;
}
