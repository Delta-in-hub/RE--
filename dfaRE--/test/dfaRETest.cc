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
    time_t _start = clock(), _end;
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
    _end = clock();
    cout << cnt2 << '/' << cnt1 << endl;
    debug(_end - _start);
    return 0;
}