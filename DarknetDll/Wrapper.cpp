#include "pch.h"
#include "detector_func.h"

#include <string>
#include <iostream>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

// https://www.codeproject.com/articles/19354/quick-c-cli-learn-c-cli-in-less-than-10-minutes
namespace Wrapper {

    public ref class Result
    {
    public:
        Result() {
            result = gcnew List<String^>(0);
        }

        List<String^>^ result;
    };

    // noticed the ref?
    public ref class Detector
    {
        DetectorState* state = nullptr;
    public:
        Detector() {
        }

        ~Detector() {
            if (!state) {
                DeinitDetector(state);
                state = nullptr;
            }
        }

        void Init(String^ base) {
            auto name_list = Marshal::StringToHGlobalAnsi(base + "/data/coco.names");
            auto cfgfile = Marshal::StringToHGlobalAnsi(base + "/cfg/yolov4.cfg");
            auto weightfile = Marshal::StringToHGlobalAnsi(base + "/cfg/yolov4.weights");
            state = InitDetector(static_cast<char*>(name_list.ToPointer()), static_cast<char*>(cfgfile.ToPointer()), static_cast<char*>(weightfile.ToPointer()), 0);
            Marshal::FreeHGlobal(name_list);
            Marshal::FreeHGlobal(cfgfile);
            Marshal::FreeHGlobal(weightfile);
        }

        Result^ Process(String^ input, String^ output) {
            auto inputC = Marshal::StringToHGlobalAnsi(input);
            auto outputC = output == nullptr ? IntPtr() : Marshal::StringToHGlobalAnsi(output);
            auto dResult = Detect(state, static_cast<char*>(inputC.ToPointer()), static_cast<char*>(outputC.ToPointer()), 0.25f, 0.5f, 0, 0);
            auto result = gcnew Result();
            for (int i = 0;i < dResult->num;++i) {
                auto nameC = state->names[dResult->results[i].best_class];
                auto name = gcnew String(nameC);
                result->result->Add(name);
            }
            DeinitResult(dResult);
            Marshal::FreeHGlobal(inputC);
            if (output != nullptr) Marshal::FreeHGlobal(outputC);
            return result;
        }
    };
}

// https://stackoverflow.com/questions/41141326/error-lnk2019unresolved-external-symbol-winmain16-referenced-in-function-int
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    Wrapper::Detector p;
    return 0;
}
