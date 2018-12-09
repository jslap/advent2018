
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


using Char = std::string::value_type;
using PosMap = std::vector <std::vector <int> >;
const int Untreated = -1;
const int NoNearest = 0;

struct Pos 
{
    Pos() = default;
    Pos(int _x, int _y): x(_x), y(_y) {}
    int x = 0; 
    int y = 0;
};

int distPos(const Pos& p1, const Pos& p2)
{
    return std::abs(p1.x-p2.x) + std::abs(p1.y-p2.y);
}

using PosVec = std::vector<Pos>;

void printGrid(const PosMap& posMap)
{
    fmt::print("Grid:\n");
    for (const auto l : posMap)
    {
        for (const auto val: l)
        {
            if (val < 0) fmt::print("?");
            else if (val == 0 ) fmt::print(".");
            else fmt::print("{}", std::string({static_cast<char>('A' + val - 1), '\0'}));
        }
        fmt::print("\n");
    }

}

PosVec pareseInput(const std::string& s)
{
    PosVec posVec;
    auto lines = absl::StrSplit(s, '\n');
    for (const auto& line : lines)
    {
        const std::string delim(", ");
        std::vector<std::string> splitText = absl::StrSplit(line, delim);

        Pos p;
        absl::SimpleAtoi(splitText[0], &p.x);
        absl::SimpleAtoi(splitText[1], &p.y);
        posVec.push_back(p);
    }
    return posVec;
}

PosMap buildPosMap(const PosVec& posVec)
{
    Pos minPos = posVec[0];
    Pos maxPos = posVec[0];
    for (const auto& p : posVec)
    {
        minPos.x = std::min(minPos.x, p.x);
        minPos.y = std::min(minPos.y, p.y);
        maxPos.x = std::max(maxPos.x, p.x);
        maxPos.y = std::max(maxPos.y, p.y);
    }

    minPos.x--;
    minPos.y--;
    maxPos.x++;
    maxPos.y++;

    const int posMapWidth = maxPos.x-minPos.x + 10;
    const int posMapHeight = maxPos.y-minPos.y + 10;
    const PosMap blankPosMap(posMapHeight, std::vector <int>(posMapWidth, Untreated));
    PosMap posMap = blankPosMap;
    int pointIndex = 1;
    for (const auto p: posVec)
    {
        posMap[p.y-minPos.y][p.x-minPos.x] = pointIndex++; 
    }
    return posMap;
}

bool checkPosMap(const PosMap& posMap, const PosMap& originalPosMap)
{
    const auto posMapHeight = posMap.size();
    const auto posMapWidth = posMap[0].size();

    for (int x = 0; x < posMapWidth; x++)
    {
        for (int y = 0; y < posMapHeight; y++)
        {
            Pos refPos(x, y);
            auto foundClosest = posMap[refPos.y][refPos.x];
            if (foundClosest < 0)
            {
                continue;
            }

            Pos closestPos(-1, -1);
            int nbClosest = 0;

            // find closest with value.
            for (int x2 = 0; x2 < posMapWidth; x2++)
            {
                for (int y2 = 0; y2 < posMapHeight; y2++)
                {
                    Pos checkPos(x2, y2);
                    if (originalPosMap[y2][x2] > 0)
                    {
                        if (closestPos.x == -1)
                        {
                            closestPos = checkPos;
                            nbClosest = 1;
                        }
                        else if (distPos(refPos, checkPos) == distPos(refPos, closestPos))
                        {
                            nbClosest++;
                        }
                        else if (distPos(refPos, checkPos) < distPos(refPos, closestPos))
                        {
                            closestPos = checkPos;
                            nbClosest = 1;
                        }
                    }
                }
            }
            
            
            auto realClosest = originalPosMap[closestPos.y][closestPos.x];
            if (nbClosest == 1)
            {
                if (foundClosest != realClosest)
                {
                    return false;
                }
            }
            else
            {
                if (foundClosest != NoNearest)
                {
                    return false;
                }
            }
        }
    }
    return true;
}


int genLargestArea(const PosMap& originalPosMap)
{
    PosMap posMap = originalPosMap;

    const auto posMapHeight = posMap.size();
    const auto posMapWidth = posMap[0].size();

    const std::array<Pos,4> neighboorList = {Pos(-1, 0), Pos(1, 0), Pos(0, 1), Pos(0, -1)};
    int remaining = 2;
    // printGrid(posMap);
    while (remaining > 0)
    {
        PosMap newDistPosMap = posMap;
        for (int x = 0; x < posMapWidth; x++)
        {
            for (int y = 0; y < posMapHeight; y++)
            {
                const auto oldValueAtPos = posMap[y][x];

                if (oldValueAtPos != Untreated)
                {
                    for (const auto& neigh : neighboorList)
                    {
                        Pos curNeighPos(x+neigh.x, y+neigh.y);

                        if (curNeighPos.x >= 0 && curNeighPos.y >= 0 && curNeighPos.x < posMapWidth && curNeighPos.y < posMapHeight)
                        {
                            auto &curValueAtNeigh = newDistPosMap[curNeighPos.y][curNeighPos.x];

                            if (oldValueAtPos == NoNearest)
                            {
                                curValueAtNeigh = NoNearest;
                            }
                            else if (curValueAtNeigh == Untreated)
                            {
                                curValueAtNeigh = oldValueAtPos;
                            }
                            else if (curValueAtNeigh != oldValueAtPos)
                            {
                                curValueAtNeigh = NoNearest;
                            }
                        }
                    }
                }
            }
        }

        remaining = 0;
        for (int x = 0; x < posMapWidth; x++)
        {
            for (int y = 0; y < posMapHeight; y++)
            {
                auto &oldValueAtPos = posMap[y][x];
                const auto &curValueAtPos = newDistPosMap[y][x];
                if (oldValueAtPos == Untreated)
                {
                    oldValueAtPos = curValueAtPos;
                }
                if (oldValueAtPos == Untreated)
                {
                    remaining++;
                }
            }
        }

        if (!checkPosMap(originalPosMap, posMap))
        {
            fmt::print("ERROR\n");
            exit(33);
        }

    }
        if (!checkPosMap(posMap, originalPosMap))
        {
            fmt::print("ERROR\n");
            exit(33);
        }
   printGrid(posMap);

    std::set<int> infiniteElements;
    for (int x = 0; x < posMapWidth; x++)
    {
        infiniteElements.insert(posMap.front()[x]);
        infiniteElements.insert(posMap.back()[x]);
    }
    for (int y = 0; y < posMapHeight; y++)
    {
        infiniteElements.insert(posMap[y].front());
        infiniteElements.insert(posMap[y].back());
    }

    for ( auto curInElt : infiniteElements)
    {
        for (int y = 0; y < posMapHeight; y++)
        {
            for (int x = 0; x < posMapWidth; x++)
            {
                auto &curValueAtPos = posMap[y][x];
                if (curValueAtPos == curInElt)
                {
                    curValueAtPos = NoNearest;
                }
            }
        }
    }

    printGrid(posMap);
    std::map<int, std::size_t> nonInfiniteElements;
    for (int y = 0; y < posMapHeight; y++)
    {
        for (int x = 0; x < posMapWidth; x++)
        {
            auto &curValueAtPos = posMap[y][x];
            if (curValueAtPos != NoNearest)
            {
                nonInfiniteElements[curValueAtPos]++;
            }
        }
    }

    std::size_t maxVal = 0;
    for (auto nonInfElt: nonInfiniteElements)
    {
        maxVal = std::max(maxVal, nonInfElt.second);
    }

    return maxVal;
}

int genLargestArea(const PosVec& posVec)
{
    const PosMap originalPosMap = buildPosMap(posVec);
    return genLargestArea(originalPosMap);
}

int genLargestArea(const std::string& s)
{
    PosVec posVec = pareseInput(s);
    return genLargestArea(posVec);
}



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

int main (int , char**)
{
    std::mt19937 rng;
    rng.seed(1334);
    int i = 0;
   while (true)
    {
        int gridSize = 12;
        int w = gridSize;
        int h = gridSize;
        std::uniform_int_distribution<std::mt19937::result_type> dist6(1,gridSize-1); // distribution in range [1, gridSize]

        int nbPos = 7;
        PosVec posVec;
        for (int j = 0; j < nbPos; j++)
        {
            Pos p(dist6(rng), dist6(rng));
            posVec.push_back(p);
            fmt::print("pos {} : ({}, {})", j, p.x, p.y);
        }

        fmt::print("random test {} res: {}\n", i++, genLargestArea(posVec));

        std::getchar();
    }


    auto src1 = "1, 1\n1, 6\n8, 3\n3, 4\n5, 5\n8, 9";
   auto res1 = genLargestArea(src1);
   expectTest(res1, 17);

    std::ifstream ifs("dec6.txt");
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
    auto res2 = genLargestArea(content);
    expectTest(res2, 10);

    return 0;
}


