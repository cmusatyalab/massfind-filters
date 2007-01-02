#ifndef UPMC_FEATURES_H_
#define UPMC_FEATURES_H_

// description of UPMC features
#define NUM_UPMC_FEATURES 38

// global features

// average pixel value of whole breast area
#define UPMC_AVG_PIXEL_VALUE 0 
// peak value in histogram of breast area
#define UPMC_PEAK_PIXEL_VALUE 1	  
// pixel value corresponding to maximum value in histogram
#define UPMC_MAX_PIXEL_VALUE 2
// pixel value standard deviation of histogram
#define UPMC_SDEV_PIXEL_VALUE 3
// pixel value skewness of histogram
#define UPMC_SKEW_PIXEL_VALUE 4
// pixel value kurtosis of histogram
#define UPMC_KURTOSIS_PIXEL_VALUE 5
// average local noise (pixel value fluctuation) of breast area
#define UPMC_AVG_NOISE 6
// standard deviation of noise of breast area
#define UPMC_SDEV_NOISE 7
// skewness of noise of breast area 
#define UPMC_SKEW_NOISE 8
// type of digitizer
#define UPMC_DIGITIZER_TYPE 9

// ROI features

// region conspicuity
#define UPMC_REGION_CONSPICUITY 10
// region size
#define UPMC_REGION_SIZE 11
// region constrast
#define UPMC_REGION_CONTRAST 12
// average local noise (pixel value fluctuation)
#define UPMC_AVG_LOCAL_NOISE 13
// standard deviation of local noise
#define UPMC_SDEV_LOCAL_NOISE 14
// skewness of local noise
#define UPMC_SKEW_LOCAL_NOISE 15
// mean radial length/region size
#define UPMC_RADIAL_LENGTH_SIZE 16
// standard deviation of radial lengths
#define UPMC_SDEV_RADIAL_LENGTHS 17
// skewness of radial lengths
#define UPMC_SKEW_RADIAL_LENGTHS 18
// ratio between the maximum and minimal radial length
#define UPMC_MIN_MAX_RADIAL_LENGTH 19
// shape factor ratio (perimeter of boundary/region size)
#define UPMC_SHAPE_FACTOR_RATIO 20
// region circularity
#define UPMC_REGION_CIRCULARITY 21
// standard deviation of pixel values inside growth region
#define UPMC_SDEV_REGION_PIXEL_VALUE 22
// skewness of pixel values inside growth region
#define UPMC_SKEW_REGION_PIXEL_VALUE 23
// kurtosis of pixel values inside growth region
#define UPMC_KURTOSIS_REGION_PIXEL_VALUE 24
// average value of gradient of boundary contour pixels
#define UPMC_AVG_BOUNDARY_GRADIENT 25
// standard deviation of gradient of boundary contour pixels
#define UPMC_SDEV_BOUNDARY_GRADIENT 26
// skewness of gradient of boundary bontour pixels
#define UPMC_SKEW_BOUNDARY_GRADIENT 27
// standard deviation of pixel values in the surrounding background
#define UPMC_SDEV_BACKGROUND 28
// skewness of pixel values in surrounding background
#define UPMC_SKEW_BACKGROUND 29
// kurtosis of pixel values in surrounding background
#define UPMC_KURTOSIS_BACKGROUND 30
// average noise of pixel values in surrounding background
#define UPMC_AVG_NOISE_BACKGROUND 31
// standard deviation of pixel value noise in the surrounding background
#define UPMC_SDEV_NOISE_BACKGROUND 32
// skewness of pixel value noise in the surrounding background
#define UPMC_SKEW_NOISE_BACKGROUND 33
// ratio of number of "minimum" pixels inside growth region
#define UPMC_NUM_MIN_PIXEL_RATIO 34
// average pixel value depth of "minimum" pixels inside growth region
#define UPMC_AVG_MIN_PIXEL_DEPTH 35
// maximum pixel value depth of "minimum" pixels inside growth region
#define UPMC_MAX_MIN_PIXEL_DEPTH 36
// center position shift (distance between gravity center and local
// minimum)/region size
#define UPMC_CENTER_POSITION_SHIFT 37


#endif /*UPMC_FEATURES_H_*/
