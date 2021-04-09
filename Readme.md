# RE--
A Zero to One Regular expression engine which supports almost all feature except Capturing Groups, Negated Character Classes and something else.

**C++11 is required.** **Only support ascii characters set.**

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

Notice that : `()`A empty bracket is rejected in RE–. `{n,m}` if n equals to m,RE– will also reject it.

## Tree of repo

```
│  .gitignore
│  Readme.md
│  Regex.cc
│  Regex.h
│  test.cc
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

## How to use

`  Regex(const size_t maxDfaState = 256);`

Default constructor,you can also set maximun number limits of state in dfa which is 256 by default.

`  Regex(const std::string& regexp, const size_t maxDfaState = 256);`

Construct state of your regexp in constructor.

`  void Regex::assign(const std::string& regexp);`

Assigns the contents to the regular expression.

You have to assign regexp to it if you use Default constructor and then you can use it to match or search.

`  bool Regex::match(const std::string& target);`

Determines if the regular expression  matches the entire target character sequence.

`  std::vector<std::pair<size_t, size_t>> Regex::search(const std::string& target);`

Determines if there is a match between the regular expression  and some subsequence in the target string.

It will return all substr positions [l,r] which matches regexp.

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
`g++ -O2 example.cc Regex.cc -o example -std=c++11`

Output
![Snipaste_2021-04-09_00_05_38](/assets/Snipaste_2021-04-09_00_05_38.jpg)

## Test cases

>   http://hackage.haskell.org/package/regex-posix-unittest-1.1/src/data-dir

`g++ -O2 test.cc Regex.cc -o test -std=c++11 -Wall `

Output
![Snipaste_2021-04-09_00_07_04](/assets/Snipaste_2021-04-09_00_07_04.jpg)



## TODO
- [x] 完善文档
- [ ] 非贪婪匹配
- [ ] support Negated Character Classes. e.g [\^abc] to match any character except (a and b and c)
