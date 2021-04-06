#ifndef DFARE
#define DFARE
#include "../nfaRE--/nfaRE--.hpp"
#include <algorithm>
#include <cstring>
#include <map>
#include <vector>
namespace RE
{
class dfaRE : protected RE::nfaRE
{
  protected:
    size_t MAXDSTATELIMIT;
    struct DState
    {
        std::unordered_map<int, DState*> m;
        std::vector<State*> n;
        bool searched;
    } * DStart;

    bool useNfa;

    struct mcmp
    {
        bool operator()(const std::vector<State*>* a, const std::vector<State*>* b) const
        {
            return *a < *b;
        }
    };

    std::map<std::vector<State*>*, DState*, mcmp> allDState;

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

  public:
    dfaRE(const std::string& rex, size_t maxdstate = 32)
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
            if (now->m.find(i) != now->m.end())
                now = now->m[i];
            else if (now->m.find(Any) != now->m.end())
                now = now->m[Any];
            else
                return false;
        }
        return std::binary_search(begin(now->n), end(now->n), &Accept);
    }
};

} // namespace RE

#endif