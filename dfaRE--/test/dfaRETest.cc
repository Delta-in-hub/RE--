#include "../dfaRE--.hpp"
#include <cassert>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

signed main(void)
{
    vector<string> test{
        "bbbbccccdddddfh",
        "abbbefh",
        "abcccccccddddddddfh",
        "abcccccccdddddddddfh",
        "bcdfh",
        "bbbbbbccccccccdddddfh",
        "bccfh",
        "acccddfh", // not match cases as follows
        "bbbbedf",
        "abbbeh",
        "sadcasd",
        "acccdddfh",
        "bcddhf",
        "abcddddd",
    };
    RE::dfaRE re("a?b+c*((d+|e?))fh");
    int cnt1 = 0, cnt2 = 0;
    for (auto&& i : test)
    {
        cnt1++;
        if (not re.match(i))
        {
            // debug(i);
            cnt2++;
        }
    }
    cout << cnt2 << '/' << cnt1 << endl;

    RE::dfaRE re2("ab.?e|.cf");

    vector<string> test2 = {
        "abce",
        "abe",
        "be",
        "bce",
        "dcf",
        "cf",
        "df",
    };
    for (auto&& i : test2)
    {
        if (not re2.match(i))
            debug(i);
    }

    RE::dfaRE re3(".*d");
    vector<string> test3 = {
        "asdl;kfjdsa;klf",
        "l;kj;lkj;lkjd",
        "d",
        ";lkklj;j;lkd",
        "ajklkjd;jlkd",
        "dddddda",
        "ddsddfddd",
        "dlkjadda",

    };
    for (auto&& i : test3)
    {
        if (not re3.match(i))
            debug(i);
    }
    cout << "-----------" << endl;
    assert(RE::dfaRE("a|a|a").match("aa"));

    return 0;
}