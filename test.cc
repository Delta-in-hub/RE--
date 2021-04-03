#include <bits/stdc++.h>

using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n';
using ll  = long long;
using ull = unsigned long long;

struct A
{
    int c;
    A* b;
};

union Ptrlist {
    Ptrlist* next;
    A* s;
};

signed main(void)
{
    A a;
    A b{2, &a};

    return 0;
}