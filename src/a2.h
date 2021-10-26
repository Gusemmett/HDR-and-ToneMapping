#pragma once

#include "floatimage.h"

// Functions from previous assignment
FloatImage brightness(const FloatImage &im, float factor);
FloatImage contrast(const FloatImage &im, float factor, float midpoint = 0.5);

// Gamma and exposure
FloatImage changeGamma(const FloatImage & im, float old_gamma, float new_gamma);
FloatImage exposure(const FloatImage &im, float factor);


// Colorspace problems
static const float weight_init[3] = {0.299, 0.587, 0.114};
FloatImage color2gray(const FloatImage &im, const std::vector<float> &weights = std::vector<float>(weight_init, weight_init+3));
std::vector<FloatImage> lumiChromi(const FloatImage &im);
FloatImage lumiChromi2rgb(const FloatImage &lumi, const FloatImage &chromi);
FloatImage brightnessContrastLumi(const FloatImage &im, float brightF, float  contrastF, float midpoint = 0.3f);
FloatImage rgb2yuv(const FloatImage &im);
FloatImage yuv2rgb(const FloatImage &im);
FloatImage saturate(const FloatImage &im, float k);
std::vector<FloatImage> spanish(const FloatImage &im);

// White Balance
FloatImage grayworld(const FloatImage & in);


// Histogram manipulations

class Histogram
{
public:
	Histogram() {}
	Histogram(const FloatImage & im, int c = 0, int numBins = 256);

	int numBins() const {return m_pdf.size();}

	double pdf(int bin) const {return m_pdf[bin];}
	double cdf(int bin) const {return m_cdf[bin];}
	int inverseCDF(double value) const;

private:
	std::vector<double> m_pdf;
	std::vector<double> m_cdf;
};

void autoLevels(FloatImage & im, int channel);

FloatImage visualizeRGBHistogram(const Histogram & histR,
                                 const Histogram & histG,
                                 const Histogram & histB);

FloatImage equalizeRGBHistograms(const FloatImage &im);
FloatImage matchRGBHistograms(const FloatImage & F1, const FloatImage & F2);

