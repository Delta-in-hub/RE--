#include "./Regex/Regex.hpp"
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
void searchTest(const std::string& rex, const std::string& source, bool flag = true)
{
    RE::Regex re2(rex);
    string tar = (source);
    auto res   = re2.search(tar, flag);
    cout << res.size() << ' ' << rex << '\t';
    if (flag)
        cout << "Gready Search!" << endl;
    else
        cout << "Non Gready Search!" << endl;
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
    searchTest("a+", "abcaaaa");
    searchTest("a+", "abcaaa", false);
    return 0;
}