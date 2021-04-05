#ifndef DFARE
#define DFARE
#include "../nfaRE--/nfaRE--.hpp"
#include <algorithm>
#include <cstring>
#include <map>
#include <vector>
namespace RE
{
constexpr size_t MAXDSTATELIMIT = 32;
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
    std::map<std::vector<State*>, DState*> allDState;

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
        if (allDState.size() > MAXDSTATELIMIT)
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
            auto pos = allDState.find(arr);
            if (pos == allDState.end())
            {
                DState* ndsta = dstate(arr);
                allDState.insert({std::move(arr), ndsta});
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
        : nfaRE(rex), useNfa(false)
    {
        std::vector<State*> arr;
        addState2(Start, arr);
        std::sort(begin(arr), end(arr));
        DStart = dstate(arr);
        allDState.insert({std::move(arr), DStart});
        buildDfa(DStart);
        if (useNfa)
        {
            for (auto&& i : allDState)
                delete i.second;
            std::map<std::vector<State*>, DState*>().swap(allDState);
        }
    }
    ~dfaRE()
    {
        for (auto&& i : allDState)
            delete i.second;
    }
    bool match(const std::string& str)
    {
        if (useNfa)
            return nfaRE::match(str);
        DState* now = DStart;
        for (auto&& i : str)
        {
            auto pos = now->m.find(i);
            if (pos != now->m.end())
                now = pos->second;
            else
                return false;
        }
        return std::binary_search(begin(now->n), end(now->n), &Accept);
    }
};

} // namespace RE

#endif