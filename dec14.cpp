#include <fstream>
#include <iostream>

#include <algorithm>
#include <cwctype>
#include <list>
#include <locale>
#include <numeric>
#include <deque>
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

using BigInt = absl::uint128;

namespace fmt
{
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

struct State
{
    State(int r1, int r2): scores({r1, r2}) {} 

    void next()
    {
        auto next = scores[index1] + scores[index2];
        std::vector<int> toAdd;
        while (next > 0)
        {
            toAdd.push_back(next%10);
            next /= 10;
        }
        for (auto i = toAdd.rbegin(); i != toAdd.rend(); ++i)
        {
            scores.push_back(*i);
        }
        index1 = (index1 + 1 + scores[index1]) % scores.size();
        index2 = (index2 + 1 + scores[index2]) % scores.size();
    }

    int getNbRecipes() const
    {
        return scores.size();
    }
    
    std::string get10Val(int nbDay) const
    {
        std::string sum;
        for (int i = nbDay; i < nbDay+10; i ++)
        {
            sum += fmt::format(fmt("{}"), scores[i]);
        }
        return sum;
    }
    
    void print() const
    {
        for (int i = 0; i < scores.size(); i++)
        {
            if (i == index1)
                fmt::print(fmt("({})"), scores[i]);
            else if (i == index2)
                fmt::print(fmt("[{}]"), scores[i]);
            else
                fmt::print(fmt(" {} "), scores[i]);
        }
        fmt::print("\n");
    }

    std::deque<int> scores;
    int index1 = 0;
    int index2 = 1;
};

std::string get10Best(int r1, int r2, int nbDay)
{
    State s(r1, r2);
    while (s.getNbRecipes() < nbDay + 10)
    {
        s.next();
    //    s.print();
    }
    return s.get10Val(nbDay);
}

int main(int, char**)
{
    expectTest(get10Best(3, 7, 9),    "5158916779");
    expectTest(get10Best(3, 7, 5),    "0124515891");
    expectTest(get10Best(3, 7, 18),   "9251071085");
    expectTest(get10Best(3, 7, 2018), "5941429882");


    fmt::print("Done\n");

    return 0;
}
