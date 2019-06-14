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

using BigInt = absl::uint128;
using PlantSlot = bool;
constexpr PlantSlot EmptySlot = false;
constexpr PlantSlot FullSlot = true;

struct Population
{

    void copyInto(Population& into) const
    {
        std::fill(into.table.begin(), into.table.end(), EmptySlot);
        for (int i = 0; i < into.table.size(); i++)
        {
            int indexInSrc = i + into.offset - offset;
            if (indexInSrc >= 0 && indexInSrc < table.size())
            {
                into.table[i] = table[indexInSrc];
            }
        }
    }

    BigInt getSignature() const
    {
        BigInt retVal = 0;
        for (int i = getFirstSlot(); i<=getLastSlot(); i++)
        {
            auto c = table[i];
            retVal <<= 1;
            if (c == FullSlot)
            {
                retVal |= 1;
            }
        }
        return retVal;
    }

    int getFirstSlot() const
    {
        int firstSlot = 0;
        for ( ; firstSlot < table.size() ; firstSlot++)
        {
            if (table[firstSlot])
            {
                return firstSlot;
            }
        }
        fmt::print("Error");
        return -1;
    }

    int getLastSlot() const
    {
        int lastSlot = table.size()-1;
        for ( ; lastSlot >= 0 ; lastSlot--)
        {
            if (table[lastSlot])
            {
                return lastSlot;
            }
        }
        fmt::print("Error");
        return -1;
    }

    int nbPlant() const 
    {
        return std::count(table.begin(), table.end(), FullSlot);
    }

    int getSum() const
    {
        int res = 0;
        for (int i = 0; i < table.size(); i++)
        {
            if (table[i] == FullSlot)
            {
                res += (i+offset);
            }
        }

        return res;
    }

    bool operator < (const Population& rhs) const
    {
        return getSignature() < rhs.getSignature();
    }

    int offset = 0;
    std::vector<PlantSlot> table;
};

constexpr int NbNeighb = 5;
using Rules = std::array<PlantSlot, 1<<NbNeighb >;

using BigMap = std::map<BigInt, BigInt>;

namespace fmt
{
    template <>
    struct formatter< Population >
    {
        template < typename ParseContext >
        constexpr auto parse(ParseContext& ctx)
        {
            return ctx.begin();
        }

        template < typename FormatContext >
        auto format(const Population& p, FormatContext& ctx)
        {
            format_to(ctx.begin(), "{:4}", p.offset);

            for (const auto& e : p.table)
            {
                format_to(ctx.begin(), "{}", e==FullSlot ? '#' : '.');
            }
            return ctx.begin();
        }
    };

    template <>
    struct formatter< BigInt >
    {
        template < typename ParseContext >
        constexpr auto parse(ParseContext& ctx)
        {
            return ctx.begin();
        }

        template < typename FormatContext >
        auto format(const BigInt& i, FormatContext& ctx)
        {    
            return format_to(ctx.begin(), "({}, {})", absl::Uint128High64(i), absl::Uint128Low64(i));
        }
    };


} // namespace fmt


int patternToInt(const Population& s, int offset)
{
    if (s.table.size() < offset+NbNeighb)
    {
        fmt::print(fmt("Error pattern: !!"));
        return -1;
    }
    int retVal = 0;
    for (int i = 0; i<NbNeighb; i++)
    {
        auto c = s.table[i+offset];
        retVal <<= 1;
        if (c == FullSlot)
        {
            retVal |= 1;
        }
    }
    return retVal;
}

int patternToInt(const std::string& s)
{
    if (s.length() != NbNeighb)
    {
        fmt::print(fmt("Error pattern: {}"), s);
        return -1;
    }
    int retVal = 0;
    for (auto c : s)
    {
        retVal <<= 1;
        if (c == '#')
        {
            retVal |= 1;
        }
    }
    return retVal;
}

Rules genRules(const std::vector<std::string>& lines)
{
    Rules rules = {EmptySlot};
    for (const auto& r : lines)
    {
        if (!r.empty() && (r[0]=='#' || r[0] == '.'))
        {
            const auto delim = absl::ByAnyChar("= <>");
            std::vector<std::string> splitText = absl::StrSplit(r, delim, absl::SkipEmpty());
            auto ruleInt = patternToInt(splitText[0]);
            auto ruleRes = splitText[1][0] == '#' ? FullSlot : EmptySlot;
            rules[ruleInt] = ruleRes;
        }
    }
    return rules;
}

Population genPop(const std::string& l)
{
    std::string popStr = l.substr(15);
    Population p;

    p.offset = 0;
    for (auto i : popStr)
    {
        p.table.push_back(i=='#' ? FullSlot : EmptySlot);
    }

    return p;
}

Population applyRules(const Rules& rules, const Population& p)
{
    // find first and last position of plant.
    int firstSlot = p.getFirstSlot();
    int firstSlotPos = p.offset + firstSlot;

    int lastSlot = p.getLastSlot();
    int lastSlotPos = p.offset + lastSlot;

    // create result with 5 more empty slot on each side.
    Population psrc;
    psrc.offset = firstSlotPos - (2*NbNeighb);
    psrc.table.resize(lastSlotPos-firstSlotPos + 4*NbNeighb);
    Population pres = psrc;
    p.copyInto(psrc);

    for (int i = 2; i < psrc.table.size()-2; i++)
    {
        int curPat = patternToInt(psrc, i-2);
        auto ruleRes = rules[curPat];
        pres.table[i] = ruleRes;
    }

    return pres;
}

BigInt computeGeneration(BigInt nbGen, const Population& p0, const Rules& rules);

BigInt computeGeneration(BigInt nbGen, const Population& p0, const Rules& rules)
{
    std::map<Population, BigInt> bigMap;
    bigMap[p0] = 0;

    // fmt::print(fmt("gen 00: {}\n"), p0);
    Population curPop = p0;
    BigInt gen = 0;
    for (; gen < nbGen; gen++)
    {
        Population pres = applyRules(rules, curPop);
        std::swap(pres, curPop);
        // fmt::print(fmt("gen {}\n{} = {}\n"), gen+1,curPop, curPop.getSum());
        if (bigMap.find(curPop) != bigMap.end())
        {
            break;
        }
        bigMap[curPop] = gen+1;
    }
    if (gen == nbGen)
    {
        return curPop.getSum();
    }

    auto cycleStart = bigMap[curPop];
    auto cycleEnd = gen+1;

    fmt::print("END!!!!!!!!!!!! {} -> {}\n", cycleStart, cycleEnd);
    auto prevPopulIter = bigMap.find(curPop);
    auto diffGen = cycleEnd-cycleStart;
    auto simPopulSum = computeGeneration(cycleStart + ((nbGen-cycleStart)%(diffGen)), p0, rules);

    auto diffOffset = (curPop.getFirstSlot() - prevPopulIter->first.getFirstSlot()) 
        + (curPop.offset - prevPopulIter->first.offset);
    auto divi = (nbGen-cycleStart)/diffGen;
    auto nbPlant = curPop.nbPlant();
    fmt::print("\n");
    fmt::print(fmt("first : {}\n"), prevPopulIter->first);
    fmt::print(fmt("now   : {}\n"), curPop);
    fmt::print(fmt("res: {} -- {} -- {} -- {} -- {}\n"), diffGen, simPopulSum, diffOffset, divi, nbPlant);
    return simPopulSum + divi * diffOffset * nbPlant;
}

BigInt computeGeneration(BigInt nbGen, const std::string& s)
{
    std::vector<std::string> lines = absl::StrSplit(s, '\n');
    auto startState = lines[0];
    Population p0 = genPop(startState);

    Rules rules = genRules(lines);

    return computeGeneration(nbGen, p0, rules);
}

int main(int, char**)
{
    constexpr auto src1 = "initial state: #..#.#..##......###...###\n\n...## => #\n..#.. => #\n.#... => #\n.#.#. => #\n.#.## => #\n.##.. => #\n.#### => #\n#.#.# => #\n#.### => #\n##.#. => #\n##.## => #\n###.. => #\n###.# => #\n####. => #";
     expectTest(computeGeneration(20, src1), 325);
    

    constexpr auto src2 = "initial state: #...#..###.#.###.####.####.#..#.##..#..##..#.....#.#.#.##.#...###.#..##..#.##..###..#..##.#..##...\n\n...#. => #\n#..## => #\n..... => .\n##.## => .\n.##.. => #\n.##.# => .\n####. => #\n.#.#. => .\n..#.# => .\n.#.## => .\n.#..# => .\n##... => #\n#...# => #\n##### => .\n#.### => #\n..### => #\n###.. => .\n#.#.# => #\n##..# => #\n..#.. => #\n.#### => .\n#.##. => .\n....# => .\n...## => .\n#.... => .\n#..#. => .\n..##. => .\n###.# => #\n#.#.. => #\n##.#. => #\n.###. => .\n.#... => .";
     expectTest(computeGeneration(20, src2), 1787);


    expectTest(computeGeneration(1000, src2), 213);

    expectTest(computeGeneration(50000000000, src2), 107);

    fmt::print("Done\n");

    return 0;
}
