// #include "Regex/Regex.hpp"
#include "Regex.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace std;

RE::Regex re;
void rexTest(const string& s)
{
    cout << s << endl;
    ifstream fs(s);
    if (not fs.good())
        throw invalid_argument("not fs.good()");
    string a, b, c, d;
    char tmp[1000];
    int cnt = 0, linenum = 0;
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
        try
        {
            re.assign(b);
        }
        catch (const std::exception& e)
        {
            cout << a << endl;
        }
        linenum++;
        if ((r - l == int(c.length()) and not re.match(c)) or (r - l != int(c.length()) and re.match(c)))
        {
            cout << a;
            cout << " not match!" << endl;
            cout << "----" << endl;
            cout << b << '\t' << c << '\t' << d << endl;
            cout << "----" << endl;
        }
        else
            cnt++;
    }
    fs.close();
    cout << s << ' ' << cnt << '/' << linenum << " cases Done and No error!" << endl;
}

void searchTest(const std::string& rex, const std::string& source)
{
    RE::Regex re2(rex);
    string tar = (source);
    auto res   = re2.search(tar);
    cout << res.size() << ' ' << rex << endl;
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

signed main(void)
{
    for (int i = 0; i < 6; i++)
    {
        rexTest("./rexTestCase/rexTestCase" + to_string(i) + ".txt");
    }
    cout << "Over!" << endl;
    searchTest("ab|cd", "abcdhfcd");
    searchTest("aa*", "abcaaaaaaa");
    searchTest("aaa", "abcaaaaaaa");
    searchTest(".*", "abcaaaaaaa");
    searchTest(".*@", "delta-in-hub@github.com");
    searchTest("[\\d]{2}", "abc123123aaa21aa1aa");

    searchTest("([hH][tT]{2}[pP]://|[hH][tT]{2}[pP][sS]://)(([A-Za-z0-9~-]+).)+([-A-Za-z0-9~\\\\/])+", "http://www.fapiao.com/dddp-web/pdf/download?request=6e7JGxxxxx4ILd-kExxxxxxxqJ4-CHLmqVnenXC692m74H38sdfdsazxcUmfcOH2fAfY1Vw__%5EDadIfJgiEf"); //https://zhuanlan.zhihu.com/p/163279494
    searchTest("[a-zA-Z0-9._]+@([a-zA-Z0-9]+.)+com", "power.overwhelming@aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");                                                                                                                           //https://zhuanlan.zhihu.com/p/46294360
    searchTest("(0|[0-1]){2,15}(hello){0,2}([0-9]+)+", "00hellohello0000000000000000000000000000");
    searchTest("([01]+)+b", "10101010110101001100101010101010101010101010101010000"); //https://www.zhihu.com/question/21145025/answer/189268974
    searchTest("\\d+\\d", "11");                                                      //https://www.zhihu.com/question/27434493/answer/36701865 评论
    return 0;
}