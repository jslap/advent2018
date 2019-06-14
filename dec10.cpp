#include <fstream>
#include <iostream>

#include <algorithm>
#include <cwctype>
#include <list>
#include <locale>
#include <numeric>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#define FMT_STRING_ALIAS 1
#include <fmt/format.h>


#include "absl/strings/numbers.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"
#include "absl/types/span.h"
#include "absl/numeric/int128.h"

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
using Pos = std::pair<int, int>; // x, y
using Box = std::pair<Pos, Pos>; // pmin, pmax

Pos minWith(const Pos& p1, const Pos& p2)
{
    return Pos(std::min(p1.first, p2.first), std::min(p1.second, p2.second));
}
Pos maxWith(const Pos& p1, const Pos& p2)
{
    return Pos(std::max(p1.first, p2.first), std::max(p1.second, p2.second));
}

void addPosToBox(Box& b, const Pos& p)
{
    b.first = minWith(b.first, p);
    b.second = maxWith(b.second, p);
}

int64_t boxArea(const Box& b)
{
    int64_t width = b.second.first-b.first.first;
    int64_t height = b.second.second-b.first.second;
    return width*height;
}

struct Star
{
    int px = 0;
    int py = 0;

    int vx = 0;
    int vy = 0;

    // Star(int _px, int _py, int _vx, int _vy): px(_px), py(_py), vx(_vx), vy(_vy) {}
    Pos getPos() const 
    {
        return Pos(px, py);
    }
    void doMove()
    {
        px+=vx;
        py+=vy;
    }
};

struct Constel
{
    std::vector<Star> starList;

    Constel(const std::string& s)
    {
        auto lines = absl::StrSplit(s, '\n');
        for (const auto& line : lines)
        {
            if (!line.empty())
            {
                const auto delim = absl::ByAnyChar(", <>");
                std::vector<std::string> splitText = absl::StrSplit(line, delim, absl::SkipEmpty());
                Star star;
                absl::SimpleAtoi(splitText[1], &star.px);
                absl::SimpleAtoi(splitText[2], &star.py);
                absl::SimpleAtoi(splitText[4], &star.vx);
                absl::SimpleAtoi(splitText[5], &star.vy);
                starList.push_back(star);
            }
        }
    }
    void doMove()
    {
        for (auto& s : starList)
        {
            s.doMove();
        }
    }

    Box getBox() const 
    {
        auto first = starList.front().getPos();
        Box b(first, first);
        for (const auto& s : starList)
        {
            auto p = s.getPos();
            addPosToBox(b, p);
        }
        return b;
    }
};

std::string compute(const std::string& s)
{
    Constel origin(s);
    Constel c = origin;

    int curTime = 0;
    int curMinTime = 0;
    auto curMinArea = boxArea(c.getBox());

    while (curTime-curMinTime < 20)
    {
        c.doMove();
        curTime++;
        auto curArea = boxArea(c.getBox());
        if (curArea < curMinArea)
        {
            curMinTime = curTime;
            curMinArea = curArea;
        }
    }

    fmt::print("Min area: {} at time {}", curMinArea, curMinTime);

    int time1 = curMinTime - 10;
    int time2 = curMinTime + 10;
    c = origin;
    for (int t = 0; t < time2; t++)
    {
        c.doMove();
        if (t > time1)
        {
            auto curBox = c.getBox();
            fmt::print("time {} : area: {}  ({}, {}) , ({}, {})\n", t, boxArea(curBox),
                curBox.first.first, curBox.first.second, curBox.second.first, curBox.second.second);
        }
        if (t == curMinTime-1)
        {
            auto curBox = c.getBox();
            curBox.first.first --;
            curBox.first.second --;
            curBox.second.first ++;
            curBox.second.second ++;
            int width = curBox.second.first - curBox.first.first;
            int height = curBox.second.second - curBox.first.second;

            std::vector < std::string > image(height, std::string(width, '.') );
            for (const auto& s : c.starList )
            {
                image[s.py - curBox.first.second][s.px - curBox.first.first] = '#';
            }

            for (const auto& row : image)
            {
                fmt::print("{}\n", row);
            }

        }

    }
    return "Yo";
}



int main(int, char**)
{
    std::ifstream ifs("dec10_1.txt");
    std::string src((std::istreambuf_iterator< char >(ifs)),
        (std::istreambuf_iterator< char >()));
    auto res1 = compute(src);
    expectTest(res1, "HI");

    std::ifstream ifs2("dec10.txt");
    std::string src2((std::istreambuf_iterator< char >(ifs2)),
        (std::istreambuf_iterator< char >()));
    auto res2 = compute(src2);
    expectTest(res2, "FHMEQGIRSXNWZBCLOTUADJPKVY");


    fmt::print("Done\n");

    return 0;
}
