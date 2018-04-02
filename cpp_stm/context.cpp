#include "context.h"


namespace stm
{


Context::Context()
    : _generator()
    , _distribution(std::uniform_int_distribution<int>(1, 16))
    , _dice(std::bind(_distribution, _generator))
{
}

void Context::takeLock()
{
    _lock.lock();
}

void Context::releaseLock()
{
    _lock.unlock();
}

TVars Context::takeSnapshot()
{
    takeLock();
    auto tvars = _tvars;
    releaseLock();
    return tvars;
}

TVarId Context::newGUID()
{
    takeLock();
    auto guid = utils::newGUID(_dice);
    releaseLock();
    return guid;
}

bool Context::tryCommit(const UStamp& ustamp, const TVars& stagedTvars)
{
    takeLock();

    bool conflict = false;
    for (auto it = stagedTvars.begin(); it != stagedTvars.end(); ++it)
    {
        TVarId stagedTVarId = it->first;
        auto found = _tvars.find(stagedTVarId);

        if (found == _tvars.end())
            continue;

        TVarHandle stagedTVarHandle = it->second;
        TVarHandle origTVarHandle = found->second;

        if (stagedTVarHandle.ustamp != origTVarHandle.ustamp)
        {
            conflict = true;
            break;
        }
    }

    if (!conflict)
    {
        for (auto it = stagedTvars.begin(); it != stagedTvars.end(); ++it)
        {
            TVarId stagedTVarId = it->first;
            TVarHandle stagedTVarHandle = it->second;
            stagedTVarHandle.ustamp = ustamp;
            _tvars[stagedTVarId] = stagedTVarHandle;
        }
    }

    releaseLock();
    return !conflict;
}

AtomicRuntime::AtomicRuntime(Context &context, const UStamp &ustamp, const TVars &tvars)
    : _context(context)
    , _ustamp(ustamp)
    , _localTVars(tvars)
{
}

TVarId AtomicRuntime::newGUID()
{
    return _context.newGUID();
}

UStamp AtomicRuntime::getUStamp() const
{
    return _ustamp;
}

TVars AtomicRuntime::getStagedTVars() const
{
    return _localTVars;
}

void AtomicRuntime::addTVarHandle(const TVarId& tvarId, const TVarHandle& tvarHandle)
{
    if (_localTVars.find(tvarId) != _localTVars.end())
        throw std::runtime_error("TVar is not unique!");

    _localTVars[tvarId] = tvarHandle;
}

TVarHandle AtomicRuntime::getTVarHandle(const TVarId& tvarId) const
{
    auto found = _localTVars.find(tvarId);
    if (found == _localTVars.end())
        throw std::runtime_error("TVar not found.");
    return found->second;
}

void AtomicRuntime::setTVarHandleData(const TVarId& tvarId, const std::any& data)
{
    _localTVars[tvarId].data = data;
}

} // namespace stm
