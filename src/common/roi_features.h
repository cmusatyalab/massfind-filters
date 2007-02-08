#ifndef ROI_FEATURES_H_
#define ROI_FEATURES_H_

// constants for ROI text attributes

#define UPMC_PREFIX "rawf"	// visual features
#define EDMF_PREFIX "edmf"	// normalized features for euclidian distance
#define BDMF_PREFIX "bdmf"	// features for boosted LDM distance
#define CONTOUR_X 	"contourx" // mass contour features
#define CONTOUR_Y	"contoury" // together these are a list of points
#define MI_MEAN		"imean"	 // mean pixel intensity
#define MI_STDEV	"istdev" // stdev of pixel intensity

#define NUM_UPMC "numrawf"
#define NUM_EDMF "numedmf"
#define NUM_BDMF "numbdmf"
#define NUM_CONTOUR "numcontour"  // number of POINTS

#define CONTOUR_DELIM ';'	// delimiter in contour point string

#endif /*ROI_FEATURES_H_*/
