# HDR and Tone Mapping C++ implementation
This repository acomplishes two goals. 
* The first being to take a list of images each with differing exposure and produce an HDR image, with basic thresholding. The steps to acomplish this are:
	1. Compute the weights for each pixel accross all images
	2. Calculate the multiplication factor between the images
	3. Merge the images according to their facotors


* The second part uses this HDR image and applies tone mapping. The steps for this are: 
	1. Decomose the image into luminance and chrominance and compute the log version of the luminance
	2. With either a Gaussian blur or Bilateral filter and use a standard deviation for the spatial Gausian equal to biggest dimension of the image divided by 50.
	3. Compute detail by taking difference from original log luminance
	4. Calculate scale factor for the log domain and apply to the logged image
	5. Convert the image back to the linear domain
	6. Reintegrate the luminance and chrominace to produce the tone mapped image.
	






### Credits
The basecode is written by Wojciech Jarosz, but it is heavily derived from (with permission):

:information_source: MIT's 6.815/6.865 basecode, written and designed by:
* Frédo Durand
* Katherine L. Bouman
* Gaurav Chaurasia
* Adrian Vasile Dalca
* Neal Wadhwa
