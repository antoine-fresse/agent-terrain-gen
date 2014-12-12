#include "noise.h"

#include "octave.h"
#include "random.h"

#include <climits>

SimplexNoise::SimplexNoise(int largestFeature,double persistence, int seed) {
    m_largestFeature = largestFeature;
    m_persistence = persistence;
    m_seed =  seed;

    //recieves a number (eg 128) and calculates what power of 2 it is (eg 2^7)
    int numberOfOctaves = (int)ceil(log(largestFeature) / log(2));

    Rand_Int rnd(seed, INT_MIN, INT_MAX);

    for (int i = 0; i < numberOfOctaves; ++i) {
        m_octaves.push_back(std::make_unique<SimplexNoise_octave>(rnd()));

        m_frequencys.push_back(pow(2, i));
        m_amplitudes.push_back(pow(m_persistence, m_octaves.size() - i));
    }
}


double SimplexNoise::getNoise(int x, int y) {
    double result = 0;
    for (unsigned int i = 0; i < m_octaves.size(); ++i) {
        //double frequency = Math.pow(2,i);
        //double amplitude = Math.pow(persistence,octaves.length-i);

        result = result + m_octaves[i]->noise(x / m_frequencys[i], y / m_frequencys[i]) * m_amplitudes[i];
    }
    return result;
}

double SimplexNoise::getNoise(int x,int y, int z) {
    double result = 0;
    for (unsigned int i = 0; i < m_octaves.size(); ++i) {
      double frequency = pow(2, i);
      double amplitude = pow(m_persistence, m_octaves.size() - i);

      result = result + m_octaves[i]->noise(x / frequency, y / frequency, z / frequency) * amplitude;
    }


    return result;
}
