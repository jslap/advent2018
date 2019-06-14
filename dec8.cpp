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
#include "absl/types/span.h"

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

struct Node 
{
    std::vector<Node> childs;
    std::vector<int>  metadata;
};

Node parse(absl::Span<int>& sp)
{
    int nbChild = sp[0];
    int nbData = sp[1];
    Node newNode;
    newNode.childs.resize(nbChild);
    sp.remove_prefix(2);
    for (auto& c : newNode.childs)
    {
        c = parse(sp);
    }
    newNode.metadata.resize(nbData);
    for (auto& d : newNode.metadata)
    {
        d = sp.front();
        sp.remove_prefix(1);
    }
    return newNode;
}

Node parseTree(const std::string& s)
{
    auto fields = absl::StrSplit(s, ' ');
    std::vector<int> dataArray;
    for (auto a : fields)
    {
        int curVal;
        absl::SimpleAtoi(a, &curVal);
        dataArray.push_back(curVal);
    }

    auto fullArraySpan = absl::MakeSpan(dataArray);
    Node rootNode = parse(fullArraySpan);

    return rootNode;
}

int sumNode(const Node& n)
{
    int mySum = std::accumulate(n.metadata.begin(), n.metadata.end(), 0);

    int childSum = std::accumulate(n.childs.begin(), n.childs.end(), 0, [](int prev, const Node& n){
        return prev + sumNode(n);
    });
    return mySum + childSum;
}


int sumNode2(const Node& n)
{
    if (n.childs.empty())
    {
        return std::accumulate(n.metadata.begin(), n.metadata.end(), 0);
    }
    else
    {
        return std::accumulate(n.metadata.begin(), n.metadata.end(), 0, [&n](int prev, int meta){
            int nodeIndex = meta-1;
            int result = prev;
            if (nodeIndex >= 0 && nodeIndex < n.childs.size())
            {
                result += sumNode2(n.childs[nodeIndex]);
            }
            return result;
        });
    }
    int mySum = std::accumulate(n.metadata.begin(), n.metadata.end(), 0);

    int childSum = std::accumulate(n.childs.begin(), n.childs.end(), 0, [](int prev, const Node& n){
        return prev + sumNode(n);
    });
    return mySum + childSum;
}

int parseSumNodes(const std::string& s)
{
    return sumNode(parseTree(s));
}

int parseSumNodes2(const std::string& s)
{
    return sumNode2(parseTree(s));
}

int main(int, char**)
{
    auto src1 = "2 3 0 3 10 11 12 1 1 0 1 99 2 1 1 2";
    auto res1 = parseSumNodes(src1);
    expectTest(res1, 138);
    auto res12 = parseSumNodes2(src1);
    expectTest(res12, 66);

    std::ifstream ifs("dec8.txt");
    std::string content((std::istreambuf_iterator< char >(ifs)),
        (std::istreambuf_iterator< char >()));
    auto res2 = parseSumNodes(content);
    expectTest(res2, 49426);
    auto res22 = parseSumNodes2(content);
    expectTest(res22, 10);


    fmt::print("Done\n");
    return 0;
}
