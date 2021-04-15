/*
* RE--
* Head File
*
* @author:delta-in-hub
* https://github.com/Delta-in-hub/RE--
*/
#ifndef __REGEX
#define __REGEX
#include <ciso646>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace __BASERE__
{
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
    } Accept, *Start;
    struct Frag
    {
        State* head;
        State* tail;
    };

    static std::unordered_set<State*> now, next;
    static const std::unordered_map<char, char> ESCAPE;

    State* state(int c, State* out, State* out1);
    State* postToNfa(const std::string& rex);
    std::string rexToPostRex(const std::string& rex);
    void _delete(State* now, std::unordered_set<State*>& dustbin);
    void addState(State* s, std::unordered_set<State*>& stateSet);
    std::vector<std::pair<size_t, size_t>> greadySearch(const std::string& target);
    std::vector<std::pair<size_t, size_t>> nonGreadySearch(const std::string& target);

  public:
    nfaRE();
    nfaRE(const std::string& rex);
    ~nfaRE();
    void assign(const std::string& rex);
    bool match(const std::string& target);
    std::vector<std::pair<size_t, size_t>> search(const std::string& target, bool isGreadySearch = true);
};

class dfaRE : protected nfaRE
{
  protected:
    size_t MAXDSTATELIMIT;
    struct DState
    {
        std::unordered_map<int, DState*> m;
        std::vector<State*> n;
        bool searched;
    } * DStart;
    struct mcmp
    {
        bool operator()(const std::vector<State*>* a, const std::vector<State*>* b) const;
    };
    bool useNfa;
    std::unordered_set<int> charset;

    std::map<std::vector<State*>*, DState*, mcmp> allDState;
    void addState2(State* s, std::vector<State*>& stateSet);
    void buildDfa(DState* dsta);
    std::vector<std::pair<size_t, size_t>> greadySearch(const std::string& target);
    std::vector<std::pair<size_t, size_t>> nonGreadySearch(const std::string& target);
    void dfsRebuild(DState* now, std::unordered_map<DState*, DState*>& rep);
    void minimizeDfa();

  public:
    dfaRE(const size_t maxdstate = 64);
    dfaRE(const std::string& rex, const size_t maxdstate = 64);
    ~dfaRE();
    void assign(const std::string& rex);
    bool match(const std::string& str);
    std::vector<std::pair<size_t, size_t>> search(const std::string& target, bool isGreadySearch = true);
};

}; // namespace __BASERE__

namespace RE
{
class Regex : protected __BASERE__::dfaRE
{
  protected:
    std::string parse(const std::string& source);
    std::string parse2(const std::string& src);

  public:
    Regex(const size_t maxdstate = 256);

    Regex(const std::string& regexp, const size_t maxdstate = 256);

    void assign(const std::string& regexp);
    bool match(const std::string& target);
    std::vector<std::pair<size_t, size_t>> search(const std::string& target, bool isGreadySearch = true);
};
} // namespace RE

#endif
