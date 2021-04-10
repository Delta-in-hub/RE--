// #include "./Regex/Regex.hpp"
#include "Regex.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <regex>

using namespace std;

bool f1 = true, f2 = false;
std::regex stdre;
RE::Regex myre;

template <typename Functor>
double execTime(Functor func)
{
    //高精度
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    func();
    high_resolution_clock::time_point t2   = high_resolution_clock::now();
    duration<double, std::micro> time_span = t2 - t1; //microseconds
    return time_span.count();
}

signed main(void)
{
    string rex("([hH][tT]{2}[pP]://|[hH][tT]{2}[pP][sS]://)(([A-Za-z0-9~-]+).)+([-A-Za-z0-9~\\\\/_%])+");
    string rex1("(?:[hH][tT]{2}[pP]://|[hH][tT]{2}[pP][sS]://)(?:(?:[A-Za-z0-9~-]+).)+(?:[-A-Za-z0-9~\\\\/_%])+");
    string tar("http://www.fapiao.com/dddp-web/pdf/download?request=6e7JGxxxxx4ILd-kExxxxxxxqJ4-CHLmqVnenXC692m74H38sdfdsazxcUmfcOH2fAfY1Vw__%5EDadIfJgiEf");
    cout << "std::regex use " << execTime([&rex1]() { stdre.assign(rex1); }) << "us to construct " << rex1 << endl;
    cout << "RE::Regex use " << execTime([&rex]() { myre.assign(rex); }) << "us to construct " << rex << endl;
    cout << "std::regex use " << execTime([&tar]() { f1 = (regex_match(tar, stdre)); }) << "us to match " << tar << endl;
    cout << "RE::Regex use " << execTime([&tar]() { f2 = (myre.match(tar)); }) << "us to match " << tar << endl;
    assert(f1 == f2);
    f1 = not f2;
    cout << endl;
    rex  = "[a-zA-Z0-9._]+@([a-zA-Z0-9]+.)+com";
    rex1 = "[a-zA-Z0-9._]+@(?:[a-zA-Z0-9]+.)+com";
    tar  = "power.overwhelming@aaaaaaaaaaaaaaaaaaaaaaaaaa";
    cout << "std::regex use " << execTime([&rex1]() { stdre.assign(rex1); }) << "us to construct " << rex1 << endl;
    cout << "RE::Regex use " << execTime([&rex]() { myre.assign(rex); }) << "us to construct " << rex << endl;
    cout << "std::regex use " << execTime([&tar]() { f1 = (regex_match(tar, stdre)); }) << "us to match\t" << tar << endl;
    cout << "RE::Regex use " << execTime([&tar]() { f2 = (myre.match(tar)); }) << "us to match\t" << tar << endl;
    assert(f1 == f2);
    f1 = not f2;
    cout << endl;
    rex  = "([01]+)+b";
    rex1 = "(?:[01]+)+b";
    tar  = "10101010110101001100101010101010101010101010101010000b";
    cout << "std::regex use " << execTime([&rex1]() { stdre.assign(rex1); }) << "us to construct " << rex1 << endl;
    cout << "RE::Regex use " << execTime([&rex]() { myre.assign(rex); }) << "us to construct " << rex << endl;
    cout << "std::regex use " << execTime([&tar]() { f1 = (regex_match(tar, stdre)); }) << "us to match " << tar << endl;
    cout << "RE::Regex use " << execTime([&tar]() { f2 = (myre.match(tar)); }) << "us to match " << tar << endl;
    assert(f1 == f2);
    f1  = not f2;
    tar = "10101010110101001100101";
    cout << "std::regex use " << execTime([&tar]() { f1 = (regex_match(tar, stdre)); }) << "us to match " << tar << endl;
    cout << "RE::Regex use " << execTime([&tar]() { f2 = (myre.match(tar)); }) << "us to match " << tar << endl;
    assert(f1 == f2);
    f1 = not f2;
    cout << endl;

    rex  = "[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+";
    rex1 = "[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(?:\\.[a-zA-Z0-9_-]+)+";
    tar  = "delta-in-hub@github.com";
    cout << "std::regex use " << execTime([&rex1]() { stdre.assign(rex1); }) << "us to construct " << rex1 << endl;
    cout << "RE::Regex use " << execTime([&rex]() { myre.assign(rex); }) << "us to construct " << rex << endl;
    cout << "std::regex use " << execTime([&tar]() { f1 = (regex_match(tar, stdre)); }) << "us to match " << tar << endl;
    cout << "RE::Regex use " << execTime([&tar]() { f2 = (myre.match(tar)); }) << "us to match " << tar << endl;
    assert(f1 == f2);
    f1 = not f2;
    cout << endl;
    return 0;
}