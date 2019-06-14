#include <fstream>
#include <iostream>

#include <algorithm>
#include <cwctype>
#include <deque>
#include <locale>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#define FMT_STRING_ALIAS 1
#include <fmt/format.h>


#include "absl/numeric/int128.h"
#include "absl/strings/str_split.h"

#include <range/v3/all.hpp>

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

int nbOk = 0;
int nbFail = 0;
int nbTotal = 0;
template < class T1, class T2 >
void expectTest(const T1& got, const T2& expected)
{
    nbTotal++;
    if (got == expected)
    {
        nbOk++;
        fmt::print("Test OK");
    }
    else
    {
        nbFail++;
        fmt::print("Test Fail: got {} should be {}", got, expected);
    }
    std::cout << std::endl;
}
void printResult()
{
    fmt::print(fmt("End: Total nb of test: {}\n"), nbTotal);
    fmt::print(fmt("     Total nb of Pass: {}\n"), nbOk);
    fmt::print(fmt("     Total nb of Fail: {}\n"), nbFail);
}

std::string fromFile(const std::string& fileName)
{

    std::ifstream ifs(fileName);
    std::string srcStr((std::istreambuf_iterator< char >(ifs)),
        (std::istreambuf_iterator< char >()));
    return srcStr;
}

class State
{
    public:
    State() = default;
    State(std::string const & s)
    {

    }
    std::array<int , 4> registers;
};

class InOuts
{
    public:
    InOuts() = default;
    InOuts(int a, int b, int c): ma{a}, mb{b}, mc{c} {}
    InOuts(std::string const & s)
    {

    }

    int ma;
    int mb;
    int mc;

};

class Operation
{
    public:
    Operation(InOuts const & io): mio{io} {}
    virtual State operator()(State const & s) = 0;

    private:
    InOuts mio;
};

class AssemblyLine
{
    public:
    AssemblyLine(std::string const & s)
    {

    }

    int mopcode;
    InOuts mio;
};

int processFile(std::string const & s)
{
    std::vector<std::string> lines = absl::StrSplit(s, '\n');

    std::array<std::string, 3> Separator{{"","",""}};
    auto endPhase1 = std::search(lines.begin(), lines.end(), Separator.begin(), Separator.end());

    std::vector<std::string> phase1{lines.begin(), endPhase1};
    std::vector<std::string> phase2{endPhase1, lines.end()};
    phase1.erase(std::remove_if(phase1.begin(), phase1.end(), [](std::string const & o){return o.empty();}));
    phase2.erase(std::remove_if(phase2.begin(), phase2.end(), [](std::string const & o){return o.empty();}));

    return phase2.size();
}

int main()
{
    expectTest(42, 42);

    expectTest(processFile(fromFile("dec16.txt")), 0);


    printResult();
    fmt::print("Done\n");

    return 0;
}