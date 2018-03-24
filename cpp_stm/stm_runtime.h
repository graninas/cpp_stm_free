#ifndef STM_RUNTIME_H
#define STM_RUNTIME_H

#include <chrono>
#include <thread>

#include "context.h"
#include "stm_interpreter.h"

namespace stm
{

//takeSnapshot :: Context -> IO (UStamp, TVars)
//takeSnapshot (Context mtvars) = do
//  tvars <- takeMVar mtvars
//  tvarKVs <- mapM cloneTVarHandle (Map.toList tvars)
//  putMVar mtvars tvars
//  ustamp <- newUnique
//  pure (ustamp, Map.fromList tvarKVs)

//runSTM :: Int -> Context -> STML a -> IO a
//runSTM delay ctx stml = do
//  (ustamp, snapshot)                  <- takeSnapshot ctx
//  (eRes, AtomicRuntime _ stagedTVars) <- runStateT (runSTML stml) (AtomicRuntime ustamp snapshot)
//  case eRes of
//    Left RetryCmd -> runSTM (delay * 2) ctx stml      -- TODO: tail recursion
//    Right res     -> do
//      success <- tryCommit ctx ustamp stagedTVars
//      if success
//        then return res
//        else runSTM (delay * 2) ctx stml      -- TODO: tail recursion



template <typename A>
A runSTM(Context& context, const STML<A>& stml)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(500, 500000);
    auto backoffIntervalDice = std::bind(distribution, generator);

    while (true)
    {
        StmResult<A> result;
        auto snapshot = context.takeSnapshot();
        auto ustamp = context.newGUID();
        AtomicRuntime runtime {context, ustamp, snapshot};
        result = runSTML(runtime, stml);

        if (result.retry)
            continue;

        bool success = context.tryCommit(ustamp, result.stagedTVars.value());
        if (success)
        {
            return result.result.value();
        }

        std::chrono::microseconds interval(backoffIntervalDice());
        std::this_thread::sleep_for(interval);
    }
}

} // namespace stm

#endif // STM_RUNTIME_H
