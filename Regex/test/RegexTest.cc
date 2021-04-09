#include "../Regex.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <queue>
#include <regex>
#include <set>
#include <stack>
#include <vector>
using namespace std;

const int Round = 1;

void searchTest(const std::string& rex, const std::string& source)
{
    RE::Regex re2(rex);
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

int main(void)
{
    searchTest("ab|cd", "abcdhfcd");
    searchTest("aa*", "abcaaaaaaa");
    searchTest("aaa", "abcaaaaaaa");
    searchTest(".*", "abcaaaaaaa");
    searchTest("[\\d]{2}", "abc123123aaa21aa1aa");
    return 0;
}