#include "../RE--.hpp"
#include <cassert>
#include <string>
#include <vector>

using namespace std;
int main()
{
    RE::RE rex("");
    bool flag = rex.match("abcde");
    assert(flag);
    return 0;
}