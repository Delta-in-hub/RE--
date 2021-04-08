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

  public:
    dfaRE(const size_t maxdstate = 128)
    {
        useNfa         = false;
        MAXDSTATELIMIT = maxdstate;
        DStart         = nullptr;
    }
    dfaRE(const std::string& rex, const size_t maxdstate = 128)
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
    void assign(const std::string& rex)
    {
        for (auto&& i : allDState)
            delete i.second;
        std::map<std::vector<State*>*, DState*, mcmp>().swap(allDState);

        RE::nfaRE::assign(rex);

        useNfa = false;
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
    bool match(const std::string& str)
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
    std::vector<std::pair<size_t, size_t>> search(const std::string& str)
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
};

} // namespace RE

#endif