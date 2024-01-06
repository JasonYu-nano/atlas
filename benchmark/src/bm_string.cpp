// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <any>

#include "benchmark/benchmark.h"

#include "string/string.hpp"

using namespace atlas;

#define ITERATION_TIMES (10000000)

static void BM_SmallStringCreation(benchmark::State& state)
{
    std::any var(u8"small string");
    auto str = std::any_cast<const char8_t*>(var);
    for (auto _ : state)
        benchmark::DoNotOptimize(String(str));
}
BENCHMARK(BM_SmallStringCreation)->Iterations(ITERATION_TIMES);

static void BM_SmallStdStringCreation(benchmark::State& state)
{
    std::any var("small string");
    auto str = std::any_cast<const char*>(var);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(std::string(str));
    }
}
BENCHMARK(BM_SmallStdStringCreation)->Iterations(ITERATION_TIMES);

static void BM_LargeStringCreation(benchmark::State& state)
{
    std::any var(u8"large string over 15 length");
    auto str = std::any_cast<const char8_t*>(var);
    for (auto _ : state)
        benchmark::DoNotOptimize(String(str));
}
BENCHMARK(BM_LargeStringCreation)->Iterations(ITERATION_TIMES);

static void BM_LargeStdStringCreation(benchmark::State& state)
{
    std::any var("large string over 15 length");
    auto str = std::any_cast<const char*>(var);
    for (auto _ : state)
        benchmark::DoNotOptimize(std::string(str));
}
BENCHMARK(BM_LargeStdStringCreation)->Iterations(ITERATION_TIMES);

BENCHMARK_MAIN();