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

#include <fmt/format.h>


#include "absl/strings/numbers.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"
#include "absl/types/span.h"
#include "absl/numeric/int128.h"

using BigInt = absl::uint128;

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

struct Game
{
    Game()
    {
        circle.push_back(0);
        cur = circle.begin();
    }

    void advance()
    {
        cur++;
        if (cur == circle.end())
            cur = circle.begin();
    }

    void back()
    {
        if (cur == circle.begin())
            cur = circle.end();
        cur--;
    }
    int play23()
    {
        back();
        back();
        back();
        back();
        back();
        back();
        back();
        int retVal = *cur;
        cur = circle.erase(cur);
        return retVal;
    }
    int play(int m)
    {
        if (m % 23 == 0)
        {
            return m + play23();
        }
        else
        {
            advance();
            advance();
            cur = circle.insert(cur, m);
            return 0;
        }
    }

    std::list< int > circle;
    std::list< int >::iterator cur;
};

namespace fmt
{
    template <>
    struct formatter< Game >
    {
        template < typename ParseContext >
        constexpr auto parse(ParseContext& ctx)
        {
            return ctx.begin();
        }

        template < typename FormatContext >
        auto format(const Game& g, FormatContext& ctx)
        {
            for (const auto& e : g.circle)
            {
                if (&e == &(*g.cur))
                    format_to(ctx.begin(), "({}) ", e);
                else
                    format_to(ctx.begin(), "{} ", e);
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

BigInt computeScore(int nbPlayer, int nbMarble)
{

    Game g;
    std::vector< BigInt > playeScore(nbPlayer, 0);
    std::size_t curPlayer = 0;
    for (int curMarble = 1; curMarble <= nbMarble; curMarble++)
    {
        // fmt::print("{}: {}\n", curMarble, g);
        playeScore[curPlayer] += g.play(curMarble);
        curPlayer = (curPlayer + 1) % nbPlayer;
    }

    return *std::max_element(playeScore.begin(), playeScore.end());
}

int main(int, char**)
{
    expectTest(computeScore(9, 25), 32);
    expectTest(computeScore(10, 1618), 8317);
    expectTest(computeScore(13, 7999), 146373);

    expectTest(computeScore(17, 1104), 2764);
    expectTest(computeScore(21, 6111), 54718);
    expectTest(computeScore(30, 5807), 37305);

    expectTest(computeScore(486, 70833), 373597);
    expectTest(computeScore(486, 7083300), 333);

// 10 players; last marble is worth 1618 points: high score is 8317
// 13 players; last marble is worth 7999 points: high score is 146373

// 17 players; last marble is worth 1104 points: high score is 2764
// 21 players; last marble is worth 6111 points: high score is 54718
// 30 players; last marble is worth 5807 points: high score is 37305

    fmt::print("Done\n");
    return 0;
}
