#include <stdlib.h>
#include "darknet.h"
#include "network.h"
#include "region_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
#include "demo.h"
#include "option_list.h"

#ifndef __COMPAR_FN_T
#define __COMPAR_FN_T
typedef int (*__compar_fn_t)(const void*, const void*);
#ifdef __USE_GNU
typedef __compar_fn_t comparison_fn_t;
#endif
#endif

#include "http_stream.h"
#include "detector_func.h"

DetectorState* InitDetector(const char* name_list, const char* cfgfile, const char* weightfile, int benchmark_layers)
{
    int names_size = 0;
    char** names = get_labels_custom(name_list, &names_size); //get_labels(name_list);

    network net = parse_network_cfg_custom(cfgfile, 1, 1); // set batch=1
    if (weightfile) {
        load_weights(&net, weightfile);
    }
    net.benchmark_layers = benchmark_layers;
    fuse_conv_batchnorm(net);
    calculate_binary_weights(net);
    if (net.layers[net.n - 1].classes != names_size) {
        printf("\n Error: in the file %s number of names %d that isn't equal to classes=%d in the file %s \n",
            name_list, names_size, net.layers[net.n - 1].classes, cfgfile);
        if (net.layers[net.n - 1].classes > names_size) getchar();
    }
    srand(2222222);
    DetectorState* result = malloc(sizeof(DetectorState));
    result->names = names;
    result->net = net;
    return result;
}

DetectResult* Detect(DetectorState* state, const char* input, const char* output, float thresh,
    float hier_thresh, int ext_output, int letter_box)
{
    float nms = .45;    // 0.4F

    image im = load_image(input, 0, 0, state->net.c);
    image sized;
    if (letter_box) sized = letterbox_image(im, state->net.w, state->net.h);
    else sized = resize_image(im, state->net.w, state->net.h);
    layer l = state->net.layers[state->net.n - 1];

    float* X = sized.data;

    //time= what_time_is_it_now();
    double time = get_time_point();
    network_predict(state->net, X);
    //network_predict_image(&net, im); letterbox = 1;
    printf("%s: Predicted in %lf milli-seconds.\n", input, ((double)get_time_point() - time) / 1000);
    //printf("%s: Predicted in %f seconds.\n", input, (what_time_is_it_now()-time));

    int nboxes = 0;
    detection* dets = get_network_boxes(&state->net, im.w, im.h, thresh, hier_thresh, 0, 1, &nboxes, letter_box);
    if (nms) {
        if (l.nms_kind == DEFAULT_NMS) do_nms_sort(dets, nboxes, l.classes, nms);
        else diounms_sort(dets, nboxes, l.classes, nms, l.nms_kind, l.beta_nms);
    }

    int selected_detections_num;
    detection_with_class* selected_detections = get_actual_detections(dets, nboxes, thresh, &selected_detections_num, state->names);

    if (output) {
        draw_detections_v3(im, dets, nboxes, thresh, state->names, NULL, l.classes, ext_output);
        save_image(im, output);
    }

    free_detections(dets, nboxes);
    free_image(im);
    free_image(sized);

    DetectResult* result = malloc(sizeof(DetectResult));
    result->results = selected_detections;
    result->num = selected_detections_num;
    return result;
}

void DeinitResult(DetectResult* result)
{
    free(result->results);
    free(result);
}

void DeinitDetector(DetectorState* state)
{
    free_ptrs((void**)state->names, state->net.layers[state->net.n - 1].classes);
    free_network(state->net);
    free(state);
}
