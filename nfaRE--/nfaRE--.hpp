#ifndef NFARE
#define NFARE
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>

namespace RE
{

// #define catenate ('.') // assume dot('.') as an explicit concatenation operator.
#define catenate ((char)(20))
class nfaRE
{
  protected:
    enum
    {
        Split = 256,
        Merge = 257,
        Match = 258,
    };
    struct State
    {
        int c;
        State* out;
        State* out1;
    } Accept{Match, nullptr, nullptr}, *Start;
    static std::unordered_set<State*> now, next;
    struct Frag
    {
        State* head;
        State* tail;
    };
    State* state(int c, State* out, State* out1)
    {
        State* t = new State;
        t->c     = c;
        t->out   = out;
        t->out1  = out1;
        return t;
    }
    State* postToNfa(const std::string& rex)
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
            case catenate:
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
    std::string rexToPostRex(const std::string& rex)
    {
        using namespace std;
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
    void _delete(State* now)
    {

        if (now == nullptr or now == &Accept or next.find(now) != next.end())
            return;
        next.insert(now);
        _delete(now->out);
        _delete(now->out1);
    }
    void addState(State* s, std::unordered_set<State*>& stateSet)
    {
        if (s == nullptr)
            return;
        if (s->c == Split or s->c == Merge)
            addState(s->out, stateSet), addState(s->out1, stateSet);
        else
            stateSet.insert(s);
    }

  public:
    nfaRE(const std::string& rex)
    {
        Start = postToNfa(rexToPostRex(rex));
    }
    ~nfaRE()
    {
        next.clear();
        _delete(Start);
        for (auto&& i : next)
        {
            delete i;
        }
        next.clear();
    }
    bool match(const std::string& target)
    {
        now.clear(), next.clear();
        addState(Start, now);
        for (auto&& i : target)
        {
            for (auto&& j : now)
            {
                if (j->c == i)
                {
                    addState(j->out, next);
                }
            }
            std::swap(now, next);
            next.clear();
        }
        return now.find(&Accept) != now.end();
    }
};
#undef catenate

std::unordered_set<nfaRE::State*> nfaRE::now{};
std::unordered_set<nfaRE::State*> nfaRE::next{};

} // namespace RE

#endif