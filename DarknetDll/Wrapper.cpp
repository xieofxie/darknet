#include "pch.h"
#include "detector_func.h"

#include <string>
#include <iostream>

using namespace System;
using namespace System::Collections::Generic;

// https://www.codeproject.com/articles/19354/quick-c-cli-learn-c-cli-in-less-than-10-minutes
namespace Wrapper {

    public ref class MyPoint
    {
    public:
        int x, y, z, time;
        String^ newStr;
        MyPoint(int x, int y, int z, int t)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->time = t;
        }
    };

    // noticed the ref?
    public ref class TokenAnalyzer
    {

    public:
        TokenAnalyzer() {
        };

        void Init() {
        }
        // use the C++/CLI type, like String^ (handles)
        List<MyPoint^>^ ProcessLine(int lineNbr, String^ line) {
            auto p = gcnew MyPoint(0, 1, 2, 3);
            p->newStr = line + gcnew String("hello world");
            auto res = gcnew List<MyPoint^>(1);
            res->Add(p);
            return res;
        }
    };
}

// https://stackoverflow.com/questions/41141326/error-lnk2019unresolved-external-symbol-winmain16-referenced-in-function-int
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    Wrapper::TokenAnalyzer p;
    p.Init();
    return 0;
}
