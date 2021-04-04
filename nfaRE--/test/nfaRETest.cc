#include "../nfaRE--.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

signed main(void)
{
    RE::nfaRE re("a?b+c*((d+|e?))fh");
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
    int cnt1 = 0, cnt2 = 0;
    for (auto&& i : test)
    {
        cnt1++;
        if (not re.match(i))
            debug(i), cnt2++;
    }
    cout << cnt2 << '/' << cnt1 << endl;
    return 0;
}