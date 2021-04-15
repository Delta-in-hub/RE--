//example.cc
#include "./Regex/Regex.hpp"
// #include "Regex.h"
#include <cassert>
#include <iostream>

using namespace std;

int main(void)
{
    RE::Regex re;
    re.assign("[a-c]{5}");
    assert(re.match("bcabb"));
    re.assign("[\\w-]*");
    assert(re.match("delta-in-hub"));

    RE::Regex re2("de\\d{2}ta");
    assert(re2.match("de42ta"));

    RE::Regex re3("[\\d]{3}");
    string tar = ("abc123123aaa21aa321aa");
    auto res   = re3.search(tar);
    cout << res.size() << endl;
    for (auto&& i : res)
    {
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
    return 0;
}