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
    size_t stateNum;
    std::unordered_set<int> charset;

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
            if (i->c != Match)
                charset.insert(i->c);
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
                stateNum++;
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
        dsta->searched = false;
    }

    void minilizeDfa()
    {
        if (false)
            return;
        using namespace std;
        unordered_map<DState*, DState*> _m;

        unordered_map<DState*, int> unionid;

        unordered_map<int, vector<DState*>> group;

        vector<DState*> ac, nac;
        for (auto&& i : allDState)
        {
            if (binary_search(begin(i.second->n), end(i.second->n), &Accept))
                ac.push_back(i.second), unionid[i.second] = 0;
            else
                nac.push_back(i.second), unionid[i.second] = 1;
        }
        sort(begin(ac), end(ac));
        sort(begin(nac), end(nac));
        group.insert({0, move(ac)});
        group.insert({1, move(nac)});
        int lastUnionId = 1;
        stack<int> _stack;
        _stack.push(0);
        _stack.push(1);
        while (not _stack.empty())
        {
            auto nowid = _stack.top();
            _stack.pop();
            auto pos = group.find(nowid);
            if (pos == group.end())
                continue;
            if (pos->second.size() == 1)
            {
                _m[pos->second.front()] = pos->second.front();
                continue;
            }
            unordered_map<int, vector<DState*>> spgroup;
            bool flag = false;
            for (auto&& ch : charset)
            {
                for (auto&& st : pos->second)
                {
                    auto cpos = st->m.find(ch);
                    if (cpos == st->m.end())
                    {
                        spgroup[-1].push_back(st);
                    }
                    else
                    {
                        spgroup[unionid[cpos->second]].push_back(st);
                    }
                }
                if (spgroup.size() == 1)
                {
                    spgroup.clear();
                }
                else
                {
                    flag = true;
                    for (auto&& i : spgroup)
                    {
                        lastUnionId++;
                        for (auto&& j : i.second)
                        {
                            unionid[j] = lastUnionId;
                        }
                        group.insert({lastUnionId, move(i.second)});
                        _stack.push(lastUnionId);
                    }
                    break;
                }
            }
            if (not flag)
            {
                auto startpos = lower_bound(begin(pos->second), end(pos->second), DStart);
                if (startpos != pos->second.end() and *startpos == DStart)
                {
                    ;
                }
                else
                {
                    startpos = pos->second.begin();
                }
                for (auto&& i : pos->second)
                {
                    if (i != *startpos)
                    {
                        _m[i] = *startpos;
                        delete i;
                        stateNum--;
                    }
                }
            }
        }
        dfsRebuild(DStart, _m);
    }
    void dfsRebuild(DState* now, std::unordered_map<DState*, DState*>& rep)
    {
        using namespace std;
        if (now == nullptr)
            return;
        now->searched = true;
        for (auto&& i : now->m)
        {
            i.second = rep[i.second];
            if (not i.second->searched)
                dfsRebuild(i.second, rep);
        }
        now->searched = false;
    }

    std::vector<std::pair<size_t, size_t>> nonGreadySearch(const std::string& str)
    {
        if (not DStart)
            throw std::logic_error("assign() before search()");
        std::vector<std::pair<size_t, size_t>> respos;
        DState* now = DStart;
        size_t left = 0, right = 0;
        for (size_t k = 0; k < str.length(); k++)
        {
            char i = str[k];
            if (now == DStart)
            {
                left = right;
            }
            if (now->m.find(i) != now->m.end())
                now = now->m[i];
            else if (now->m.find(Any) != now->m.end() and i != '\n')
                now = now->m[Any];
            else
                now = nullptr;
            if (now and std::binary_search(begin(now->n), end(now->n), &Accept))
            {
                respos.push_back({left, right});
                left = right + 1;
                now  = DStart;
            }
            else if (now == nullptr)
            {
                now = DStart;
            }
            right++;
        }
        return respos;
    }
    std::vector<std::pair<size_t, size_t>> greadySearch(const std::string& str)
    {
        if (not DStart)
            throw std::logic_error("assign() before search()");
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

  public:
    dfaRE(const size_t maxdstate = 128)
    {
        stateNum       = 0;
        useNfa         = false;
        MAXDSTATELIMIT = maxdstate;
        DStart         = nullptr;
    }
    dfaRE(const std::string& rex, const size_t maxdstate = 128)
        : nfaRE(rex)
    {
        stateNum       = 0;
        useNfa         = false;
        MAXDSTATELIMIT = maxdstate;
        std::vector<State*> arr;
        addState2(Start, arr);
        std::sort(begin(arr), end(arr));
        DStart = new DState({{}, std::move(arr), false});
        stateNum++;
        allDState.insert({&(DStart->n), DStart});
        buildDfa(DStart);
        if (useNfa)
        {
            for (auto&& i : allDState)
                delete i.second;
            std::map<std::vector<State*>*, DState*, mcmp>().swap(allDState);
        }
        else
        {
            minilizeDfa();
        }
    }
    ~dfaRE()
    {
        std::cout << "dfa:" << stateNum << std::endl;
        for (auto&& i : allDState)
            delete i.second;
    }
    void assign(const std::string& rex)
    {
        std::cout << "dfa:" << stateNum << std::endl;
        stateNum = 0;
        for (auto&& i : allDState)
            delete i.second;
        std::map<std::vector<State*>*, DState*, mcmp>().swap(allDState);

        RE::nfaRE::assign(rex);

        useNfa = false;
        std::vector<State*> arr;
        addState2(Start, arr);
        std::sort(begin(arr), end(arr));
        DStart = new DState({{}, std::move(arr), false});
        stateNum++;
        allDState.insert({&(DStart->n), DStart});
        buildDfa(DStart);
        if (useNfa)
        {
            for (auto&& i : allDState)
                delete i.second;
            std::map<std::vector<State*>*, DState*, mcmp>().swap(allDState);
        }
        else
        {
            minilizeDfa();
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
    std::vector<std::pair<size_t, size_t>> search(const std::string& str, bool isGreadySearch = true)
    {
        if (useNfa)
            return nfaRE::search(str, isGreadySearch);
        else if (isGreadySearch)
            return greadySearch(str);
        else
            return nonGreadySearch(str);
    }
};

} // namespace RE

#endif