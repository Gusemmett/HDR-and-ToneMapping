/*!
    CS 73/273 Computational Aspects of Digital Photography C++ basecode.
*/

#include "a2.h"
#include <math.h>
#include "utils.h"
#include <assert.h>
#include <iostream>

using namespace std;


FloatImage brightness(const FloatImage &im, float factor)
{
	return im * factor;
}

FloatImage contrast(const FloatImage &im, float factor, float midpoint)
{
	return (im - midpoint) * factor + midpoint;
}

FloatImage changeGamma(const FloatImage &im, float old_gamma, float new_gamma)
{
	FloatImage output(im.width(), im.height(), im.channels());
	// Figure out what power to take the values of im, to get the values of output
	float power = new_gamma / old_gamma;
	for (int i = 0; i < output.size(); ++i)
		output(i) = pow(im(i), power);
	return output;
}

// Change the exposure of the image. This is different than brightness because
// it means you multiply an image's intensity in the linear domain.
FloatImage exposure(const FloatImage &im, float factor)
{
	FloatImage output(im.width(), im.height(), im.channels());
	output = changeGamma(im, 1.0f / 2.2f, 1.0f);
	output = brightness(output, factor);
	output = changeGamma(output, 1.0f, 1.0f / 2.2f);
	return output;
}

FloatImage color2gray(const FloatImage &im, const vector<float> &weights)
{
	if ((int) weights.size() != im.channels())
		throw MismatchedDimensionsException();

	FloatImage output(im.width(), im.height(), 1);
	// Convert to grayscale
	for (auto y : range(im.height()))
		for (auto x : range(im.width()))
		{
			float dot_product = 0.0f;
			for (int z = 0; z < im.channels(); ++z)
				dot_product += weights[z] * im(x, y, z);

			output(x, y, 0) = dot_product;
		}

	return output;
}

// For this function, we want two outputs, a single channel luminance image
// and a three channel chrominance image. Return them in a vector with luminance first
vector<FloatImage> lumiChromi(const FloatImage &im)
{

	FloatImage im_chrominance(im.width(), im.height(), im.channels());
	FloatImage im_luminance(im.width(), im.height(), 1);
	vector<FloatImage> output;

	// Create luminance and chrominance images
	im_luminance = color2gray(im);
	im_chrominance = im / (im_luminance + 1e-12f);

	// push the luminance and chrominance images onto the vector
	output.push_back(im_luminance);
	output.push_back(im_chrominance);
	return output;

}

// go from a luminance/chrominance images back to an rgb image
FloatImage lumiChromi2rgb(const FloatImage &lumi, const FloatImage &chromi)
{
	return chromi * lumi;
}

// Modify brightness then contrast
FloatImage brightnessContrastLumi(const FloatImage &im, float brightF, float contrastF, float midpoint)
{
	// Create output image of appropriate size
	FloatImage output(im.width(), im.height(), im.channels());

	// Modify brightness, then contrast of luminance image
	vector<FloatImage> lc = lumiChromi(im);

	lc[0] = brightness(lc[0], brightF);
	lc[0] = contrast(lc[0], contrastF, midpoint);

	output = lc[1] * lc[0];
	return output;
}

FloatImage rgb2yuv(const FloatImage &im)
{
	// Create output image of appropriate size
	FloatImage output(im.width(), im.height(), im.channels());

	// Change colorspace
	for (auto y : range(im.height()))
		for (auto x : range(im.width()))
		{
			output(x, y, 0) = 0.299f * im(x, y, 0) + 0.587f * im(x, y, 1) + 0.114f * im(x, y, 2);
			output(x, y, 1) = -0.147f * im(x, y, 0) - 0.289f * im(x, y, 1) + 0.436f * im(x, y, 2);
			output(x, y, 2) = 0.615f * im(x, y, 0) - 0.515f * im(x, y, 1) - 0.100f * im(x, y, 2);
		}

	return output;
}

FloatImage yuv2rgb(const FloatImage &im)
{
	// Create output image of appropriate size
	FloatImage output(im.width(), im.height(), im.channels());

	// Change colorspace
	for (auto y : range(im.height()))
		for (auto x : range(im.width()))
		{
			output(x, y, 0) = 1.0f * im(x, y, 0) + 0.000f * im(x, y, 1) + 1.140f * im(x, y, 2);
			output(x, y, 1) = 1.0f * im(x, y, 0) - 0.395f * im(x, y, 1) - 0.581f * im(x, y, 2);
			output(x, y, 2) = 1.0f * im(x, y, 0) + 2.032f * im(x, y, 1) + 0.000f * im(x, y, 2);
		}

	return output;
}

FloatImage saturate(const FloatImage &im, float factor)
{
	// Create output image of appropriate size
	FloatImage output = rgb2yuv(im);

	// Saturate image
	for (auto y : range(im.height()))
		for (auto x : range(im.width()))
		{
			output(x, y, 1) *= factor;
			output(x, y, 2) *= factor;
		}

	output = yuv2rgb(output);
	return output;
}

// Return two images in a C++ vector
vector<FloatImage> spanish(const FloatImage &im)
{
	// Remember to create the output images and the output vector
	FloatImage neg_chrom(im.width(), im.height(), im.channels());
	FloatImage lum(im.width(), im.height(), im.channels());

	// Do all the required processing
	FloatImage yuv = rgb2yuv(im);

	for (auto y : range(im.height()))
		for (auto x : range(im.width()))
		{
			lum(x, y, 0) = yuv(x, y, 0);
			lum(x, y, 1) = yuv(x, y, 0);
			lum(x, y, 2) = yuv(x, y, 0);

			neg_chrom(x, y, 0) = 0.5f;
			neg_chrom(x, y, 1) = -yuv(x, y, 1);
			neg_chrom(x, y, 2) = -yuv(x, y, 2);
		}

	neg_chrom = yuv2rgb(neg_chrom);

	int mw = floor(im.width() / 2);
	int mh = floor(im.height() / 2);
	lum(mw, mh, 0) = 0.0;
	lum(mw, mh, 1) = 0.0;
	lum(mw, mh, 2) = 0.0;

	neg_chrom(mw, mh, 0) = 0.0;
	neg_chrom(mw, mh, 1) = 0.0;
	neg_chrom(mw, mh, 2) = 0.0;

	// Push the images onto the vector
	vector<FloatImage> output;
	output.push_back(neg_chrom);
	output.push_back(lum);

	// Return the vector
	return output;
}

// White balances an image using the gray world assumption
FloatImage grayworld(const FloatImage &im)
{
	// Your code goes here
	float average[3] = {0.0f, 0.0f, 0.0f};
	float size = 1.0f / (im.width() * im.height());

	// compute average color
	for (auto y : range(im.height()))
		for (auto x : range(im.width()))
		{
			average[0] += im(x, y, 0) * size;
			average[1] += im(x, y, 1) * size;
			average[2] += im(x, y, 2) * size;
		}

	FloatImage output = im;
	for (auto y : range(im.height()))
		for (auto x : range(im.width()))
		{
			output(x, y, 0) *= average[1] / average[0];
			output(x, y, 1) *= 1.0f; // average[1] / average[1];
			output(x, y, 2) *= average[1] / average[2];
		}

	return output;
}


// Histograms

// Stretches the pixel values of channel c of im so that the minimum value maps to 0,
// and the maximum value maps to 1
void autoLevels(FloatImage &im, int c)
{
	// stretch pixel values to fill 0..1 range
	float mn = im.min(c);
	float mx = im.max(c);

	for (auto y : range(im.height()))
		for (auto x : range(im.width()))
			im(x, y, c) = (im(x, y, c) - mn) / (mx - mn);
}

// initialize a histogram with numBins bins from the c-th channel of im
Histogram::Histogram(const FloatImage &im, int c, int numBins) :
	m_pdf(numBins, 0.0f), m_cdf(numBins, 0.0f)
{
	// populate m_pdf with histogram values
	double d = 1.0 / (im.width() * im.height());
	for (auto y : range(im.height()))
		for (auto x : range(im.width()))
			m_pdf[clamp(int(floor(im(x, y, c) * (numBins - 1))), 0, numBins - 1)] += d;

	m_cdf[0] = m_pdf[0];
	// populate m_cdf with running sum
	for (auto i : range(m_cdf.size()))
		m_cdf[i] = m_cdf[i - 1] + m_pdf[i];
}

// return the histogram bin that value falls within
int Histogram::inverseCDF(double value) const
{
	// naive linear-time algorithm
	for (auto i : range(m_cdf.size()))
	{
		if (value <= m_cdf[i])
			return i;
	}

	return m_cdf.size() - 1;
	// return lower_bound(m_cdf.begin(), m_cdf.end(), value) - m_cdf.begin();
}

// Produce a numBins() x 100 x 3 image containing a visualization of the
// red, green and blue histogram pdfs
FloatImage visualizeRGBHistogram(const Histogram &histR,
                                 const Histogram &histG,
                                 const Histogram &histB)
{
	assert(histR.numBins() == histG.numBins() && histR.numBins() == histB.numBins());

	// create an image of appropriate size
	FloatImage im(histR.numBins(), 100, 3);

	// populate im with RGB histograms

	const Histogram *hists[] = {&histR, &histG, &histB};

	for (int c = 0; c < 3; ++c)
	{
		double maxPdfValue = hists[c]->pdf(0);
		for (int i = 1; i < hists[c]->numBins(); ++i)
			maxPdfValue = std::max(hists[c]->pdf(i), maxPdfValue);

		for (int bin = 0; bin < histR.numBins(); ++bin)
		{
			int maxY = int(round(float(hists[c]->pdf(bin)) / maxPdfValue * 100));
			for (int y = 0; y < maxY; ++y)
				im(bin, im.height() - 1 - y, c) = 1.0f;
		}
	}

	return im;
}

// Return a FloatImage which is the result of applying histogram equalization to im
FloatImage equalizeRGBHistograms(const FloatImage &im)
{
	// apply histogram equalization to each channel
	int numLevels = 256;
	FloatImage output(im.width(), im.height(), im.channels());
	for (int c = 0; c < 3; ++c)
	{
		Histogram hist(im, c, numLevels);

		for (int y = 0; y < output.height(); ++y)
			for (int x = 0; x < output.width(); ++x)
			{
				int r = floor(clamp(im(x, y, c), 0.0f, 1.0f) * (numLevels - 1));
				float cdfVal = hist.cdf(r);     // value between 0 and 1

				output(x, y, c) = cdfVal;
			}
	}
	return output;
}

// Return a FloatImage which is the result of transfering the histogram of F2 onto the image data in F1
FloatImage matchRGBHistograms(const FloatImage &F1, const FloatImage &F2)
{
	int numBins = 256;

	FloatImage output = F1;
	for (int c = 0; c < 3; ++c)
	{
		Histogram hist1(F1, c, numBins);
		Histogram hist2(F2, c, numBins);

		for (int y = 0; y < output.height(); ++y)
			for (int x = 0; x < output.width(); ++x)
			{
				int g = floor(clamp(F1(x, y, c), 0.0f, 1.0f) * (numBins - 1));
				double cdfVal = hist1.cdf(g); // value between 0 and 1
				int bin = hist2.inverseCDF(cdfVal);

				output(x, y, c) = float(bin) / (numBins - 1);
			}
	}
	return output;
}

