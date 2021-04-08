#include "Regex.h"
#include <algorithm>
#include <cstring>
#include <map>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
__BASERE__::nfaRE::State* __BASERE__::nfaRE::state(int c, State* out, State* out1)
{
    State* t = new State;
    t->c     = c;
    t->out   = out;
    t->out1  = out1;
    return t;
}
__BASERE__::nfaRE::State* __BASERE__::nfaRE::postToNfa(const std::string& rex)
{
    using namespace std;
    stack<Frag> st;
    Frag e1, e2;
    State *s, *s1;
    auto cat = [](State* e1, State* s) {
        e1->c == Split ? e1->out1 = s : e1->out = s;
    };
    for (auto&& i : rex)
    {
        switch (i)
        {
        case Catenate:
            e2 = st.top();
            st.pop();
            e1 = st.top();
            st.pop();
            cat(e1.tail, e2.head);
            st.push({e1.head, e2.tail});
            break;
        case '|':
            e2 = st.top();
            st.pop();
            e1 = st.top();
            st.pop();
            s  = state(Split, e1.head, e2.head);
            s1 = state(Merge, nullptr, nullptr);
            cat(e1.tail, s1);
            cat(e2.tail, s1);
            st.push({s, s1});
            break;
        case '?':
            e1 = st.top();
            st.pop();
            s1 = state(Merge, nullptr, nullptr);
            s  = state(Split, e1.head, s1);
            cat(e1.tail, s1);
            st.push({s, s1});
            break;
        case '+':
            e1 = st.top();
            st.pop();
            s = state(Split, e1.head, nullptr);
            cat(e1.tail, s);
            st.push({e1.head, s});
            break;
        case '*':
            e1 = st.top();
            st.pop();
            s = state(Split, e1.head, nullptr);
            cat(e1.tail, s);
            st.push({s, s});
            break;
        case '.':
            s = state(Any, nullptr, nullptr);
            st.push({s, s});
            break;
        case 18: // *
        case 19: //.
        case 20: //+
        case 21:
        case 22:
        case 23:
        case 24:
            s = state(ESCAPE.at(i), nullptr, nullptr);
            st.push({s, s});
            break;
        default:
            s = state(i, nullptr, nullptr);
            st.push({s, s});
            break;
        }
    }
    e1 = st.top();
    st.pop();
    if (not st.empty())
        throw logic_error(rex);
    cat(e1.tail, &Accept);
    return e1.head;
}

std::string __BASERE__::nfaRE::rexToPostRex(const std::string& rex)
{
    using namespace std;
    string res;
    auto pb      = [&res](char ch) { res.push_back(ch); };
    auto invaild = [&rex]() { throw std::invalid_argument("Invalid Regex:" + rex); };
    stack<pair<int, int>> paren;
    int numAtom = 0 /* 当前正则块的个数 [0-2]*/, numAlt = 0; /* | 的个数*/
    for (size_t j = 0; j < rex.length(); j++)
    {
        char i = rex[j];
        switch (i)
        {
        case '(':
            if (numAtom == 2)
            {
                numAtom--;
                pb(Catenate);
            }
            paren.push({numAtom, numAlt});
            numAtom = numAlt = 0;
            break;
        case ')':
            if (numAtom == 0 or paren.empty())
                invaild();
            if (numAtom == 2)
                pb(Catenate);
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
                pb(Catenate);
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
                pb(Catenate);
            }
            pb(i);
            numAtom++;
            break;
        }
    }
    if (numAtom == 0 or not paren.empty())
        invaild();
    if (numAtom == 2)
        pb(Catenate);
    while (numAlt-- > 0)
        pb('|');
    // cout << res << endl;

    return res;
}

void __BASERE__::nfaRE::_delete(State* now)
{
    if (now == nullptr or now == &Accept or next.find(now) != next.end())
        return;
    next.insert(now);
    _delete(now->out);
    _delete(now->out1);
}
void __BASERE__::nfaRE::addState(State* s, std::unordered_set<State*>& stateSet)
{
    if (s == nullptr or ((s->c == Split or s->c == Merge) and s->searched))
        return;
    if (s->c == Split or s->c == Merge)
        s->searched = true;
    if (s->c == Split or s->c == Merge)
    {
        addState(s->out, stateSet);
        addState(s->out1, stateSet);
    }
    else
        stateSet.insert(s);
    s->searched = false;
}
__BASERE__::nfaRE::nfaRE()
{
    Start      = nullptr;
    Accept.c   = Match;
    Accept.out = Accept.out1 = nullptr;
    Accept.searched          = false;
}

__BASERE__::nfaRE::nfaRE(const std::string& rex)
{
    Accept.c   = Match;
    Accept.out = Accept.out1 = nullptr;
    Accept.searched          = false;
    Start      = postToNfa(rexToPostRex(rex));
}
__BASERE__::nfaRE::~nfaRE()
{
    next.clear();
    _delete(Start);
    for (auto&& i : next)
    {
        delete i;
    }
    next.clear();
}

void __BASERE__::nfaRE::assign(const std::string& rex)
{
    next.clear();
    _delete(Start);
    for (auto&& i : next)
    {
        delete i;
    }
    next.clear();
    Start = postToNfa(rexToPostRex(rex));
}

bool __BASERE__::nfaRE::match(const std::string& target)
{
    now.clear(), next.clear();
    if (not Start)
        throw std::logic_error("assign() before match()");
    addState(Start, now);
    for (auto&& i : target)
    {
        for (auto&& j : now)
        {
            if (j->c == i or (j->c == Any and i != '\n'))
            {
                addState(j->out, next);
            }
        }
        std::swap(now, next);
        next.clear();
    }
    return now.find(&Accept) != now.end();
}

std::vector<std::pair<size_t, size_t>> __BASERE__::nfaRE::search(const std::string& target)
{
    using namespace std;
    vector<pair<size_t, size_t>> respos;
    using namespace std;
    now.clear(), next.clear();
    if (not Start)
        throw std::logic_error("assign() before match()");

    addState(Start, now);
    unordered_set<nfaRE::State*> origin(now);
    size_t left = 0, right = 0;
    std::pair<size_t, size_t> last;
    bool matched = false;
    for (size_t k = 0; k < target.length(); k++)
    {
        char i = target[k];
        for (auto&& j : now)
        {
            if (j->c == i or (j->c == Any and j->c != '\n'))
            {
                addState(j->out, next);
            }
        }
        if (next.empty())
        {
            left = right + 1;
            next = origin;
        }
        std::swap(now, next);
        next.clear();
        if (now.find(&Accept) != now.end())
        {
            last    = {left, right};
            matched = true;
            if (k + 1 < target.length())
            {
                i         = target[k + 1];
                bool flag = false;
                for (auto&& j : now)
                {
                    if (j->c == i or (j->c == Any and j->c != '\n'))
                    {
                        flag = true;
                        break;
                    }
                }
                if (not flag)
                {
                    left = right + 1;
                    now  = origin;
                }
            }
        }
        else
        {
            if (matched)
            {
                respos.push_back(last);
                matched = false;
            }
        }
        right++;
    }
    if (matched)
        respos.push_back(last);
    return respos;
}

std::unordered_set<__BASERE__::nfaRE::State*> __BASERE__::nfaRE::now{};
std::unordered_set<__BASERE__::nfaRE::State*> __BASERE__::nfaRE::next{};
const std::unordered_map<char, char> __BASERE__::nfaRE::ESCAPE{
    {18, '*'},
    {19, '.'},
    {20, '+'},
    {21, '?'},
    {22, '|'},
    {23, '('},
    {24, ')'},
};

bool __BASERE__::dfaRE::mcmp::operator()(const std::vector<State*>* a, const std::vector<State*>* b) const
{
    return *a < *b;
}
void __BASERE__::dfaRE::addState2(State* s, std::vector<State*>& stateSet)
{
    if (s == nullptr or ((s->c == Split or s->c == Merge) and s->searched))
        return;
    if (s->c == Split or s->c == Merge)
        s->searched = true;
    if (s->c == Split or s->c == Merge)
    {
        addState2(s->out, stateSet);
        addState2(s->out1, stateSet);
    }
    else
        stateSet.push_back(s);
    s->searched = false;
}
void __BASERE__::dfaRE::buildDfa(DState* dsta)
{
    if (allDState.size() > MAXDSTATELIMIT)
    {
        useNfa = true;
        return;
    }
    static bool vis[512];
    memset(vis, 0, sizeof(vis));
    for (auto&& i : dsta->n)
    {
        if (vis[i->c])
            continue;
        vis[i->c] = true;
        std::vector<State*> arr;
        for (auto&& j : dsta->n)
        {
            if (j->c == i->c or j->c == Any)
                addState2(j->out, arr), addState2(j->out1, arr);
        }
        if (arr.empty())
            continue;
        std::sort(begin(arr), end(arr));
        arr.erase(std::unique(begin(arr), end(arr)), end(arr));
        auto pos = allDState.find(&arr);
        if (pos == allDState.end())
        {
            DState* ndsta = new DState({{}, std::move(arr), false});
            allDState.insert({&(ndsta->n), ndsta});
            dsta->m[i->c] = ndsta;
        }
        else
        {
            dsta->m[i->c] = pos->second;
        }
        arr.clear();
    }
    dsta->searched = true;
    for (auto&& i : dsta->m)
    {
        if (not i.second->searched)
            buildDfa(i.second);
    }
}

__BASERE__::dfaRE::dfaRE(const size_t maxdstate)
{
    useNfa         = false;
    MAXDSTATELIMIT = maxdstate;
    DStart         = nullptr;
}

__BASERE__::dfaRE::dfaRE(const std::string& rex, const size_t maxdstate)
    : nfaRE(rex)
{
    useNfa         = false;
    MAXDSTATELIMIT = maxdstate;
    std::vector<State*> arr;
    addState2(Start, arr);
    std::sort(begin(arr), end(arr));
    DStart = new DState({{}, std::move(arr), false});
    allDState.insert({&(DStart->n), DStart});
    buildDfa(DStart);
    if (useNfa)
    {
        for (auto&& i : allDState)
            delete i.second;
        std::map<std::vector<State*>*, DState*, mcmp>().swap(allDState);
    }
}

__BASERE__::dfaRE::~dfaRE()
{
    for (auto&& i : allDState)
        delete i.second;
}

void __BASERE__::dfaRE::assign(const std::string& rex)
{
    for (auto&& i : allDState)
        delete i.second;
    std::map<std::vector<State*>*, DState*, mcmp>().swap(allDState);
    useNfa = false;
    nfaRE::assign(rex);
    std::vector<State*> arr;
    addState2(Start, arr);
    std::sort(begin(arr), end(arr));
    DStart = new DState({{}, std::move(arr), false});
    allDState.insert({&(DStart->n), DStart});
    buildDfa(DStart);
    if (useNfa)
    {
        for (auto&& i : allDState)
            delete i.second;
        std::map<std::vector<State*>*, DState*, mcmp>().swap(allDState);
    }
}

bool __BASERE__::dfaRE::match(const std::string& str)
{
    if (useNfa)
        return nfaRE::match(str);
    if (not DStart)
        throw std::logic_error("assign() before match()");
    DState* now = DStart;
    for (auto&& i : str)
    {
        if (now->m.find(i) != now->m.end())
            now = now->m[i];
        else if (now->m.find(Any) != now->m.end() and i != '\n')
            now = now->m[Any];
        else
            return false;
    }
    return std::binary_search(begin(now->n), end(now->n), &Accept);
}

std::vector<std::pair<size_t, size_t>> __BASERE__::dfaRE::search(const std::string& str)
{
    if (useNfa)
        return nfaRE::search(str);
    if (not DStart)
        throw std::logic_error("assign() before match()");
    std::vector<std::pair<size_t, size_t>> respos;
    DState* now = DStart;
    size_t left = 0, right = 0;
    std::pair<size_t, size_t> last;
    bool matched = false;
    for (size_t k = 0; k < str.length(); k++)
    {
        char i = str[k];
        if (now->m.find(i) != now->m.end())
            now = now->m[i];
        else if (now->m.find(Any) != now->m.end() and i != '\n')
            now = now->m[Any];
        else
            now = nullptr;
        if (now == nullptr)
        {
            now  = DStart;
            left = right + 1;
        }
        if (std::binary_search(begin(now->n), end(now->n), &Accept))
        {
            last    = {left, right};
            matched = true;
            if (k + 1 < str.length())
            {
                i         = str[k + 1];
                bool flag = false;
                if (now->m.find(i) != now->m.end())
                    flag = true;
                else if (now->m.find(Any) != now->m.end() and i != '\n')
                    flag = true;
                if (not flag)
                {
                    left = right + 1;
                    now  = DStart;
                }
            }
        }
        else
        {
            if (matched)
            {
                respos.push_back(last);
                matched = false;
            }
        }
        right++;
    }
    if (matched)
        respos.push_back(last);
    return respos;
}

RE::Regex::Regex(const size_t maxdstate)
{
    dfaRE::MAXDSTATELIMIT = maxdstate;
}

RE::Regex::Regex(const std::string& str, const size_t maxdstate)
    : dfaRE(parse2(parse(str)), maxdstate)
{
    ;
}

void RE::Regex::assign(const std::string& str)
{
    dfaRE::assign(parse2(parse(str)));
}
bool RE::Regex::match(const std::string& tar)
{
    return dfaRE::match(tar);
}

std::vector<std::pair<size_t, size_t>> RE::Regex::search(const std::string& str)
{
    return dfaRE::search(str);
}

std::string RE::Regex::parse(const std::string& source)
{
    using namespace std;
    auto next = [&source](size_t& i) -> char {
        if (i + 1 < source.size())
            return source[++i];
        else
            throw std::invalid_argument(source + " at [" + std::to_string(i) + "]");
    };

    std::string res;
    for (size_t i = 0; i < source.length(); i++)
    {
        char now = source[i];
        std::string tmp;
        char from;
        bool hasEscape = false;
        switch (now)
        {
        case '[':
            do
            {
                now = next(i);
                if (now == ']')
                    break;
                else if (now == '-')
                {
                    if (tmp.empty())
                        // throw std::invalid_argument(source + " at [" + std::to_string(i) + "]");
                        tmp.push_back('-');
                    else
                    {
                        from = tmp.back();
                        now  = next(i);
                        if (now == ']')
                        {
                            tmp.push_back('-');
                            break;
                        }
                        while (from < now)
                        {
                            tmp.push_back(++from);
                        }
                    }
                }
                else if (now == '\\')
                {
                    now = next(i);
                    switch (now)
                    {
                    case '\\':
                        hasEscape = true;
                        break;
                    case 'w':
                        tmp.append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
                        break;
                    case 'd':
                        tmp.append("0123456789");
                        break;
                    case 'n':
                        tmp.push_back(10);
                        break;
                    case 't':
                        tmp.push_back(9);
                        break;
                    case '*':
                        tmp.push_back(18);
                        break;
                    case '.':
                        tmp.push_back(19);
                        break;
                    case '+':
                        tmp.push_back(20);
                        break;
                    case '?':
                        tmp.push_back(21);
                        break;
                    case '|':
                        tmp.push_back(22);
                        break;
                    case '(':
                        tmp.push_back(23);
                        break;
                    case ')':
                        tmp.push_back(24);
                        break;
                    case '[':
                        hasEscape = true;
                        tmp.push_back('[');
                        break;
                    case ']':
                        hasEscape = true;
                        tmp.push_back(']');
                        break;
                    default:
                        throw std::logic_error("\\... Escape error!");
                        break;
                    }
                }
                else
                    tmp.push_back(now);
            } while (true);
            if (not tmp.empty())
            {
                std::sort(tmp.begin(), tmp.end());
                tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());
                res.push_back('(');
                for (auto&& i : tmp)
                {
                    res.push_back(i);
                    if (i != '\\')
                        res.push_back('|');
                }
                if (hasEscape)
                    res.append("\\)"), hasEscape = false;
                else
                    res.back() = ')';
                tmp.clear();
            }
            break;
        case '\\':
            now = next(i);
            switch (now)
            {
            case '\\':
                res.push_back(now);
                break;
            case 'w':
                res.append("(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9|_)");
                break;
            case 'd':
                res.append("(0|1|2|3|4|5|6|7|8|9)");
                break;
            case 'n':
                res.push_back(10);
                break;
            case 't':
                res.push_back(9);
                break;
            case '*':
                res.push_back(18);
                break;
            case '.':
                res.push_back(19);
                break;
            case '+':
                res.push_back(20);
                break;
            case '?':
                res.push_back(21);
                break;
            case '|':
                res.push_back(22);
                break;
            case '(':
                res.push_back(23);
                break;
            case ')':
                res.push_back(24);
                break;
            case '[':
            case ']':
            case '{':
            case '}':
                res.push_back('\\');
                res.push_back(now);
                break;
            default:
                throw std::logic_error("\\... Escape error!");
                break;
            }
            break;
        default:
            res.push_back(now);
            break;
        }
    }
    // cout << res << endl;
    return res;
}

// process {1,3} {1,} {3}
std::string RE::Regex::parse2(const std::string& src)
{
    using namespace std;
    auto error = [&src](int i) { throw std::invalid_argument(src + " at [" + std::to_string(i) + "]"); };
    auto next  = [&src](size_t& i) -> char {
        if (i + 1 < src.size())
            return src[++i];
        else
            throw std::invalid_argument(src + " at [" + std::to_string(i) + "]");
    };
    string res, tmp;
    int n1 = 0, n2 = 0;
    char ch;
    int l = 0, r = 0;
    int bracNum = 0;
    for (size_t i = 0; i < src.length(); i++)
    {
        if (src[i] == '{')
        {
            n1 = n2 = 0;
            r       = i;
            ch      = next(i);
            while (isdigit(ch))
            {
                n1 = n1 * 10 + ch - '0';
                ch = next(i);
            }
            if (ch == ',')
            {
                ch = next(i);
                if (ch == '}')
                { //a{3,}
                    switch (src.at(r - 1))
                    {
                    case ')':
                        for (l = res.size() - 1; l >= 0; l--)
                        {
                            if (res[l] == ')')
                                bracNum++;
                            else if (res[l] == '(')
                                bracNum--;
                            if (bracNum == 0)
                                break;
                        }
                        tmp = res.substr(l, res.size() - l);
                        for (int j = 0; j < n1; j++)
                        {
                            res.append(tmp);
                        }
                        res.push_back('*');
                        break;
                    case '*':
                    case '|':
                    case '?':
                    case '{':
                        error(r - 1);
                        break;
                    default:
                        for (int j = 0; j < n1; j++)
                        {
                            res.push_back(src.at(r - 1));
                        }
                        res.push_back('*');
                        break;
                    }
                    continue;
                }
                while (isdigit(ch))
                {
                    n2 = n2 * 10 + ch - '0';
                    ch = next(i);
                }
                if (ch == '}')
                {
                    if (n1 >= n2)
                        error(i);
                    switch (src.at(r - 1))
                    {
                    case ')':
                        for (l = res.size() - 1; l >= 0; l--)
                        {
                            if (res[l] == ')')
                                bracNum++;
                            else if (res[l] == '(')
                                bracNum--;
                            if (bracNum == 0)
                                break;
                        }
                        if (res.at(l) != '(')
                            error(l);
                        tmp = res.substr(l, res.size() - l);
                        res.erase(l, res.size() - l);
                        if (n1 == 0)
                        {
                            res.push_back('(');
                            res.append(tmp);
                            res.push_back('?');
                            std::string tmp2 = tmp;
                            for (int j = 1; j < n2; j++)
                            {
                                tmp2.append(tmp);
                                res.push_back('|');
                                res.append(tmp2);
                            }
                            res.push_back(')');
                        }
                        else
                        {
                            std::string tmp2;
                            for (int j = 0; j < n1; j++)
                            {
                                tmp2.append(tmp);
                            }
                            res.push_back('(');
                            res.append(tmp2);
                            while (n1++ < n2)
                            {
                                res.push_back('|');
                                tmp2.append(tmp);
                                res.append(tmp2);
                            }
                            res.push_back(')');
                        }
                        break;
                    case '*':
                    case '|':
                    case '?':
                    case '{':
                        error(r - 1);
                        break;
                    default:
                        res.pop_back();
                        if (n1 == 0)
                        {
                            res.push_back('(');
                            res.push_back(src.at(r - 1));
                            res.push_back('?');
                            std::string tmp2;
                            tmp2.push_back(src.at(r - 1));
                            for (int j = 1; j < n2; j++)
                            {
                                res.push_back('|');
                                tmp2.push_back(src.at(r - 1));
                                res.append(tmp2);
                            }
                            res.push_back(')');
                        }
                        else
                        {
                            std::string tmp2;
                            for (int j = 0; j < n1; j++)
                            {
                                tmp2.push_back(src.at(r - 1));
                            }
                            res.push_back('(');
                            res.append(tmp2);
                            while (n1++ < n2)
                            {
                                res.push_back('|');
                                tmp2.push_back(src.at(r - 1));
                                res.append(tmp2);
                            }
                            res.push_back(')');
                        }
                        break;
                    }
                }
                else
                    error(i);
            }
            else if (ch == '}')
            { //a{3}
                switch (src.at(r - 1))
                {
                case ')':
                    for (l = res.size() - 1; l >= 0; l--)
                    {
                        if (res[l] == ')')
                            bracNum++;
                        else if (res[l] == '(')
                            bracNum--;
                        if (bracNum == 0)
                            break;
                    }
                    tmp = res.substr(l, res.size() - l);
                    res.erase(l, res.size() - l);
                    for (int j = 0; j < n1; j++)
                    {
                        res.append(tmp);
                    }
                    break;
                case '*':
                case '|':
                case '?':
                case '{':
                    error(r - 1);
                    break;
                default:
                    res.pop_back();
                    for (int j = 0; j < n1; j++)
                    {
                        res.push_back(src.at(r - 1));
                    }
                    break;
                }
            }
            else
            {
                error(i);
            }
        }
        else if (src[i] == '\\')
        {
            if (i + 1 < src.length())
            {
                ch = next(i);
                switch (ch)
                {
                case '[':
                case ']':
                case '{':
                case '}':
                    res.push_back(ch);
                    break;
                default:
                    res.push_back('\\');
                    res.push_back(ch);
                    break;
                }
            }
            else
                res.push_back('\\');
        }
        else
        {
            res.push_back(src[i]);
        }
    }
    // cout << res << endl;
    return res;
}
