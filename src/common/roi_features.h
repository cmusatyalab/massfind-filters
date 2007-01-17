#ifndef ROI_FEATURES_H_
#define ROI_FEATURES_H_

// constants for ROI text attributes

#define MAXFEATURELEN 80
#define MAXFNAMELEN 20

#define UPMC_PREFIX "rawf"	// visual features
#define EDMF_PREFIX "edmf"	// normalized features for euclidian distance
#define BDMF_PREFIX "bdmf"	// features for boosted LDM distance
#define CONTOUR_X_PREFIX "contourx" // mass contour features
#define CONTOUR_Y_PREFIX "contoury" // a pair of these is a point

#define NUM_UPMC "numrawf"
#define NUM_EDMF "numedmf"
#define NUM_BDMF "numbdmf"
#define NUM_CONTOUR "numcontour"  // number of POINTS

#endif /*ROI_FEATURES_H_*/
