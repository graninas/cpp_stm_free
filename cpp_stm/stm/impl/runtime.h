#ifndef STM_IMPL_RUNTIME_H
#define STM_IMPL_RUNTIME_H

#include <chrono>
#include <thread>
#include <iostream>
#include <random>

#include "../context.h"

namespace stm
{

template <typename A>
using RunnerFunc = std::function<RunResult<A>(AtomicRuntime&)>;

template <typename A>
A runSTM(Context& context,
         const RunnerFunc<A>& runner)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 100);
    auto backoffIntervalDice = std::bind(distribution, generator);
    auto ustamp = context.newId();

    while (true)
    {
        auto snapshot = context.takeSnapshot();
        AtomicRuntime runtime {context, ustamp, snapshot};
        RunResult<A> runResult = runner(runtime);
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

} // namespace stm

#endif // STM_IMPL_RUNTIME_H
