#include "../Regex.hpp"
#include <algorithm>
#include <cassert>
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
#include <set>
#include <stack>
#include <vector>
using namespace std;
/*
    * Done
    *
    * [xyz] (x|y|z)
    * [0-3] (0|1|2|3)
    * [0-2a-c] (0|1|2|a|b|c)
    *
    * \w [A-Za-z0-9_]
    * \d  [0-9]
    * \n char(10)
    *
    * TODO
    *
    * a{3} (aaa)
    * a{3,5} (aaa|aaaa|aaaaa)
    */
int main(void)
{
    clock_t s1 = clock(), s2, s3;
    RE::Regex re("([a-z-0-9_\\.-]+)@([\\da-z\\.-]+)\\.[a-z]*", 64);
    s2 = clock();
    assert(re.match("delta-in-hub@github.com"));
    s3 = clock();
    cout << s3 - s1 << endl;
    cout << s2 - s1 << endl;

    return 0;
}