#include "../dfaRE--.hpp"
#include <cassert>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

void searchTest(const std::string& rex, const std::string& source)
{
    RE::dfaRE re2(rex);
    string tar = (source);
    auto res   = re2.search(tar);
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
    // searchTest("ab|cd", "abcdhfcd");
    // searchTest("aa*", "abcaaaaaaa");
    // searchTest("aaa", "abcaaaaaaa");
    RE::dfaRE re("[a-z]+@com");
    cout << re.match("asd@com");
    return 0;
}