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

int main(void)
{
    clock_t t1, t2;
    double ans1 = 0;
    t1          = clock();
    RE::Regex re("((..)|(.))((..)|(.))"); //bug
    // RE::Regex re("([a][dhz]?)+"); //Ok
    t2 = clock();
    cout << t2 - t1 << endl;
    for (int i = 0; i < Round; i++)
    {
        t1 = clock();
        assert(re.match("aab"));
        t2 = clock();
        ans1 += t2 - t1;
    }
    cout << ans1 << endl;
    re.assign("[a-c]{3}\\(\\)\\{\\}");
    assert(re.match("abb(){}"));
    cout << 111 << endl;
    RE::Regex re2;
    re2.assign("[a-e]*");
    assert(re2.match("abcde"));
    cout << 222 << endl;
    return 0;
}