#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>
#define debug(x) std::cout << #x << " is\t" << (x) << '\n';

using namespace std;
string rexToPostRex(const string& rex)
{
    string result;
    if (rex.empty())
        return result;
    const char connectChar = '.';
    stack<char> opStack;
    auto invalidRex = []() { throw std::logic_error("Invaild RegExp"); };
    auto popBack    = [&opStack, &result](char op) {
        while (not opStack.empty() and opStack.top() == op)
        {
            result.push_back(opStack.top());
            opStack.pop();
        }
    };
    bool isFirstChar    = true;
    bool isFirstbracket = true;
    for (auto&& i : rex)
    {
        switch (i)
        {
        case '?':
        case '*':
        case '+':
            result.push_back(i);
            popBack(connectChar);
            break;
        case '(':
            if (not result.empty())
                isFirstbracket = false;
            popBack(connectChar);
            if (isFirstbracket)
                isFirstbracket = false;
            else
                opStack.push(connectChar);
            opStack.push(i);
            isFirstChar = true;
            break;
        case '|':
            popBack(connectChar);
            opStack.push(i);
            isFirstChar = true;
            break;
        case ')':
            while (not opStack.empty() and opStack.top() != '(')
            {
                result.push_back(opStack.top());
                opStack.pop();
            }
            opStack.pop();
            break;
        default:
            popBack(connectChar);
            if (isFirstChar)
                isFirstChar = false;
            else
                opStack.push(connectChar);
            result.push_back(i);
            break;
        }
    }
    while (not opStack.empty())
    {
        result.push_back(opStack.top());
        opStack.pop();
    }
    return result;
}

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
    ifstream f("rex.txt");
    string tmp;
    while (f >> tmp)
    {
        if (rexToPostRex(tmp) != re2post(tmp.data()))
        {
            debug(tmp);
            debug(rexToPostRex(tmp));
            debug(re2post(tmp.data()));
            break;
        }
    }
    return 0;
}