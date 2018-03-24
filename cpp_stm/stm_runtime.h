#ifndef STM_RUNTIME_H
#define STM_RUNTIME_H

#include <chrono>
#include <thread>

#include "context.h"
#include "stm_interpreter.h"

namespace stm
{

template <typename A>
A runSTM(Context& context, const STML<A>& stml)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(500, 500000);
    auto backoffIntervalDice = std::bind(distribution, generator);

    while (true)
    {
        auto snapshot = context.takeSnapshot();
        auto ustamp = context.newGUID();
        AtomicRuntime runtime {context, ustamp, snapshot};
        RunResult<A> runResult = runSTML<A, stm::StmlVisitor>(runtime, stml);

        if (runResult.retry)
            continue;

        bool success = context.tryCommit(ustamp, runtime.getStagedTVars());
        if (success)
        {
            return runResult.result.value();
        }

        std::chrono::microseconds interval(backoffIntervalDice());
        std::this_thread::sleep_for(interval);
    }
}

} // namespace stm

#endif // STM_RUNTIME_H
