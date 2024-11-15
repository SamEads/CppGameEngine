#pragma once

#include <cstdint>

class Random
{
public:
    Random();
    Random(long long seed);

    void setSeed(uint64_t seed);

    int32_t nextInt();
    int32_t nextInt(uint32_t bound);
    bool nextBool();
    float nextFloat();
    double nextDouble();
    unsigned long long nextLong();
private:
    uint64_t m_seed = 0;
    int next(int bits);
};