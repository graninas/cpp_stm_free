#ifndef STM_CONTEXT_H
#define STM_CONTEXT_H

#include <map>
#include <any>
#include <mutex>
#include <optional>
#include <atomic>

#include "tvar.h"

namespace stm
{

using UStamp = Id;

struct TVarHandle
{
    UStamp ustamp;
    std::any data;
    bool modified;
};

using TVars = std::map<TVarId, TVarHandle>;

class Context
{
private:

    std::atomic<Id> _id;
    TVars _tvars;

    std::mutex _lock;

public:
    Context();

    bool tryCommit(const UStamp& ustamp, const TVars& stagedTvars);

    Id newId();
    TVars takeSnapshot();
};

class AtomicRuntime
{
private:
    Context& _context;

    UStamp _ustamp;
    TVars _localTVars;

public:
    AtomicRuntime(Context& context, const UStamp& ustamp, const TVars& tvars);

    TVarId newId();
    UStamp getUStamp() const;
    TVars getStagedTVars() const;

    void addTVarHandle(const TVarId& tvarId, const TVarHandle& tvarHandle);
    TVarHandle getTVarHandle(const TVarId& tvarId) const;
    void setTVarHandleData(const TVarId& tvarId, const std::any& data);
};

template <typename A>
struct RunResult
{
    bool retry;
    std::optional<A> result;
};

} // namespace stm

#endif // STM_CONTEXT_H
