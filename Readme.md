# RE--
A Zero to One Regular expression engine which supports almost all feature except Capturing Groups, Negated Character Classes and something else.

**C++11 is required.** **Only support ascii character set.**

`Regex.h` and `Regex.cc` is all things to let RE-- run.

## RE– Support

| Metacharacter | Description                                                                 |
| :------------ | --------------------------------------------------------------------------- |
| .             | Match any character except newline'\n'                                      |
| ?             | Match a expression zero or one time                                         |
| +             | Match a expression one or more times                                        |
| *             | Match a expression zero or more times                                       |
| \|            | Alternation                                                                 |
| (expression)  | Just Non-Capturing Groups .                                                 |
| {M,N}         | Denotes the minimum M and the maximum N match count.e.g`x{0,} y{1,} z{0,1}` |
| []            | A set of possible character matches.e.g `[a-z0-9_]`                         |
| \w            | Same as [A-Za-z0-9\_].Match an alphanumeric character, including "\_"       |
| \d            | Same as [0-9].Match a digit                                                 |
| \n            | Match newline'\n'. (char)(10) in ascii                                      |
| \t            | Match tab'\t'. (char)(9) in ascii                                      |
| \\\\          | Just match `\`                                                              |
| \\?           | Just match `?`                                                              |
| \\+           | Just match `+`                                                              |
| \\*           | Just match `*`                                                              |
| `\(\)`        | Just match `()`                                                             |
| `\[\]`        | Just match `[]`                                                             |
| \\{\\}        | Just match `{}`                                                             |





## RE– Don’t Support

| Metacharacter | Description                                       |
| ------------- | ------------------------------------------------- |
| ()            | Capturing Groups                                  |
| [^…]          | Negated Character Classes                         |
| ^             | Matches the beginning of a line or string         |
| $             | Matches the end of a line or string               |
| ……            | Other Metacharacter do not be listed in `Support` |

Notice that : `()`A empty brackets is rejected in RE–. `{n,m}` if n equals to m,RE– will also reject it.



## How to use

`  Regex(const size_t maxDfaState = 256);`

Default constructor,you can also set maximun number limit of state in dfa which is 256 by default.
If the number of dfa's state in construction greater than your limit,it will use nfa to work rather than dfa.

`  Regex(const std::string& regexp, const size_t maxDfaState = 256);`

Construct state of your regexp in constructor.

`  void Regex::assign(const std::string& regexp);`

Assigns the contents to the regular expression.

You have to assign regexp to it if you use Default constructor and then you can use it to match or search.

`  bool Regex::match(const std::string& target);`

Determines if the regular expression  matches the entire target character sequence.

`  std::vector<std::pair<size_t, size_t>> Regex::search(const std::string& target,bool isGreadySearch = true);`

Determines if there is a match between the regular expression  and some subsequence in the target string. **It will return all substr positions [l,r] which matches regexp.**

By default,It will search greedily to match substr as long as possible.
Otherwise,It will try to match as few content as possible that meet the regexp.
See it:
![Snipaste_2021-04-09_21_55_25](/assets/Snipaste_2021-04-09_21_55_25.jpg)

## Run Example

```c++
//example.cc
#include "Regex.h"
#include <cassert>
#include <iostream>

using namespace std;

int main(void)
{
    RE::Regex re;
    re.assign("[a-c]{5}");
    assert(re.match("bcabb"));
    re.assign("[\\w-]*");
    assert(re.match("delta-in-hub"));

    RE::Regex re2("de\\d{2}ta");
    assert(re2.match("de42ta"));

    RE::Regex re3("[\\d]{3}");
    string tar = ("abc123123aaa21aa321aa");
    auto res   = re3.search(tar);
    cout << res.size() << endl;
    for (auto&& i : res)
    {
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
    return 0;
}
```
`g++ -O2 example.cc Regex.cc -o example -std=c++11 -Wall`

Output:

![Snipaste_2021-04-09_00_05_38](/assets/Snipaste_2021-04-09_00_05_38.jpg)

## Run Test Cases

Regexp Test cases are from [haskell.org/package/regex-posix-unittest-1.1](http://hackage.haskell.org/package/regex-posix-unittest-1.1/src/data-dir)

`g++ -O2 test.cc Regex.cc -o test -std=c++11 -Wall `

Output:

![Snipaste_2021-04-09_00_07_04](/assets/Snipaste_2021-04-09_00_07_04.jpg)

## Simply comparing performance to std::regex

[Code here](./performance.cc)

Intel 8250u
gcc version 10.2.0 (MinGW-W64 x86_64-posix-seh, built by Brecht Sanders)
g++ -O2 performance.cc Regex.cc -o performance -std=c++17 -Wall

```
std::regex use 1011.8us to construct (?:[hH][tT]{2}[pP]://|[hH][tT]{2}[pP][sS]://)(?:(?:[A-Za-z0-9~-]+).)+(?:[-A-Za-z0-9~\\/_%])+
RE::Regex use 1998.8us to construct ([hH][tT]{2}[pP]://|[hH][tT]{2}[pP][sS]://)(([A-Za-z0-9~-]+).)+([-A-Za-z0-9~\\/_%])+
std::regex use 0us to match http://www.fapiao.com/dddp-web/pdf/download?request=6e7JGxxxxx4ILd-kExxxxxxxqJ4-CHLmqVnenXC692m74H38sdfdsazxcUmfcOH2fAfY1Vw__%5EDadIfJgiEf
RE::Regex use 0us to match http://www.fapiao.com/dddp-web/pdf/download?request=6e7JGxxxxx4ILd-kExxxxxxxqJ4-CHLmqVnenXC692m74H38sdfdsazxcUmfcOH2fAfY1Vw__%5EDadIfJgiEf

std::regex use 1003.7us to construct [a-zA-Z0-9._]+@(?:[a-zA-Z0-9]+.)+com
RE::Regex use 3997.6us to construct [a-zA-Z0-9._]+@([a-zA-Z0-9]+.)+com
std::regex use 1.701e+06us to match power.overwhelming@aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
RE::Regex use 0us to match power.overwhelming@aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

std::regex use 0us to construct (?:[01]+)+b
RE::Regex use 998.6us to construct ([01]+)+b
std::regex use 0us to match 10101010110101001100101010101010101010101010101010000b
RE::Regex use 0us to match 10101010110101001100101010101010101010101010101010000b
std::regex use 318996us to match 10101010110101001100101
RE::Regex use 0us to match 10101010110101001100101

std::regex use 0us to construct [a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(?:\.[a-zA-Z0-9_-]+)+
RE::Regex use 1000.1us to construct [a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\.[a-zA-Z0-9_-]+)+
std::regex use 0us to match delta-in-hub@github.com
RE::Regex use 0us to match delta-in-hub@github.com
```

## How could I also make one
```c++
class nfaRE{...};
class dfaRE:protected nfaRE{...};
class Regex:protected dfaRE{...};
```
1. First read this article  https://swtch.com/~rsc/regexp/regexp1.html
2. Write your own nfaRE,only supporting (concatenation, *, +, ?, | ) operator
3. Write your own dfaRE,only supporting (concatenation, *, +, ?, | ) operator
4. Do some work to let your engine suppport **.** *(match any character)*
5. Do some test and fix some bugs
6. Write your own Regex, parsing complex regexp to simple regexp which only contains (concatenation, *, +, ?, |, **.** ) operator
7. Do some test and fix some bugs
8. Done and give me a star.

Read Readme.md in each subfolder to get more details.

## Tree of repo

```
│  .gitignore
│  Readme.md
│  Regex.cc     (RE--)
│  Regex.h      (RE-- header file)
│  test.cc
|  example.cc
|  performance.cc
├─assets
│      ...
├─dfaRE--
│  │  dfaRE--.hpp  (dfaRE's declaration and definition in one .hpp file)
│  │  Readme.md
│  └─test
│          dfaRETest.cc
├─nfaRE--
│  │  nfaRE--.hpp   (nfaRE's declaration and definition in one .hpp file)
│  │  Readme.md
│  └─test
│          nfaRETest.cc
│          rexTest.txt
│          rexToPostRexTest.cc
├─Regex
│  │  Regex.hpp     (Regex's declaration and definition in one .hpp file)
│  │  Readme.md
│  └─test
│          RegexTest.cc
└─rexTestCase
        ... some regexp test cases
```


## TODO
- [x] 完善文档
- [x] 非?贪婪匹配
- [ ] support Negated Character Classes. e.g [\^abc] to match any character except (a and b and c)
