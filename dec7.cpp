
#include <fstream>
#include <iostream>

#include <algorithm>
#include <cwctype>
#include <locale>
#include <numeric>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>


#include "absl/strings/numbers.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"

template < class T1, class T2 >
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
using Prec = std::pair< Step, Step >;
using PrecList = std::vector< Prec >;
using StepList = std::string;

using Constraint = std::map< Step, StepList >;

template < class StringType >
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
        if (!line.empty())
            pl.push_back(parsePrec(line));
    }
    return pl;
}

Constraint generateConstraints(const PrecList& plist)
{
    Constraint constraints;
    for (const auto& p : plist)
    {
        constraints[p.first] = "";
    }
    for (const auto& p : plist)
    {
        constraints[p.second] += std::string({ p.first });
    }
    return constraints;
}

void removeFromConstraint(Constraint& constraints, Step step)
{
    constraints.erase(step);
    for (auto& kv : constraints)
    {
        kv.second.erase(std::remove(kv.second.begin(), kv.second.end(), step), kv.second.end());
    }
}


int workerTime(const std::string& s, int nbWorker, int addedTime)
{
    auto pl = parsePrecList(s);
    Constraint constraints = generateConstraints(pl);

    using RemainingTime = std::map< Step, int >;
    RemainingTime remainingTime;
    auto getTimeLeft = [](const RemainingTime& r) {
        return std::accumulate(std::begin(r), std::end(r), 0, [](const int previous, const std::pair< Step, int >& p) { return previous + p.second; });
    };
    for (const auto& kv : constraints)
    {
        remainingTime[kv.first] = addedTime + kv.first - 'A' + 1;
    }

    int timeCount = 0;
    std::set< Step > executedSteps;
    while (getTimeLeft(remainingTime) > 0)
    {
        timeCount++;

        // find tasks for workers.
        if (executedSteps.size() < nbWorker)
        {
            for (const auto& kv : constraints)
            {
                if (kv.second.empty())
                {
                    executedSteps.insert(kv.first);
                    if (executedSteps.size() >= nbWorker)
                    {
                        break;
                    }
                }
            }
        }

        fmt::print("{:<5}", timeCount);
        std::set< Step > endedSteps;
        for (auto task : executedSteps)
        {
            fmt::print("{:<3}", task);
            if (--remainingTime[task] == 0)
            {
                endedSteps.insert(task);
            }
        }
        for (auto task : endedSteps)
        {
            removeFromConstraint(constraints, task);
            executedSteps.erase(task);
        }

        fmt::print("\n");
    }
    return timeCount;
}

StepList getSteps(const PrecList& plist)
{
    Constraint constraints = generateConstraints(plist);
    StepList sl;
    while (!constraints.empty())
    {
        Step nextStep = 0;
        for (const auto& kv : constraints)
        {
            if (kv.second.empty())
            {
                nextStep = kv.first;
                break;
            }
        }

        if (nextStep == 0)
        {
            fmt::print("Error");
        }
        sl += std::string(1, nextStep);
        removeFromConstraint(constraints, nextStep);
    }

    return sl;
}

std::string getSteps(const std::string& s)
{
    auto pl = parsePrecList(s);
    auto steps = getSteps(pl);
    return steps;
}

int main(int, char**)
{
    auto src1 = "Step C must be finished before step A can begin.\nStep C must be finished before step F can begin.\nStep A must be finished before step B can begin.\nStep A must be finished before step D can begin.\nStep B must be finished before step E can begin.\nStep D must be finished before step E can begin.\nStep F must be finished before step E can begin.";
    auto res1 = getSteps(src1);
    expectTest(res1, "CABDFE");

    std::ifstream ifs("dec7.txt");
    std::string content((std::istreambuf_iterator< char >(ifs)),
        (std::istreambuf_iterator< char >()));
    auto res2 = getSteps(content);
    expectTest(res2, "FHMEQGIRSXNWZBCLOTUADJPKVY");

    auto res12 = workerTime(src1, 2, 0);
    expectTest(res12, 15);

    auto res22 = workerTime(content, 5, 60);
    expectTest(res22, 15);

    fmt::print("Done\n");
    return 0;
}
