#ifndef DETECTOR_FUNC_H
#define DETECTOR_FUNC_H

#include "darknet.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DetectorState
{
	char** names;
	network net;
};

typedef struct DetectorState DetectorState;

LIB_API DetectorState* InitDetector(const char* name_list, const char* cfgfile, const char* weightfile, int benchmark_layers);

LIB_API void Detect(DetectorState* state, const char* input, const char* output, float thresh,
	float hier_thresh, int ext_output, int letter_box);

LIB_API void DeinitDetector(DetectorState* state);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif
