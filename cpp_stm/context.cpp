#include "context.h"


namespace stm
{


Context::Context()
    : _generator()
    , _distribution(std::uniform_int_distribution<int>(1, 16))
    , _dice(std::bind(_distribution, _generator))
{
}

TVarId Context::newGUID()
{
    _lock.lock();
    auto guid = utils::newGUID(_dice);
    _lock.unlock();
    return guid;
}

AtomicRuntime::AtomicRuntime(Context &context, UStamp ustamp)
    : _context(context)
    , _ustamp(ustamp)
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
