#ifndef DFARE
#define DFARE
#include "../nfaRE--/nfaRE--.hpp"

namespace RE
{
class dfaRE : protected RE::nfaRE
{
  protected:
  public:
    dfaRE(const std::string& rex)
        : nfaRE(rex)
    {
        ;
    }
};
} // namespace RE

#endif