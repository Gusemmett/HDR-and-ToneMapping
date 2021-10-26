#include "hdr.h"
#include "a2.h"
#include "align.h"
#include <sstream>

using namespace std;

// test the compute factor and compute weight function
void testComputeFactorAndWeight()
{
	// load in 2 images
	FloatImage im1(DATA_DIR "/input/a5/ante2-1.png");
	FloatImage im2(DATA_DIR "/input/a5/ante2-2.png");

	// invert gamma correction
	im1 = changeGamma(im1, 1.0 / 2.2, 1.0f);
	im2 = changeGamma(im2, 1.0 / 2.2, 1.0f);

	// compute weight images and save them
	FloatImage w1 = computeWeight(im1);
	FloatImage w2 = computeWeight(im2);
	w1.write(DATA_DIR "/output/ante2-w1.png");
	w2.write(DATA_DIR "/output/ante2-w2.png");

	// compute the factor
	float factor = computeFactor(im1, w1, im2, w2);
	cout << "factor: " << factor << endl;

}

// test the hdr merging function
void testMakeHDR()
{
	// load an image sequence
	vector<FloatImage> imSeq;
	imSeq.push_back(FloatImage(DATA_DIR "/input/a5/design-1.png"));
	imSeq.push_back(FloatImage(DATA_DIR "/input/a5/design-2.png"));
	imSeq.push_back(FloatImage(DATA_DIR "/input/a5/design-3.png"));
	imSeq.push_back(FloatImage(DATA_DIR "/input/a5/design-4.png"));
	imSeq.push_back(FloatImage(DATA_DIR "/input/a5/design-5.png"));
	imSeq.push_back(FloatImage(DATA_DIR "/input/a5/design-6.png"));
	imSeq.push_back(FloatImage(DATA_DIR "/input/a5/design-7.png"));

	// generate an hdr image
	FloatImage hdr = makeHDR(imSeq);

	// save out HDR image
	hdr.write(DATA_DIR "/output/design-out.hdr");

	// save out PNG images clipped to different ranges.
	float maxVal = hdr.max();
	FloatImage hdrScale0 = hdr / maxVal;

	changeGamma((1e0) * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR_design-0.png");
	changeGamma((1e2) * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR_design-1.png");
	changeGamma((1e4) * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR_design-2.png");
	changeGamma((1e6) * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR_design-3.png");
	changeGamma((1e8) * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR_design-4.png");
	changeGamma((1e10) * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR_design-5.png");
}

// HDR and Tone Mapping on Ante2 images
void testMakeAndToneMap_ante2()
{
	// load images
	vector<FloatImage> imSeq;
	imSeq.push_back(FloatImage(DATA_DIR "/input/a5/ante2-1.png"));
	imSeq.push_back(FloatImage(DATA_DIR "/input/a5/ante2-2.png"));

	// create hdr image
	FloatImage hdr = makeHDR(imSeq);

	// save out HDR image
	hdr.write(DATA_DIR "/output/ante2-out.hdr");

	// tone map with gaussian blur
	toneMap(hdr, 100, 1, false).write(DATA_DIR "/output/ante2-tonedHDRsimple-gauss.png");
	// tone map with bilaterial
	toneMap(hdr, 100, 3, true, 0.1).write(DATA_DIR "/output/ante2-tonedHDRsimple-bilateral.png");
}

// HDR and Tone Mapping on turkey images
void testToneMapping_turkey()
{
	FloatImage hdr(DATA_DIR "/input/a5/turkey.hdr");

	// save out PNG images clipped to different ranges.
	float maxVal = hdr.max();
	FloatImage hdrScale0 = hdr / maxVal;
	changeGamma(1.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-turkey-0.png");
	changeGamma(2.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-turkey-1.png");
	changeGamma(4.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-turkey-2.png");
	changeGamma(8.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-turkey-3.png");
	changeGamma(16.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-turkey-4.png");

	// tone map with gaussian blur
	toneMap(hdr, 20, 1.5).write(DATA_DIR "/output/turkey-tonedHDRsimple-gauss.png");
	toneMap(hdr, 100, 3).write(DATA_DIR "/output/turkey-tonedHDRsimple-gauss2.png");

	// tone map with bilaterial
	toneMap(hdr, 20, 1.5, true, 0.2).write(DATA_DIR "/output/turkey-tonedHDRsimple-bilateral.png");
	toneMap(hdr, 100, 3, true, 0.2).write(DATA_DIR "/output/turkey-tonedHDRsimple-bilateral2.png");
}

void testMakeAndToneMap_turkey()
{
	vector<string> filenames;
	int nImages = 6;

	// load an image sequence
	vector<FloatImage> imSeq;
	for (int i = 0; i < nImages; i++)
	{
		ostringstream ss;
		ss << DATA_DIR "/input/a5/turkey-" << i << ".png";
		string filename = ss.str();
		imSeq.push_back(FloatImage(filename));
	}

	// generate an hdr image
	FloatImage hdr(DATA_DIR "/input/a5/turkey.hdr");

	// save out HDR image
	hdr.write(DATA_DIR "/output/turkey-out.hdr");

	// save out PNG images clipped to different ranges.
	float maxVal = hdr.max();
	FloatImage hdrScale0 = hdr / maxVal;
	changeGamma(1.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-turkey-0.png");
	changeGamma(2.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-turkey-1.png");
	changeGamma(4.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-turkey-2.png");
	changeGamma(8.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-turkey-3.png");
	changeGamma(16.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-turkey-4.png");

	// tone map with gaussian blur
	toneMap(hdr, 20, 1.5).write(DATA_DIR "/output/full-turkey-tonedHDRsimple-gauss.png");
	toneMap(hdr, 100, 3).write(DATA_DIR "/output/full-turkey-tonedHDRsimple-gauss2.png");
	// tone map with bilaterial
	toneMap(hdr, 20, 1.5, true, 0.2).write(DATA_DIR "/output/full-turkey-tonedHDRsimple-bilateral.png");
	toneMap(hdr, 100, 3, true, 0.2).write(DATA_DIR "/output/full-turkey-tonedHDRsimple-bilateral2.png");
}

// HDR and Tone Mapping on memorial images
void testToneMapping_memorial()
{
	FloatImage hdr(DATA_DIR "/input/a5/memorial.hdr");

	// save out PNG images clipped to different ranges.
	float maxVal = hdr.max();
	FloatImage hdrScale0 = hdr / maxVal;
	changeGamma(1.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-memorial-0.png");
	changeGamma(4.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-memorial-1.png");
	changeGamma(16.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-memorial-2.png");
	changeGamma(64.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-memorial-3.png");
	changeGamma(256.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-memorial-4.png");
	changeGamma(1024.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/scaledHDR-memorial-5.png");

	// tone map with gaussian blur
	toneMap(hdr, 20, 1.5).write(DATA_DIR "/output/memorial-tonedHDRsimple-gauss.png");
	toneMap(hdr, 100, 1.5).write(DATA_DIR "/output/memorial-tonedHDRsimple-gauss2.png");
	// tone map with bilaterial
	toneMap(hdr, 20, 1.5, true, 0.2).write(DATA_DIR "/output/memorial-tonedHDRsimple-bilateral.png");
	toneMap(hdr, 100, 3, true, 0.2).write(DATA_DIR "/output/memorial-tonedHDRsimple-bilateral2.png");
}

void testMakeAndToneMap_memorial() {
	vector<string> filenames;
	int nImages = 10;

	// load an image sequence
	vector<FloatImage> imSeq;
	for (int i = 0; i < nImages; i++)
	{
		ostringstream ss;
		ss << DATA_DIR "/input/a5/memorial-" << i << ".png";
		string filename = ss.str();
		imSeq.push_back(FloatImage(filename));
	}

	// generate an hdr image
	FloatImage hdr = makeHDR(imSeq, 0.1, 0.9f);

	// save out HDR image
	hdr.write(DATA_DIR "/output/memorial-out.hdr");

	// save out PNG images clipped to different ranges.
	float maxVal = hdr.max();
	FloatImage hdrScale0 = hdr / maxVal;
	changeGamma(1.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-memorial-0.png");
	changeGamma(4.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-memorial-1.png");
	changeGamma(16.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-memorial-2.png");
	changeGamma(64.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-memorial-3.png");
	changeGamma(256.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-memorial-4.png");
	changeGamma(1024.f * hdrScale0, 1.0f, 1.f/2.2f).write(DATA_DIR "/output/full-scaledHDR-memorial-5.png");

	// tone map with gaussian blur
	toneMap(hdr, 20, 1.5).write(DATA_DIR "/output/full-memorial-tonedHDRsimple-gauss.png");
	toneMap(hdr, 100, 1.5).write(DATA_DIR "/output/full-memorial-tonedHDRsimple-gauss2.png");
	// tone map with bilaterial
	toneMap(hdr, 20, 1.5, true, 0.2).write(DATA_DIR "/output/full-memorial-tonedHDRsimple-bilateral.png");
	toneMap(hdr, 100, 3, true, 0.2).write(DATA_DIR "/output/full-memorial-tonedHDRsimple-bilateral2.png");
}

void testMakeAndToneMap_sign() {
	vector<string> filenames;
	int nImages = 5;

	// load an image sequence
	vector<FloatImage> imSeq;
	for (int i = 0; i < nImages; i++)
	{
		ostringstream ss;
		ss << DATA_DIR "/input/a5/sign" << i << ".jpg";
		string filename = ss.str();
		imSeq.push_back(FloatImage(filename));
	}

	// generate an hdr image
	FloatImage hdr = makeHDR(imSeq, 0.1, 0.9f);

	// save out HDR image
	hdr.write(DATA_DIR "/output/sign-out.hdr");


	// tone map with gaussian blur
	toneMap(hdr, 20, 1.5).write(DATA_DIR "/output/full-sign-tonedHDRsimple-gauss.png");
	toneMap(hdr, 100, 1.5).write(DATA_DIR "/output/full-sign-tonedHDRsimple-gauss2.png");

	// tone map with bilaterial
	// toneMap(hdr, 20, 1.5, true, 0.2).write(DATA_DIR "/output/full-sign-tonedHDRsimple-bilateral.png");
	// toneMap(hdr, 100, 3, true, 0.2).write(DATA_DIR "/output/full-sign-tonedHDRsimple-bilateral2.png");
}

void alignMakeAndTone_sign() {
	vector<string> filenames;
	int nImages = 5;

	// load an image sequence
	vector<FloatImage> imSeq;
	for (int i = 0; i < nImages; i++)
	{
		ostringstream ss;
		ss << DATA_DIR "/input/a5/sign" << i << ".jpg";
		string filename = ss.str();
		imSeq.push_back(FloatImage(filename));
	}

	vector<vector<FloatImage>> imSeqLC{};
	for (int i = 0; i < (int)imSeq.size(); i++){
		imSeqLC.push_back(lumiChromi(imSeq[i]));
	} 

	// Align images with luminance
	for (int i = 0; i < (int)imSeq.size(); i++){
		vector<int> offset = align(imSeqLC[0][1], imSeqLC[i][1], 10);
		cout << "aligned photo: " << i << " " << offset[0] << " " << offset[1] << endl;
		imSeq[i] = roll(imSeq[i], offset[0], offset[1]);
	}

	// generate an hdr image
	FloatImage hdr = makeHDR(imSeq, 0.1, 0.9f);

	// save out HDR image
	hdr.write(DATA_DIR "/output/align-sign-out.hdr");

	// tone map with gaussian blur
	toneMap(hdr, 20, 1.5).write(DATA_DIR "/output/full-align-tonedHDRsimple-gauss.png");
	toneMap(hdr, 100, 1.5).write(DATA_DIR "/output/full-align-tonedHDRsimple-gauss2.png");

	// tone map with bilaterial
	// toneMap(hdr, 20, 1.5, true, 0.2).write(DATA_DIR "/output/full-align-sign-tonedHDRsimple-bilateral.png");
	// toneMap(hdr, 100, 3, true, 0.2).write(DATA_DIR "/output/full-align-sign-tonedHDRsimple-bilateral2.png");
}



// This is a way for you to test your functions.
// We will not grade the contents of the main function
int main()
{
	// uncomment these test functions as you complete the assignment

	// // create an hdr image
	// testComputeFactorAndWeight();
	// cout << "done fac w" << endl;
	// testMakeHDR();
	// cout << "done makeHDR" << endl;

	// // tone map an hdr image
	// testToneMapping_turkey();
	// cout << "done turkey" << endl;
	// testToneMapping_memorial();
	// cout << "done testToneMapping_memorial" << endl;

	// // generate hdr and tonemap
	// testMakeAndToneMap_ante2();
	// cout << "done testMakeAndToneMap_ante2" << endl;
	// testMakeAndToneMap_turkey();
	// cout << "done testMakeAndToneMap_turkey" << endl;
	// testMakeAndToneMap_memorial();

	// testMakeAndToneMap_sign();

	// alignMakeAndTone_sign();

	for (int i = 0; i < 1000000000; i++){
		cout << i << endl;
	}

}
