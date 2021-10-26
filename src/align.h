#pragma once

#include "floatimage.h"
#include <iostream>
#include <math.h>

FloatImage sequenceMean(const std::vector<FloatImage> &imgs);
FloatImage sequenceMedian(const std::vector<FloatImage> &imgs);
FloatImage logSNR(const std::vector<FloatImage> &imSeq, float scale=1.0/20.0);
std::vector<int> align(const FloatImage &im1, const FloatImage &im2, int maxOffset=20);
FloatImage alignAndDenoise(const std::vector<FloatImage> &imSeq, int maxOffset=20);
FloatImage split(const FloatImage &sergeyImg);
FloatImage sergeyRGB(const FloatImage &sergeyImg, int maxOffset=20);

FloatImage roll(const FloatImage &im, int xRoll, int yRoll);