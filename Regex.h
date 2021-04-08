/*
* RE--
* Head File
*
* @author:delta-in-hub
* @date:2021/04/07
* https://github.com/Delta-in-hub/RE--
*/
#ifndef REGEX
#define REGEX
#include <map>
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
    } Accept{Match, nullptr, nullptr}, *Start;
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
    void _delete(State* now);
    void addState(State* s, std::unordered_set<State*>& stateSet);

  public:
    nfaRE();
    nfaRE(const std::string& rex);
    ~nfaRE();
    void assign(const std::string& rex);
    bool match(const std::string& target);
    std::vector<std::pair<size_t, size_t>> search(const std::string& target);
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

    std::map<std::vector<State*>*, DState*, mcmp> allDState;
    void addState2(State* s, std::vector<State*>& stateSet);
    void buildDfa(DState* dsta);

  public:
    dfaRE(const size_t maxdstate = 64);
    dfaRE(const std::string& rex, const size_t maxdstate = 64);
    ~dfaRE();
    void assign(const std::string& rex);
    bool match(const std::string& str);
    std::vector<std::pair<size_t, size_t>> search(const std::string& target);
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

    Regex(const std::string& str, const size_t maxdstate = 256);

    void assign(const std::string& str);
    bool match(const std::string& tar);
    std::vector<std::pair<size_t, size_t>> search(const std::string& target);
};
} // namespace RE

#endif
