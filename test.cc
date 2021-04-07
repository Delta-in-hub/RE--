#include "Regex/Regex.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace std;

void rexTest(const string& s)
{
    cout << s << endl;
    ifstream fs(s);
    if (not fs.good())
        throw invalid_argument("not fs.good()");
    string a, b, c, d;
    char tmp[1000];
    int cnt = 0;
    while (fs >> a >> b >> c >> d)
    {
        if (c == "NULL")
            continue;
        int l = 0, r = 0;
        sscanf(d.c_str(), "(%d,%d)%s", &l, &r, tmp);
        if (b.front() == '^')
            b.erase(0, 1);
        if (b.back() == '$')
            b.pop_back();
        RE::Regex* re;
        try
        {
            re = new RE::Regex(b);
        }
        catch (const std::exception& e)
        {
            cout << a << endl;
        }
        if ((r - l == int(c.length()) and not re->match(c)) or (r - l != int(c.length()) and re->match(c)))
        {
            cout << a;
            cout << " not match!" << endl;
            cout << "----" << endl;
            cout << b << '\t' << c << '\t' << d << endl;
            cout << "----" << endl;
        }
        // cout << endl;
        cnt++;
        delete re;
    }
    fs.close();
    cout << s <<' '<<cnt<< " cases Done and No error!" << endl;
}

signed main(void)
{
    for (int i = 0; i < 6; i++)
    {
        rexTest("./rexTestCase/rexTestCase" + to_string(i) + ".txt");
    }
    return 0;
}