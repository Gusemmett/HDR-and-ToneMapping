// hdr.cpp
// Assignment 5


#include "hdr.h"
#include "filtering.h"
#include "a2.h"
#include "utils.h"
#include <math.h>
#include <algorithm>

using namespace std;

/**************************************************************
 //                       HDR MERGING                        //
 *************************************************************/

// Generate a weight image that indicates which pixels are good to use in hdr
FloatImage computeWeight(const FloatImage &im, float epsilonMini, float epsilonMaxi)
{
	// should return an image with pixel values = 1 if im(...) falls
	// in the range [epsilonMini, epsilonMaxi]
	FloatImage out(im);

	for (int x = 0; x < im.sizeX(); x++){
		for (int y = 0; y < im.sizeY(); y++){
			for (int z = 0; z < im.sizeZ(); z++){
				out(x,y,z) = (im(x,y,z) >= epsilonMini && im(x,y,z) <= epsilonMaxi) ? 1.0f : 0.0f;
			}
		}
	}
	return out;
}

// Compute the multiplication factor between a pair of images
float computeFactor(const FloatImage &im1, const FloatImage &w1, const FloatImage &im2, const FloatImage &w2)
{
	vector<float> ratio;
	// populate list with ratios of im1 and im2 for all valid pixels
	for (int x = 0; x < im2.sizeX(); x++){
		for (int y = 0; y < im2.sizeY(); y++){
			for (int z = 0; z < im2.sizeZ(); z++){
				if (w1(x,y,z) == 1 && w2(x,y,z) == 1){
					float tempRatio = im2(x,y,z) / (im1(x,y,z) + 0.000000001);
					ratio.push_back(tempRatio);
				}
			}
		}
	}

	sort(ratio.begin(), ratio.end());

	cout << ratio[ceil(ratio.size() / 2)] << endl;

	if (ratio.size() % 2 == 0){
		return (ratio[ceil(ratio.size() / 2)] + ratio[floor(ratio.size() / 2)]) / 2; 
	}
	else { 
		return ratio[ceil(ratio.size() / 2)];
	}
}

// Merge images to make a single hdr image
FloatImage makeHDR(vector<FloatImage> &imSeq, float epsilonMini, float epsilonMaxi)
{
	// invert gamma correction
	for (int i = 0; i < ((int) imSeq.size()); i++){
		imSeq[i] = changeGamma(imSeq[i], 1.0 / 2.2, 1.0f);
	}

	vector<FloatImage> weights{};
	weights.push_back(computeWeight(imSeq[0], epsilonMini, 1.0f));
	for (int i = 1; i < ((int) imSeq.size() - 1); i++){
		weights.push_back(computeWeight(imSeq[i], epsilonMini, epsilonMaxi));
		cout << weights[i](100,100,0) << endl;
	}
	weights.push_back(computeWeight(imSeq[imSeq.size() - 1], 0.0f, epsilonMaxi));

	vector<float> factors{1};
	for (int i = 1; i < ((int) imSeq.size()); i++){
		factors.push_back(computeFactor(imSeq[i - 1], weights[i - 1], imSeq[i], weights[i]));
	}


	// float numerator, denominator, scale;
	FloatImage out(imSeq[0]);
	for (int j = 0; j < imSeq[0].size(); j++){
		float numerator = 0;
		float denominator = 0;
		float scale = 1;
		for (int i = 0; i < ((int) imSeq.size()); i++){
			scale *= factors[i];
			numerator += weights[i](j) * (1/scale) * imSeq[i](j);
			denominator += weights[i](j);
		}
		if (denominator == 0){
			out(j) = imSeq[0](j);
		} else{
			out(j) = numerator / denominator;
		} 	
	}

	return out;
}

/**************************************************************
 //                      TONE MAPPING                        //
 *************************************************************/


// Tone map an hdr image
FloatImage toneMap(const FloatImage &im, float targetBase, float detailAmp, bool useBila, float sigmaRange)
{
	
	vector<FloatImage> LC = lumiChromi(im);
	FloatImage lumiLog = log10FloatImage(LC[0]);

	float stds = (im.sizeX() > im.sizeY()) ? (im.sizeX() / 50) : (im.sizeY() / 50);

	FloatImage lumiBase;
	if (useBila){
		lumiBase = bilateral(lumiLog,sigmaRange,stds);
	}else{
	 	lumiBase = gaussianBlur_seperable(lumiLog,stds);
	}
	
	FloatImage logDetail = lumiLog - lumiBase;

	float k = (log10(targetBase) / (lumiBase.max() - lumiBase.min()));
	FloatImage out = (detailAmp*logDetail) + k * (lumiBase - lumiBase.max());

	out = exp10FloatImage(out);
	out = lumiChromi2rgb(out,LC[1]);

	return changeGamma(out, 1.0f, 1/2.2);
}

// Tone Mapping Helper Functions

// image --> log10FloatImage
FloatImage log10FloatImage(const FloatImage &im)
{
	float minnon0 = image_minnonzero(im);
	FloatImage out(im);

	for (int i = 0; i < im.size(); i++){
		if (im(i) == 0){
			out(i) = log10(im(i)) + minnon0;
		}
		else {
			out(i) = log10(im(i));
		}	
	}

	return out; 
}

// FloatImage --> 10^FloatImage
FloatImage exp10FloatImage(const FloatImage &im)
{
	FloatImage out(im);
	for (int i = 0; i < im.size(); i++){
		out(i) = pow(10,im(i));
	}
	return out;
}

// min non-zero pixel value of image
float image_minnonzero(const FloatImage &im)
{
	float out = INFINITY;
	for (int i = 0; i < im.size(); i++){
		if (im(i) != 0 && im(i) < out){
			out = im(i);	
		}
	}
	return out;
}
