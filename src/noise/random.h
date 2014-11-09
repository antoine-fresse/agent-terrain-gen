#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <chrono>

class Rand_Int {
    public:
    Rand_Int(int low, int hight) : re(std::chrono::system_clock::now().time_since_epoch().count()), dist{low, hight} {

    }
    
    Rand_Int(long seed, int low, int hight) : re(seed), dist{low, hight} {

    }

    int operator()() {
        return dist(re);
    }

    private:
    std::default_random_engine re;
    std::uniform_int_distribution<> dist;
};

class Rand_Float {
    public:
    Rand_Float(float low, float hight) : re(std::chrono::system_clock::now().time_since_epoch().count()), dist{low, hight} {

    }
    
    Rand_Float(long seed, float low, float hight) : re(seed), dist{low, hight} {

    }

    float operator()() {
        return dist(re);
    }

    private:
    std::default_random_engine re;
    std::uniform_real_distribution<> dist;
};


#endif // RANDOM_HPP

