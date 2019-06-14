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


// #include "absl/strings/numbers.h"
// #include "absl/strings/str_replace.h"
// #include "absl/strings/str_split.h"
// #include "absl/types/span.h"
// #include "absl/numeric/int128.h"

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

int computeCell(int x, int y, int serial)
{
    int p = (x+10)*(y*(x+10) + serial );
    return ((p/100)%10)-5;
}

using Grid = std::vector< std::vector<int>>;

Grid computeGrid(int cellDim, int serial)
{
    Grid g(cellDim, std::vector<int>(cellDim, 0));

    for (int x = 0 ; x < cellDim; x++)
    {
        for (int y = 0; y < cellDim; y++)
        {
            g[y][x] = computeCell(x, y, serial);
        }
    }
    return g;
}

Grid computePartialSums(const Grid& g)
{
    const std::size_t cellDim = g.size();
    Grid g2 = g;
    for (int y = 1; y < cellDim; y++)
    {
        g2 [y][0] = g2[y-1][0] + g2[y][0];
    }
    for (int x = 1; x < cellDim; x++)
    {
        g2 [0][x] = g2[0][x-1] + g2[0][x];
    }
    for (int y = 1; y < cellDim; y++)
    {
        for (int x = 1; x < cellDim; x++)
        {
            g2 [y][x] = g2[y][x] + g2[y][x-1] + g2[y-1][x] - g2[y-1][x-1];
        }
    }
    return g2;
}


std::string getMaxCellAll(int cellDim, int serial)
{
    Grid g = computeGrid(cellDim, serial);

    Grid psum = computePartialSums(g);

    int xmax = 0;
    int ymax = 0;
    int sizemax = 1;
    int maxval = 0;
    for (int y = 0; y < cellDim; y++)
    {
        for (int x = 0; x < cellDim; x++)
        {
            int maxPossSize = std::min(cellDim-x, cellDim-y);
            for (int s = 1; s < maxPossSize; s++)
            {
                int rectEndX = x + s;
                int rectEndY = y + s;
                int curVal = psum[y+s][x+s] + psum[y][x] - (psum[y+s][x] + psum[y][x+s]);
                if (curVal > maxval)
                {
                    maxval = curVal;
                    sizemax = s;
                    ymax = y;
                    xmax = x;
                }
            }
        }
    }

    return fmt::format("{},{},{}", xmax+1,ymax+1,sizemax);
}


int getMaxCell(int cellDim, int serial)
{
    Grid g = computeGrid(cellDim, serial);

    Grid g2 = g;
    for (int y = 0; y < cellDim; y++)
    {
        const auto& row = g[y];
        auto& newrow = g2[y];
        int curSum = row[0] + row[1];
        for (int x = 0; x < cellDim-2; x++)
        {
            curSum += row[x+2];
            newrow[x] = curSum;
            curSum -= row[x];
        }
    }
    std::swap(g, g2);

    for (int x = 0; x < cellDim-2; x++)
    {
        int curSum = g[0][x] + g[1][x];
        for (int y = 0; y< cellDim-2; y++)
        {
            curSum += g[y+2][x];
            g2[y][x] = curSum;
            curSum -= g[y][x];
        }
    }
    std::swap(g, g2);

    int maxx = 0;
    int maxy = 0;
    int maxval = -100000;
    for (int x = 0; x < cellDim-2; x++)
    {
        for (int y = 0; y< cellDim-2; y++)
        {
            if (g[y][x] > maxval)
            {
                maxval = g[y][x];
                maxx = x;
                maxy = y;
            }
        }
    }

    fmt::print(fmt("found max {} at ({},{})"), maxval, maxx, maxy);
    return maxval;
}

int main(int, char**)
{
    expectTest(computeCell(3, 5, 8), 4);

    expectTest(computeCell(122, 79, 57), -5);
    expectTest(computeCell(217, 196, 39), 0);
    expectTest(computeCell(101, 153, 71), 4);

    expectTest(getMaxCell(300, 18), 29);
    expectTest(getMaxCell(300, 42), 30);
    expectTest(getMaxCell(300, 5235), 28);

    expectTest(getMaxCellAll(300, 18), "90,269,16");
    expectTest(getMaxCellAll(300, 42), "232,251,12");
    expectTest(getMaxCellAll(300, 5235), "Yo");
    

    fmt::print("Done\n");

    return 0;
}


// Fuel cell at  122,79, grid serial number 57: power level -5.
// Fuel cell at 217,196, grid serial number 39: power level  0.
// Fuel cell at 101,153, grid serial number 71: power level  4.
