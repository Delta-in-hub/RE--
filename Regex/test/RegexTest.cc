#include "../Regex.hpp"
#include <algorithm>
#include <cassert>
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
/*
    * [xyz] (x|y|z)
    * [0-3] (0|1|2|3)
    * [0-2a-c] (0|1|2|a|b|c)
    *
    * \w [A-Za-z0-9_]
    * \d  [0-9]
    * \n char(10)
    *
    *
    * a{3} (aaa)
    * a{3,5} (aaa|aaaa|aaaaa)
    */
int main(void)
{
    RE::Regex re("\\*\\|[a-c]d\\d", 64);
    assert(re.match("*|bd9"));
    return 0;
}