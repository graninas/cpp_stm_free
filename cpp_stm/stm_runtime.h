#ifndef STM_RUNTIME_H
#define STM_RUNTIME_H

#include <chrono>
#include <thread>
#include <iostream>

#include "context.h"
#include "stm_interpreter.h"

namespace stm
{
namespace free
{

template <typename A>
A runSTM(Context& context, const STML<A>& stml)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 100);
    auto backoffIntervalDice = std::bind(distribution, generator);
    auto ustamp = context.newGUID();

    while (true)
    {
        auto snapshot = context.takeSnapshot();
        AtomicRuntime runtime {context, ustamp, snapshot};
        RunResult<A> runResult = runSTML<A, StmlVisitor>(runtime, stml);

        if (runResult.retry)
        {
            auto t = backoffIntervalDice();
//            std::cout << "Retry after: " << t * 10 << " milliseconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(t * 10));
            continue;
        }

        bool success = context.tryCommit(ustamp, runtime.getStagedTVars());
        if (success)
        {
            return runResult.result.value();
        }
    }
}

} // namespace free
} // namespace stm

#endif // STM_RUNTIME_H
