#include "../nfaRe--.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n';

signed main(void)
{
    nfaRE::RE re("a?b+c*((d+|e?))fh");
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
    for (auto&& i : test)
    {
        if (not re.match(i))
            debug(i);
    }
    return 0;
}