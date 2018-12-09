
#include <iostream>
#include <fstream>

#include <string>
#include <cwctype>
#include <locale>
#include <algorithm>
#include <numeric>
#include <vector>

// #define FMT_STRING_ALIAS 1
#include <fmt/format.h>

// #include <range/v3/all.hpp>

bool isCombo(std::string::value_type c1, std::string::value_type c2)
{
    return (c1 != c2) && (std::tolower(c1, std::locale()) == std::tolower(c2, std::locale()));
}
constexpr auto SP = ' ';

std::string minimizePoly(std::string s)
{
    if (s.length() <= 1)
    {
        return s;
    }

    const auto send = s.end();
    const auto sbeg = s.begin();
    auto prev = s.begin();
    auto cur = s.begin();
    cur++;

    auto advIter = [&send](std::string::iterator& iter)
    {
        while (iter != send && *iter == SP)
        {
            iter++;
        }
    };

    auto backIter = [&sbeg](std::string::iterator& iter)
    {
        while (iter != sbeg && *iter == SP)
        {
            iter--;
        }
    };

    while (cur != s.end())
    {
        if (isCombo(*prev, *cur))
        {
            *prev = SP;
            *cur  = SP;
            backIter(prev);
            advIter(cur);
        }
        else
        {
            prev = cur;
            cur++;
            advIter(cur);
        }

    }

    s.erase(std::remove_if(s.begin(), s.end(), [](const auto& c){return std::isspace(c, std::locale());}), s.end());
    return s;
}

std::size_t findBestPoly(const std::string& s)
{
    constexpr std::size_t nbLetter = 'Z'-'A'+1;
    std::vector<std::string::value_type> alphaArray(nbLetter);
    std::iota(alphaArray.begin(), alphaArray.end(), 'A');
    std::vector<std::size_t> res(nbLetter);
    std::transform(alphaArray.begin(), alphaArray.end(), res.begin(), [&s](std::string::value_type c){
        std::string scopy = s;
        scopy.erase(std::remove_if(scopy.begin(), scopy.end(), [c](const auto& c1){return c==std::toupper(c1, std::locale());}), scopy.end());
        return minimizePoly(scopy).size();
    });

    return *std::min_element(res.begin(), res.end());
}

template <class T1, class T2>
void expectTest(const T1& expected, const T2& got)
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
    auto src1 = "dabAcCaCBAcCcaDA";
    auto res1 = minimizePoly(src1);
    expectTest(res1.size(), 10);

    std::ifstream ifs("dec5.txt");
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
    auto res2 = minimizePoly(content);
    expectTest(res2.size(), 10);

    auto res11 = findBestPoly(src1);
    expectTest(res11, 4);

    auto res12 = findBestPoly(content);
    expectTest(res12, 4);

    return 0;
}


