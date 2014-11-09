#ifndef NOISE_H
#define NOISE_H

#include <vector>
#include <memory>

#include "octave.h"

class SimplexNoise {
public:
    SimplexNoise(int largestFeature, double persistence, int seed);
    double getNoise(int x, int y);
    double getNoise(int x, int y, int z);
    
private:
    std::vector<std::unique_ptr<SimplexNoise_octave>> m_octaves;
    std::vector<double> m_frequencys;
    std::vector<double> m_amplitudes;

    int m_largestFeature;
    double m_persistence;
    int m_seed;
};

#endif
