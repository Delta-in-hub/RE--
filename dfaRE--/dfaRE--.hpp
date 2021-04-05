#ifndef DFARE
#define DFARE
#include "../nfaRE--/nfaRE--.hpp"
#include <algorithm>
#include <cstring>
#include <map>
#include <vector>
namespace RE
{
constexpr size_t MAXSTATELIMIT = 32;
class dfaRE : protected RE::nfaRE
{
  protected:
    struct DState
    {
        std::unordered_map<char, DState*> m;
        std::vector<State*> n;
        bool searched;
    } * DStart;

    DState* dstate(const std::vector<State*>& s)
    {
        return new DState({{}, s, false});
    }
    bool useNfa;
    std::map<std::vector<State*>, DState*> allState;

    void addState2(State* s, std::vector<State*>& stateSet)
    {
        if (s == nullptr)
            return;
        if (s->c == Split or s->c == Merge)
            addState2(s->out, stateSet), addState2(s->out1, stateSet);
        else
            stateSet.push_back(s);
    }

    void buildDfa(DState* dsta)
    {
        if (allState.size() > MAXSTATELIMIT)
        {
            useNfa = true;
            return;
        }
        static bool vis[256];
        memset(vis, 0, sizeof(vis));
        for (auto&& i : dsta->n)
        {
            if (vis[i->c])
                continue;
            vis[i->c] = true;
            std::vector<State*> arr;
            for (auto&& j : dsta->n)
            {
                if (j->c == i->c)
                    addState2(j->out, arr), addState2(j->out1, arr);
            }
            std::sort(begin(arr), end(arr));
            auto pos = allState.find(arr);
            if (pos == allState.end())
            {
                DState* ndsta = dstate(arr);
                allState.insert({std::move(arr), ndsta});
                dsta->m[i->c] = ndsta;
            }
            else
            {
                dsta->m[i->c] = pos->second;
            }
        }
        dsta->searched = true;
        for (auto&& i : dsta->m)
        {
            if (not i.second->searched)
                buildDfa(i.second);
        }
    }

  public:
    dfaRE(const std::string& rex)
        : nfaRE(rex)
    {
        useNfa = false;
        std::vector<State*> arr;
        addState2(Start, arr);
        std::sort(begin(arr), end(arr));
        DStart = dstate(arr);
        allState.insert({std::move(arr), DStart});
        buildDfa(DStart);
        if (useNfa)
        {
            for (auto&& i : allState)
                delete i.second;
            std::map<std::vector<State*>, DState*>().swap(allState);
        }
    }
    ~dfaRE()
    {
        for (auto&& i : allState)
            delete i.second;
    }
    bool match(const std::string& str)
    {
        if (useNfa)
            return nfaRE::match(str);
        DState *now = DStart, *next;
        for (auto&& i : str)
        {
            if (now->m.find(i) != now->m.end())
                next = now->m.at(i);
            else
                return false;
            now = next;
        }
        return std::binary_search(begin(now->n), end(now->n), &Accept);
    }
};

} // namespace RE

#endif