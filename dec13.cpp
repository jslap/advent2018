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
#include "absl/strings/str_split.h"
// #include "absl/types/span.h"
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

struct Pos
{
    Pos(int _x, int _y): x(_x), y(_y) {}
    bool operator ==(const Pos& rhs) const {return x==rhs.x && y==rhs.y;}
    int x = 0;
    int y = 0;
};

namespace fmt
{
    template <>
    struct formatter< Pos >
    {
        template < typename ParseContext >
        constexpr auto parse(ParseContext& ctx)
        {
            return ctx.begin();
        }

        template < typename FormatContext >
        auto format(const Pos& p, FormatContext& ctx)
        {
            format_to(ctx.begin(), "({},{})", p.x, p.y);
            return ctx.begin();
        }
    };

} // namespace fmt

enum class Dir { 
    North = 0, East = 1, South = 2, West = 3
};

Dir& operator++(Dir& d)
{
    return d = (d == Dir::West) ? Dir::North : static_cast<Dir>(static_cast<int>(d)+1);
}

Dir& operator--(Dir& d)
{
    return d = (d == Dir::North) ? Dir::West : static_cast<Dir>(static_cast<int>(d)-1);
}

enum class Turn { Left, Straight, Right };

Turn& operator++(Turn& t)
{
    return t = (t == Turn::Right) ? Turn::Left : static_cast<Turn>(static_cast<int>(t)+1);
}

Dir applyTurn(Dir d, Turn t)
{
    switch (t)
    {
        case Turn::Left: return --d;
        case Turn::Straight: return d;
        case Turn::Right: return ++d;
    }
}

struct Elf
{
    Elf(int _id, Pos _p, Dir _d): id(_id), p(_p), dir(_d) {}
    int id;
    Pos p;
    Dir dir;
    Turn nextTurn = Turn::Left;
    bool isAlive = true;

    void applyCroosroad()
    {
        switch (dir)
        {
            case Dir::North:
                dir = Dir::West;
                break;
            case Dir::South:
                dir = Dir::East;
                break;
            case Dir::East:
                dir = Dir::South;
                break;
            case Dir::West:
                dir = Dir::North;
                break;
        }

    }
};

using Map = std::vector< std::vector <std::string::value_type> >;


bool elfOrder(const Elf& e1, const Elf&e2)
{
    if (e1.p.y == e2.p.y)
        return e1.p.x < e2.p.x;
    return e1.p.y < e2.p.y;
}

bool elfIsAlive(const Elf& e)
{
    return e.isAlive;
}

struct MapState
{
    Map roads;
    std::vector<Elf> elfList;
    Map elfMap;
};

MapState loadStart(const std::string& s)
{
    MapState retVal;
    auto lines = absl::StrSplit(s, '\n');
    for (const auto& line : lines)
    {
        retVal.roads.emplace_back(line.begin(), line.end());
    }
    int elfId = 1;
    for (int y = 0; y < retVal.roads.size(); y++)
    {
        std::vector <std::string::value_type> &curMapLine = retVal.roads[y];
        for (int x = 0; x < curMapLine.size(); x++)
        {
            auto& curChar = curMapLine[x];
            switch (curChar)
            {
                case '<':
                {
                    curMapLine[x] = '-';
                    retVal.elfList.emplace_back(elfId++, Pos(x, y), Dir::West);
                }
                break;
                case '>':
                {
                    curMapLine[x] = '-';
                    retVal.elfList.emplace_back(elfId++, Pos(x, y), Dir::East);
                }
                break;
                case '^':
                {
                    curMapLine[x] = '|';
                    retVal.elfList.emplace_back(elfId++, Pos(x, y), Dir::North);
                }
                break;
                case 'v':
                {
                    curMapLine[x] = '|';
                    retVal.elfList.emplace_back(elfId++, Pos(x, y), Dir::South);
                }
                break;
            }
        }
    } 
    return retVal;   
}

void initElfMap(MapState& state)
{
    state.elfMap = state.roads;
    for (auto &l : state.elfMap)
    {
        l.assign(l.size(), 0);
    }

    for (const auto& elf : state.elfList)
    {
        state.elfMap[elf.p.y][elf.p.x] = 1;
    }

}

Pos getFirstCrash(const std::string& s, bool firstCrash = true)
{
    MapState state = loadStart(s);
    initElfMap(state);



    bool didCrash = false;
    Pos crashPos(0,0);

    int curTurn = 0;
    while (!didCrash)
    {
        auto nbAlive = std::count_if(state.elfList.begin(), state.elfList.end(), elfIsAlive);
        if (nbAlive <= 1)
        {
            break;
        }
        
        curTurn++;
//        fmt::print(fmt("curTurn {}\n"), curTurn);
        std::sort(state.elfList.begin(), state.elfList.end(), &elfOrder);


        for (auto& elf : state.elfList)
        {
            if (!elf.isAlive)
            {
                continue;
            }
            
            Pos posBefore = elf.p;
            // advance pos.
            switch (elf.dir)
            {
                case Dir::East:
                    elf.p.x++;
                    break;
                case Dir::West:
                    elf.p.x--;
                    break;
                case Dir::North:
                    elf.p.y--;
                    break;
                case Dir::South:
                    elf.p.y++;
                    break;
            }
            if (state.elfMap[elf.p.y][elf.p.x] != 0)
            {
                //handle crash
                if (firstCrash)
                {
                    crashPos = elf.p;
                    didCrash = true;
                    break;
                }
                else
                {
                    // find crashing elves.
                    elf.isAlive = false;
                    state.elfMap[posBefore.y][posBefore.x] = 0;
                    state.elfMap[elf.p.y][elf.p.x] = 0;
                    Pos curCrashPos = elf.p;
                    auto crashElf2 = std::find_if(state.elfList.begin(), state.elfList.end(), [&curCrashPos](const Elf& e){
                        return e.p == curCrashPos && e.isAlive;
                    });
                    if (crashElf2 == state.elfList.end())
                    {
                        fmt::print("Error crash");
                        return Pos(-1, -1);
                    }
                    
                    // set as not alive.
                    crashElf2->isAlive = false;

                }
            }

            if (elf.isAlive)
            {
                state.elfMap[posBefore.y][posBefore.x] = 0;
                state.elfMap[elf.p.y][elf.p.x] = 1;

                // check for turn
                auto mapElt = state.roads[elf.p.y][elf.p.x];
                if (mapElt == '/')
                {
                    switch (elf.dir)
                    {
                        case Dir::North:
                            elf.dir = Dir::East;
                            break;
                        case Dir::South:
                            elf.dir = Dir::West;
                            break;
                        case Dir::East:
                            elf.dir = Dir::North;
                            break;
                        case Dir::West:
                            elf.dir = Dir::South;
                            break;
                    }
                }
                else if (mapElt == '\\')
                {
                    switch (elf.dir)
                    {
                        case Dir::North:
                            elf.dir = Dir::West;
                            break;
                        case Dir::South:
                            elf.dir = Dir::East;
                            break;
                        case Dir::East:
                            elf.dir = Dir::South;
                            break;
                        case Dir::West:
                            elf.dir = Dir::North;
                            break;
                    }
                }
                else if (mapElt == '+')
                {
                    elf.dir = applyTurn(elf.dir, elf.nextTurn);
                    ++elf.nextTurn;
                }                
            }

        }
    }

    if (firstCrash)
    {
        return crashPos;
    }
    else
    {
        auto fit = std::find_if(state.elfList.begin(), state.elfList.end(), elfIsAlive);
        if (fit != state.elfList.end())
            return fit->p;
    }
    return Pos(-1,-1);
}

int main(int, char**)
{
//    constexpr auto src1 = "|\nv\n|\n|\n|\n^\n|";
//     expectTest(getFirstCrash(src1), Pos(0,3));
    
//   constexpr auto src2 =
//R"foo(/->-\        )foo" "\n"
//R"foo(|   |  /----\)foo" "\n"
//R"foo(| /-+--+-\  |)foo" "\n"
//R"foo(| | |  | v  |)foo" "\n"
//R"foo(\-+-/  \-+--/)foo" "\n"
//R"foo(  \------/   )foo";
//    expectTest(getFirstCrash(src2), Pos(7,3));
    
     std::ifstream ifs2("dec13.txt");
     std::string src3((std::istreambuf_iterator< char >(ifs2)),
         (std::istreambuf_iterator< char >()));
//      expectTest(getFirstCrash(src3), Pos(139,65));

//   constexpr auto src21 =
//R"foo(/>-<\  )foo" "\n"
//R"foo(|   |  )foo" "\n"
//R"foo(| /<+-\)foo" "\n"
//R"foo(| | | v)foo" "\n"
//R"foo(\>+</ |)foo" "\n"
//R"foo(  |   ^)foo" "\n"
//R"foo(  \<->/)foo";
//      expectTest(getFirstCrash(src21, false), Pos(6,4));

      expectTest(getFirstCrash(src3, false), Pos(6,4));

    fmt::print("Done\n");

    return 0;
}
