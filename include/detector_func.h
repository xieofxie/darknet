#ifndef DETECTOR_FUNC_H
#define DETECTOR_FUNC_H

#include "darknet.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DetectorState
{
	char** names;
	network net;
} DetectorState;

typedef struct DetectResult
{
	detection_with_class* results;
	int num;
} DetectResult;

LIB_API DetectorState* InitDetector(const char* name_list, const char* cfgfile, const char* weightfile, int benchmark_layers);

LIB_API DetectResult* Detect(DetectorState* state, const char* input, const char* output, float thresh,
	float hier_thresh, int ext_output, int letter_box);

LIB_API void DeinitResult(DetectResult* result);

LIB_API void DeinitDetector(DetectorState* state);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif
