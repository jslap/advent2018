#include <string>
#include <iostream>
#include <sstream> 


namespace Format
{

// Support int, float , then enum.
// Firts, use tuples, and parse str run-time. no format specifier. No positional arg.
// Then, parse str at compile time.
// allow  format specifier.
// Validate tuple with format specief
// Custom types... ?

class FormatSpec
{
    public:
        template <class StrType>
        FormatSpec(const StrType& s): mFmtSpecStr(s) {}
        FormatSpec() {}

        const std::string mFmtSpecStr;
};

namespace 
{
const std::string::value_type FormatSpecStart{'{'};
const std::string::value_type FormatSpecEnd{'}'};
struct FormatSpecResult
{
    bool mFound;
    FormatSpec mFormatSpec;
    std::size_t mStartOffset;
    std::size_t mEndOffset;
};

template <class StrType>
FormatSpecResult findNextSpec(StrType const & s, std::size_t offset)
{
    std::string sCopy{s}; // TODO optimize this
    auto foundPos = offset;
    foundPos = sCopy.find(FormatSpecStart, foundPos);
    while (foundPos != std::string::npos)
    {
        bool doubleCurlyFound = (foundPos != sCopy.length()-1) && (sCopy[foundPos+1] == FormatSpecStart);
        if (doubleCurlyFound) 
        {
            foundPos = sCopy.find(FormatSpecStart, foundPos+1);
        }
        else
        {
            auto foundEndFormat = sCopy.find(FormatSpecEnd, foundPos+1);
            if (foundEndFormat == std::string::npos)
            {
                throw std::invalid_argument("no end format delimiter");
            }
            return {true, FormatSpec(), foundPos, foundEndFormat};
        }        
    }
    return {false, FormatSpec(), std::string::npos};
}

} // namespace



template <class T>
std::string convertWithSpec(const FormatSpec& /*formatSpec*/, const T& val)
{
    std::ostringstream os;
    os << val;
    return os.str();
}

template <class... Ts> struct FormatPack {};

template <class T, class... Ts>
struct FormatPack<T, Ts...> : FormatPack<Ts...> 
{
    FormatPack(T t, Ts... ts) : FormatPack<Ts...>(ts...), tail(t) {}

    FormatPack<Ts...> const& rest() const {return *this;}

    T tail;
};

template <class StrType, class ... Ts>
std::string convertFormatPack(StrType const & fmtString, std::size_t offset, FormatPack<Ts ...> const & fmtPack);

template <class StrType, class T, class ... Ts>
std::string convertFormatPack(StrType const & fmtString, std::size_t offset, FormatPack<T, Ts ...> const & fmtPack)
{
    auto foundSpec = findNextSpec(fmtString, offset);
    if (!foundSpec.mFound)
    {
        throw std::invalid_argument("Need a format specifier since we have an argument");
    }
    
    auto bef = std::string(fmtString).substr(offset, foundSpec.mStartOffset);
    auto cur = convertWithSpec(foundSpec.mFormatSpec, fmtPack.tail);
    auto rem = convertFormatPack(fmtString, foundSpec.mEndOffset+1, fmtPack.rest());
    return bef + cur + rem;
}


template <class StrType>
std::string convertFormatPack(StrType const & fmtString, std::size_t offset, FormatPack<> const & fmtPack)
{
    auto foundSpec = findNextSpec(fmtString, offset);
    if (foundSpec.mFound)
    {
        throw std::invalid_argument("Should not have a format specifier since we do not have an argument");
    }
    return std::string(fmtString).substr(offset);
}

template <class StrType, class... Ts>
class FormatStr
{
    public:
    FormatStr(const StrType &s, Ts ... ts):
        mFormatString(s),
        mPack(ts ...)
        {}

    std::string operator()() 
    {
        return convertFormatPack(mFormatString, 0, mPack);
    }


    const std::string mFormatString;
    FormatPack<Ts ...> mPack;
};

} // Format

using namespace Format;
int main()
{
    // FormatStr f{"Allo {} again.", 42.0};
    // FormatStr f2{"Allo You!"};
    // std::cout << f() << std::endl << f2() << std::endl;
    std::cout << FormatStr("Allo {} and {} again.", 34.0, 33.0)() << std::endl;
    std::cout << FormatStr("Allo {} and {} again.", "What!!!!", '?')() << std::endl;
    return 1;
}
