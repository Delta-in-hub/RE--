#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#define debug(x) std::cout << #x << " is\t" << (x) << '\n';

using namespace std;

constexpr char catenate = '.';

string rexToPostRex(const string& rex)
{
    string res;
    auto pb      = [&res](char ch) { res.push_back(ch); };
    auto invaild = [&rex]() { throw std::invalid_argument("Invalid Regex:" + rex); };
    stack<pair<int, int>> paren;
    int numAtom = 0 /* 当前正则块的个数 [0-2]*/, numAlt = 0; /* | 的个数*/
    for (auto&& i : rex)
    {
        switch (i)
        {
        case '(':
            if (numAtom == 2)
            {
                numAtom--;
                pb(catenate);
            }
            paren.push({numAtom, numAlt});
            numAtom = numAlt = 0;
            break;
        case ')':
            if (numAtom == 0 or paren.empty())
                invaild();
            if (numAtom == 2)
                pb(catenate);
            numAtom = 0;
            while (numAlt-- > 0)
                pb('|');
            tie(numAtom, numAlt) = paren.top();
            paren.pop();
            numAtom++;
            break;
        case '|':
            if (numAtom == 0)
                invaild();
            if (numAtom == 2)
                pb(catenate);
            numAtom = 0;
            numAlt++;
            break;
        case '*':
        case '+':
        case '?':
            if (numAtom == 0)
                invaild();
            pb(i);
            break;
        default:
            if (numAtom == 2)
            {
                numAtom--;
                pb(catenate);
            }
            pb(i);
            numAtom++;
            break;
        }
    }
    if (numAtom == 0 or not paren.empty())
        invaild();
    if (numAtom == 2)
        pb(catenate);
    while (numAlt-- > 0)
        pb('|');
    return res;
}

//From https://swtch.com/~rsc/regexp/nfa.c.txt
char* re2post(const char* re)
{
    int nalt, natom;
    static char buf[8000];
    char* dst;
    struct
    {
        int nalt;
        int natom;
    } paren[100], *p;

    p     = paren;
    dst   = buf;
    nalt  = 0;
    natom = 0;
    if (strlen(re) >= sizeof buf / 2)
        return NULL;
    for (; *re; re++)
    {
        switch (*re)
        {
        case '(':
            if (natom > 1)
            {
                --natom;
                *dst++ = '.';
            }
            if (p >= paren + 100)
                return NULL;
            p->nalt  = nalt;
            p->natom = natom;
            p++;
            nalt  = 0;
            natom = 0;
            break;
        case '|':
            if (natom == 0)
                return NULL;
            while (--natom > 0)
                *dst++ = '.';
            nalt++;
            break;
        case ')':
            if (p == paren)
                return NULL;
            if (natom == 0)
                return NULL;
            while (--natom > 0)
                *dst++ = '.';
            for (; nalt > 0; nalt--)
                *dst++ = '|';
            --p;
            nalt  = p->nalt;
            natom = p->natom;
            natom++;
            break;
        case '*':
        case '+':
        case '?':
            if (natom == 0)
                return NULL;
            *dst++ = *re;
            break;
        default:
            if (natom > 1)
            {
                --natom;
                *dst++ = '.';
            }
            *dst++ = *re;
            natom++;
            break;
        }
    }
    if (p != paren)
        return NULL;
    while (--natom > 0)
        *dst++ = '.';
    for (; nalt > 0; nalt--)
        *dst++ = '|';
    *dst = 0;
    return buf;
}

signed main(void)
{
    ifstream f("rextest.txt");
    string tmp;
    while (f >> tmp)
    {
        char* res   = re2post(tmp.data());
        string res2 = rexToPostRex(tmp);
        if (res)
        {
            if (string(res) != res2)
            {
                debug(tmp);
                debug(rexToPostRex(tmp));
                debug(re2post(tmp.data()));
            }
        }
        else if (not res2.empty())
        {
            debug(tmp);
            debug(rexToPostRex(tmp));
            debug(re2post(tmp.data()));
        }
    }
    return 0;
}