/*
 * A speed-improved simplex noise algorithm for 2D, 3D and 4D in Java.
 *
 * Based on example code by Stefan Gustavson (stegu@itn.liu.se).
 * Optimisations by Peter Eastman (peastman@drizzle.stanford.edu).
 * Better rank ordering method by Stefan Gustavson in 2012.
 *
 * This could be speeded up even further, but it's useful as it is.
 *
 * Version 2012-03-09
 *
 * This code was placed in the public domain by its original author,
 * Stefan Gustavson. You may use it as you see fit, but
 * attribution is appreciated.
 *
 */
#ifndef OCTAVE_H
#define OCTAVE_H

#include <cmath>

class Grad
{
public:
    double x, y, z, w;

    Grad(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Grad(double x, double y, double z, double w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
};

class SimplexNoise_octave {  // Simplex noise in 2D, 3D and 4D
public:
    const int RANDOMSEED=0;
    const int NUMBEROFSWAPS=400;  

    SimplexNoise_octave(int seed);

    // This method is a *lot* faster than using (int)Math.floor(x)
    static int fastfloor(double x);
    static double dot(Grad g, double x, double y);
    static double dot(Grad g, double x, double y, double z);
    static double dot(Grad g, double x, double y, double z, double w);

    // 2D simplex noise
    double noise(double xin, double yin);

    // 3D simplex noise
    double noise(double xin, double yin, double zin);

    // 4D simplex noise, better simplex rank ordering method 2012-03-09
    double noise(double x, double y, double z, double w);

private:
    static const Grad grad3[12];
    static const Grad grad4[32];
    static const short p_supply[256]; //this contains all the numbers between 0 and 255, these are put in a random order depending upon the seed

    short p[256];

    // To remove the need for index wrapping, double the permutation table length
    short perm[512];
    short permMod12[512];
    
    // Skewing and unskewing factors for 2, 3, and 4 dimensions
    const double F2 = 0.5*(sqrt(3.0)-1.0);
    const double G2 = (3.0-sqrt(3.0))/6.0;
    const double F3 = 1.0/3.0;
    const double G3 = 1.0/6.0;
    const double F4 = (sqrt(5.0)-1.0)/4.0;
    const double G4 = (5.0-sqrt(5.0))/20.0;

};

#endif
