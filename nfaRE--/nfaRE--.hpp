#ifndef NFARE
#define NFARE
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include <iostream>
namespace RE
{

// #define catenate ('.') // assume dot('.') as an explicit concatenation operator.
// constexpr char Catenate = ((char)(20));
class nfaRE
{
  protected:
    enum
    {
        Catenate = 17, //! 17 , 18-24  are reserved.
        Split    = 256,
        Merge    = 257,
        Match    = 258,
        Any      = 259,
    };
    struct State
    {
        int c;
        State* out;
        State* out1;
        bool searched = false;
    } Accept{Match, nullptr, nullptr}, *Start;

    static std::unordered_set<State*> now, next;
    static const std::unordered_map<char, char> ESCAPE;
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
    std::string rexToPostRex(const std::string& rex)
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

  public:
    nfaRE()
    {
        Start = nullptr;
    }
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
    void assign(const std::string& rex)
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
    bool match(const std::string& target)
    {
        now.clear(), next.clear();
        if(not Start)
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
};

std::unordered_set<nfaRE::State*> nfaRE::now{};
std::unordered_set<nfaRE::State*> nfaRE::next{};
const std::unordered_map<char, char> nfaRE::ESCAPE{
    {18, '*'},
    {19, '.'},
    {20, '+'},
    {21, '?'},
    {22, '|'},
    {23, '('},
    {24, ')'},
};

} // namespace RE

#endif