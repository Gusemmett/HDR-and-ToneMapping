#pragma once

// hdr.h
// Assignment 5

#include "floatimage.h"
#include <iostream>
#include <math.h>

// HDR
FloatImage computeWeight(const FloatImage &im, float epsilonMini=0.002, float epsilonMaxi=0.99);
float computeFactor(const FloatImage &im1, const FloatImage &w1, const FloatImage &im2, const FloatImage &w2);
FloatImage makeHDR(std::vector<FloatImage> &imSeq, float epsilonMini=0.002, float epsilonMaxi=0.99);

// Tone Mapping
FloatImage toneMap(const FloatImage &im, float targetBase=100, float detailAmp=3, bool useBila=false, float sigmaRange=0.1);
FloatImage exp10FloatImage(const FloatImage &im);
FloatImage log10FloatImage(const FloatImage &im);
float image_minnonzero(const FloatImage &im);