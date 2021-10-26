#include "align.h"
#include "utils.h"
#include <math.h>

using namespace std;

// Basic denoising by computing the average of a sequence of images
FloatImage sequenceMean(const vector<FloatImage> &imSeq)
{
	FloatImage output(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
	for (const auto &im : imSeq)
		output = output + im;

	output = output / imSeq.size();
	return output;
}

// Grad students only:
// Compute the per-pixel median of a sequence of images
FloatImage sequenceMedian(const vector<FloatImage> &imSeq)
{
	FloatImage output(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());

	for (auto c : range(output.channels()))
	{
		for (auto y : range(output.height()))
		{
			for (auto x : range(output.width()))
			{
				vector<float> pixels(imSeq.size(), 0.0f);
				for (auto i : range(imSeq.size()))
					pixels[i] = imSeq[i](x, y, c);

				std::nth_element(pixels.begin(), pixels.begin() + pixels.size() / 2, pixels.end());
				output(x, y, c) = pixels[pixels.size() / 2];
			}
		}
	}

	// output = output / imSeq.size();
	return output;
}

// returns an image visualizing the per-pixel and
// per-channel log of the signal-to-noise ratio scaled by scale.
FloatImage logSNR(const vector<FloatImage> &imSeq, float scale)
{
	FloatImage mean = sequenceMean(imSeq);

	// inplace/incremental variance computation
	FloatImage variance(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
	for (const auto &im : imSeq)
	{
		FloatImage diff = im - mean;
		variance = variance + diff * diff;
	}
	// size-1 is the Bessel correction
	variance = variance / (imSeq.size() - 1.0f);

	FloatImage logSNR(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
	for (auto c : range(logSNR.channels()))
		for (auto y : range(logSNR.height()))
			for (auto x : range(logSNR.width()))
				logSNR(x, y, c) = 20.0f * scale * log10(mean(x, y, c) / sqrt(variance(x, y, c)));

	return logSNR;

	//
	// alternate solution using explicit variance calculation:
	//

	// get the expected value of the images (mean image)
	FloatImage EIm = sequenceMean(imSeq);

	// get the expected value of the square of the images
	vector<FloatImage> imSeq2;
	for (int i = 0; i < imSeq.size(); i++)
		imSeq2.push_back(imSeq[i] * imSeq[i]);
	FloatImage EIm2 = sequenceMean(imSeq2);

	// calculate the variance
	FloatImage VarIm = (EIm2 - EIm * EIm);
	VarIm *= float(imSeq.size()) / float(imSeq.size() - 1.0); // Bessel Correction

	// calculate the snr from the variance and EIm2
	// VarIm += 0.000001; //add a small offset: TODO MAYBE MAKE THIS MORE NORMALIZED NEXT TIME
	FloatImage SNRIm = EIm2 / VarIm;

	for (int x = 0; x < SNRIm.width(); x++)
	{
		for (int y = 0; y < SNRIm.height(); y++)
		{
			for (int z = 0; z < SNRIm.channels(); z++)
			{
				SNRIm(x, y, z) = scale * 10.0 * log10(SNRIm(x, y, z));
			}
		}
	}

	return SNRIm;
}

// returns the (x,y) offset that best aligns im2 to match im1.
vector<int> align(const FloatImage &im1, const FloatImage &im2, int maxOffset)
{
	vector<int> offset = {0, 0};
	float bestError = 1e12f;

	for (int y = -maxOffset; y <= maxOffset; ++y)
		for (int x = -maxOffset; x <= maxOffset; ++x)
		{
			// cout << "offset: (" << x << ", " << y << ")" << endl;
			FloatImage diff = im1 - roll(im2, x, y);
			diff = diff * diff;

			float error = 0.0f;
			for (int y2 = maxOffset; y2 < im1.height() - maxOffset; ++y2)
				for (int x2 = maxOffset; x2 < im1.width() - maxOffset; ++x2)
					for (int z = 0; z < im1.channels(); ++z)
						error += diff(x2, y2, z);

			if (error < bestError)
			{
				offset[0] = x;
				offset[1] = y;
				bestError = error;
			}
		}

	return offset;
}

// registers all images to the first one in a sequence and outputs
// a denoised image even when the input sequence is not perfectly aligned.
FloatImage alignAndDenoise(const vector<FloatImage> &imSeq, int maxOffset)
{
	FloatImage output = imSeq[0];
	for (auto i : range<unsigned long long>(1ull, imSeq.size()))
	{
		// cout << "computing offset for filename: " << imSeq[i].name() << endl;
		vector<int> offset = align(imSeq[0], imSeq[i], maxOffset);
		cout << "optimal offset: " << offset[0] << ", " << offset[1] << endl;
		cout << "shifting and accumulating into average" << endl;
		output = output + roll(imSeq[i], offset[0], offset[1]);
	}

	output = output / imSeq.size();

	return output;
}

// split a Sergey images to turn it into one 3-channel image.
FloatImage split(const FloatImage &sergeyImg)
{
	int height = floor(sergeyImg.height() / 3);
	FloatImage output(sergeyImg.width(), height, 3);

	for (int y = 0; y < output.height(); ++y)
		for (int x = 0; x < output.width(); ++x)
		{
			output(x, y, 2) = sergeyImg(x, y, 0);
			output(x, y, 1) = sergeyImg(x, y + height, 0);
			output(x, y, 0) = sergeyImg(x, y + 2 * height, 0);
		}

	return output;
}

// aligns the green and blue channels of your rgb channel of a sergey
// image to the red channel. This should return an aligned RGB image
FloatImage sergeyRGB(const FloatImage &sergeyImg, int maxOffset)
{
	FloatImage rgb = split(sergeyImg);
	FloatImage r(sergeyImg.width(), sergeyImg.height(), 1),
		g(sergeyImg.width(), sergeyImg.height(), 1),
		b(sergeyImg.width(), sergeyImg.height(), 1);

	for (int y = 0; y < rgb.height(); ++y)
		for (int x = 0; x < rgb.width(); ++x)
		{
			r(x, y) = rgb(x, y, 0);
			g(x, y) = rgb(x, y, 1);
			b(x, y) = rgb(x, y, 2);
		}

	vector<int> rgoffset = align(r, g, 15);
	g = roll(g, rgoffset[0], rgoffset[1]);
	vector<int> rboffset = align(r, b, 15);
	b = roll(b, rboffset[0], rboffset[1]);

	for (int y = 0; y < rgb.height(); ++y)
		for (int x = 0; x < rgb.width(); ++x)
		{
			rgb(x, y, 0) = r(x, y);
			rgb(x, y, 1) = g(x, y);
			rgb(x, y, 2) = b(x, y);
		}

	// rgb = grayworld(rgb);

	return rgb;
}


/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// This circularly shifts an image by xRoll in the x direction and
// yRoll in the y direction. xRoll and yRoll can be negative or postive
FloatImage roll(const FloatImage &im, int xRoll, int yRoll)
{
	int xNew, yNew;
	FloatImage imRoll(im.width(), im.height(), im.channels());

	// for each pixel in the original image find where its corresponding
	// location is in the rolled image
	for (int x = 0; x < im.width(); x++)
	{
		for (int y = 0; y < im.height(); y++)
		{
			// use modulo to figure out where the new location is in the
			// rolled image. Then take care of when this returns a negative number
			xNew = (x + xRoll) % im.width();
			yNew = (y + yRoll) % im.height();
			xNew = (xNew < 0) * (imRoll.width() + xNew) + (xNew >= 0) * xNew;
			yNew = (yNew < 0) * (imRoll.height() + yNew) + (yNew >= 0) * yNew;

			// assign the rgb values for each pixel in the original image to
			// the location in the new image
			for (int z = 0; z < im.channels(); z++)
				imRoll(xNew, yNew, z) = im(x, y, z);
		}
	}

	// return the rolled image
	return imRoll;
}
