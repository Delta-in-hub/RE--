#include "Regex/Regex.hpp"
#include <bits/stdc++.h>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'
using ll  = long long;
using ull = unsigned long long;

signed main(void)
{
    ifstream fs("rexTestCase.txt");
    string a, b, c, d;
    char tmp[1000];
    while (fs >> a >> b >> c >> d)
    {
        int l, r;
        sscanf(d.c_str(), "(%d,%d)%s", &l, &r, tmp);

        if (b.front() == '^')
            b.erase(0, 1);
        if (b.back() == '$')
            b.pop_back();
        auto re = new RE::Regex(b);
        cout << a << endl;
        if ((r - l == c.length() and not re->match(c)) or (r - l != c.length() and re->match(c)))
        {
            cout << "not match!" << endl;
        }
        delete re;
    }

    return 0;
}