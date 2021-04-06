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
using namespace std::chrono;
int main(void)
{
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    RE::Regex re("\\n([a-z-0-9_\\.-]+)@([\\da-z-]+)\\.[a-z]*\\t", 64);
    assert(re.match("\ndelta-in-hub@github123123123.com\t"));
    // assert(re.match("\ndelta-in-hub@11223\t\n3github.com\t"));
    high_resolution_clock::time_point t2   = high_resolution_clock::now();
    duration<double, std::milli> time_span = t2 - t1;
    cout << time_span.count() << "ms" << endl;
    return 0;
}