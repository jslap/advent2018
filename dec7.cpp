
#include <iostream>
#include <fstream>

#include <string>
#include <string_view>
#include <cwctype>
#include <locale>
#include <algorithm>
#include <numeric>
#include <vector>
#include <random>

#include <fmt/format.h>


#include "absl/strings/str_split.h"
#include "absl/strings/numbers.h"

template <class T1, class T2>
void expectTest(const T1& got, const T2& expected)
{
    if (got == expected)
    {
        fmt::print("Test OK");
    }
    else
    {
        fmt::print("Test Fail: got {} should be {}", got, expected);
    }
    std::cout << std::endl;
}

using Step = std::string::value_type;
using Prec = std::pair<Step, Step>;
using PrecList = std::vector<Prec>;
using StepList = std::string;

template <class StringType>
Prec parsePrec(const StringType& s)
{
    return Prec(s[5], s[36]);
}

PrecList parsePrecList(const std::string& s)
{
    PrecList pl;
    auto lines = absl::StrSplit(s, '\n');
    for (const auto& line : lines)
    {
        pl.push_back(parsePrec(line));
    }
    return pl;
}

StepList getSteps(const PrecList& plist)
{
    std::map<Step, StepList> constraints;
    for (const auto& p: plist)
    {
        constraints[p.second] += std::string({p.first});
    }
    return {};
}

std::string getSteps(const std::string& s)
{
    auto pl = parsePrecList(s);
    auto steps = getSteps(pl);
    return steps;
}

int main (int , char**)
{
    // std::mt19937 rng;
    // rng.seed(1334);

    auto src1 = "Step C must be finished before step A can begin.\nStep C must be finished before step F can begin.\nStep A must be finished before step B can begin.\nStep A must be finished before step D can begin.\nStep B must be finished before step E can begin.\nStep D must be finished before step E can begin.\nStep F must be finished before step E can begin.";
    auto res1 = getSteps(src1);
   expectTest(res1, "CABDFE");

    std::ifstream ifs("dec6.txt");
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
    // auto res2 = genLargestArea(content);
    // expectTest(res2, 10);

    fmt::print("Done\n");
    return 0;
}


