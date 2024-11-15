#include "random.h"
#include <cmath>

constexpr auto RANDOM_MULTIPLIER = 0x5DEECE66DULL;
constexpr auto RANDOM_ADDEND = 0xBULL;
constexpr auto RANDOM_MASK = ((1ULL << 48u) - 1);
constexpr auto DOUBLE_UNIT = 0x1.0p-53;

Random::Random()
{
    m_seed = rand();
}

Random::Random(long long seed)
{
    setSeed(seed);
}

int32_t Random::next(int bits)
{
    m_seed = (m_seed * RANDOM_MULTIPLIER + RANDOM_ADDEND) & RANDOM_MASK;
    return static_cast<int32_t>(m_seed >> (48u - bits));
}

int32_t Random::nextInt()
{
    return next(32);
}

int Random::nextInt(uint32_t bound)
{
    int32_t r = next(31);
    const uint16_t m = bound - 1u;
    if ((bound & m) == 0)
    {
        r = (int32_t)((bound * (uint64_t)r) >> 31u);
    }
    else for (int32_t u = r; u - (r = u % bound) + m < 0; u = next(31));

    return r;
}

bool Random::nextBool()
{
    return (nextInt(2) == 0);
}

float Random::nextFloat()
{
    return static_cast<float>(nextDouble());
}

double Random::nextDouble()
{
    return static_cast<double>((((uint64_t)((uint32_t)next(26)) << 27)) + next(27)) * DOUBLE_UNIT;
}

void Random::setSeed(uint64_t seed)
{
    m_seed = (seed ^ RANDOM_MULTIPLIER) & RANDOM_MASK;
}

unsigned long long Random::nextLong()
{
    return (static_cast<unsigned long long>(next(32)) << 32u) + next(32);
}