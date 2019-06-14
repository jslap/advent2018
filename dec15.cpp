#include <fstream>
#include <iostream>

#include <algorithm>
#include <cwctype>
#include <locale>
#include <numeric>
#include <deque>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

#define FMT_STRING_ALIAS 1
#include <fmt/format.h>


#include "absl/strings/str_split.h"
#include "absl/numeric/int128.h"

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

using MapElt = std::string::value_type;
constexpr MapElt Elf = 'E';
constexpr MapElt Gob = 'G';
// constexpr MapElt Wall = '#';
constexpr MapElt Empty = '.';

bool isPerson(MapElt c)
{
    return c == Elf || c == Gob;
}



struct Pos
{
    constexpr Pos(int _x, int _y): x(_x), y(_y) {}
    bool operator ==(const Pos& rhs) const {return x==rhs.x && y==rhs.y;}
    int x = 0;
    int y = 0;
};

bool posSort(const Pos& p1, const Pos& p2)
{
    return (p1.y == p2.y) ? p1.x < p2.x : p1.y < p2.y;
}

template <class T>
class MapT : public std::vector< std::vector<T>>
{
    public:
        const T& operator()(const Pos& p) const {return (*this)[p.y][p.x];}
              T& operator()(const Pos& p)       {return (*this)[p.y][p.x];}
};
using Map = MapT<MapElt>;

std::vector<Pos> getMapRange(const Pos& p, const Map& m)
{
    Pos arr[] = {Pos(p.x, p.y-1), Pos(p.x-1, p.y), Pos(p.x+1, p.y), Pos(p.x, p.y+1)};
    auto res = arr | ranges::view::remove_if([&m](const Pos& p){
        return !(p.x >= 0 && p.y >= 0 && p.y < m.size() && p.x < m.back().size());
    });
    return res;
}

std::vector<Pos> getMapRange(const Map& m)
{
    const auto xRange = ranges::view::iota(0) | ranges::view::take_exactly(m.back().size());
    const auto yRange = ranges::view::iota(0) | ranges::view::take_exactly(m.size());

    auto res = ranges::view::cartesian_product(xRange, yRange) | 
            ranges::view::transform([](const std::tuple<int, int> &t){return Pos(std::get<0>(t), std::get<1>(t));});

    return res;
}

constexpr int NotVisited = -1;
constexpr int Objective = -2;

struct Person
{
    std::optional<Pos> closestEnemyMove(const Map& m)
    {
        auto closeTarget = closestTargetPos(m);
        if (closeTarget)
        {
            MapT< int > distMap;
            for (auto l : m)
            {
                distMap.emplace_back(l.size(), NotVisited);
            }

            for (auto p : getMapRange(pos, m))
            {
                if (m(p) == Empty)
                    distMap(p) = Objective;
            }

            return closestTargetPos(*closeTarget, m, distMap);
        }
        return {};
    }

    std::optional<Pos> closestTargetPos(const Map& m)
    {
        MapT< int > distMap;
        for (auto l : m)
        {
            distMap.emplace_back(l.size(), NotVisited);
        }

        for (const Pos& p0 : getMapRange(m))
        {
            const auto & cur = m(p0);
            if (isPerson(cur) && cur != type)
            {
                for (auto p : getMapRange(p0, m))
                {
                    if (m(p) == Empty)
                        distMap(p) = Objective;
                }
            }
        }

        return closestTargetPos(pos, m, distMap);
    }

    static std::optional<Pos> closestTargetPos(const Pos& start, const Map& m, const MapT<int>& objMap)
    {
        //prepare distance map;
        auto distMap = objMap;
        
        int curDist = 0;
        if (distMap(start) == Objective)
            return start;
        
        distMap(start) = curDist;

        int nbExpand = 1;
        std::vector< Pos > reachedEnemies;
        while (nbExpand > 0)
        {
            nbExpand = 0;
            for (const Pos& p0 : getMapRange(m))
            {
                if (distMap(p0) == curDist)
                {
                    for (auto p : getMapRange(p0, m))
                    {
                        auto &curM = m(p);
                        auto &curDistMap = distMap(p);
                        if (curM == Empty && curDistMap < 0)
                        {
                            if (curDistMap == Objective)
                                reachedEnemies.push_back(p);
                            curDistMap = curDist+1;
                            nbExpand++;
                        }
                    }
                }
            }
            curDist++;
            if (!reachedEnemies.empty())
            {
                std::sort(reachedEnemies.begin(), reachedEnemies.end() , &posSort);
                return reachedEnemies.front();
            } 
        }
        
        return {};
    }

    MapElt type = ' ';
    int  attack = 0;
    int hp = 0;
    Pos pos = {-1, -1};
    bool hasPlayed = false;
};

bool posPersonSort(const Person& p1, const Person& p2)
{
    return posSort(p1.pos, p2.pos);
}

struct MapState
{
    MapState(const std::string& s)
    {
        auto lines = absl::StrSplit(s, '\n');
        for (const auto& line : lines)
        {
            if (!line.empty())
            {
                m.push_back({});
                for (auto c: line)
                    m.back().emplace_back(c);
            }
        }

        for (auto p : getMapRange(m))
        {
            auto curElt = m(p);
            if (isPerson(curElt))
            {
                Person pers;
                pers.attack = 3;
                pers.hp = 200;
                pers.type = curElt;
                pers.pos = p;
                persons.push_back(pers);
            }
        }
    }

    void print() const
    {
        fmt::print(fmt("After {} round:\n"), curRound);
        for (int y = 0 ; y < m.size() ; y++)
        {
            for (auto c : m[y])
                fmt::print(fmt("{}"), c);
            auto personsCopy = persons;
            sortPersons(personsCopy);

            std::vector<std::string> personStrRng = personsCopy
                | ranges::view::remove_if([y](const Person& p){return p.hp <=0 || p.pos.y != y;})
                | ranges::view::transform([](const Person& p){return fmt::format(fmt("{}({})"), p.type, p.hp);});
            
            if (!personStrRng.empty())
            {
                fmt::print(fmt("   {}"), fmt::join(personStrRng, ", "));
            }
            fmt::print("\n");
        }
    }

    int getTotalHP() const
    {
        return ranges::accumulate(persons, 0, ranges::plus{}, &Person::hp);
    }
    void sortPersons()
    {
        sortPersons(persons);
    }

    void sortPersons(std::vector< Person >& pl) const
    {
        std::sort(pl.begin(), pl.end(), &posPersonSort);
    }

    std::optional<Pos> enemyInRange(const Person& person)
    {
        std::vector<Pos> enemyPossible;
        for (auto neighPos : getMapRange(person.pos, m))
        {
            const auto & cur = m(neighPos);
            if (isPerson(cur) && cur != person.type)
                enemyPossible.push_back(neighPos);
        }
        if (enemyPossible.empty())
            return {};
        if (enemyPossible.size() == 1)
            return enemyPossible.front();

        std::vector<Person> enemyList = enemyPossible | 
            ranges::view::transform([this](const Pos& needlePos) -> std::optional<Person>{
                return findpersonOpt(needlePos);
                }) |
            ranges::view::filter([](const std::optional<Person>& p) -> bool {return p.has_value();}) |
            ranges::view::transform([](const std::optional<Person>& p) -> Person {return *p;});
        ranges::action::sort(enemyList, [](const Person& p1, const Person& p2){
            return p1.hp==p2.hp ? posPersonSort(p1,p2) : p1.hp<p2.hp; 
            });
        return enemyList.front().pos;
    }

    auto alives()
    {
        return ranges::view::filter(persons, [](const Person& p){return p.hp > 0;});
    }

    auto findperson(const Pos& p)
    {
        return alives() | ranges::view::filter([p](const Person& pers){return pers.pos == p;});
    }

    Person& findpersonRef(const Pos& p)
    {
        auto foundRng = findperson(p);
        if (ranges::distance(foundRng) != 1)
            throw std::runtime_error(fmt::format(fmt("yo {}"), ranges::distance(foundRng)));

        return foundRng.front();
    }
    std::optional<Person> findpersonOpt(const Pos& p)
    {
        auto foundRng = findperson(p);
        if (ranges::distance(foundRng) != 1)
            return {};

        return foundRng.front();
    }

    void playPerson(Person& p)
    {
        if (p.hp <= 0)
            return;
        
        if (!enemyInRange(p))
        {
            // Plan move
            auto bestMove = p.closestEnemyMove(m);  
            if (bestMove)
            {
                m(p.pos) = Empty;
                m(*bestMove) = p.type;
                p.pos = *bestMove;
            }
            
        }

        auto curEnemy = enemyInRange(p); 
        if (curEnemy)
        {
            Person& enemy = findpersonRef(*curEnemy);

            enemy.hp -= p.attack;
            if (enemy.hp <= 0)
            {
                enemy.hp = 0;
                m(enemy.pos) = Empty;
            }
            // do attack
            // fmt::print("Attack");
        }
        p.hasPlayed = true;
    }
    
    int nbElf() const
    {
        return nbOf(Elf);
    }
    int nbGob() const
    {
        return nbOf(Gob);
    }
    int nbOf(MapElt elt) const
    {
        return std::count_if(persons.begin(), persons.end(), [elt](const Person& p){return p.hp > 0 && p.type == elt;});
    }

    bool roundIsCompleted() const
    {
        auto unplayedRange = persons | 
            ranges::view::filter([](const Person& p){return p.hp > 0;}) |
            ranges::view::filter([](const Person& p){return !p.hasPlayed;});
        return unplayedRange.empty();
    }

    void resetPlayed()
    {
        for (auto& p: persons)
            p.hasPlayed = false;
    }

    bool ongoing() const
    {
        return nbElf()>0 && nbGob() > 0;
    }
    
    void playAll()
    {
        bool isOnGoing = ongoing();
        while (isOnGoing)
        {
            // print();
            sortPersons();
            resetPlayed();
            for (auto& p: persons)
            {
                isOnGoing = ongoing();
                if (!isOnGoing)
                    break;
                playPerson(p);
            }
            if (roundIsCompleted())
                curRound++;
        }
        // print();
    }

    int getOutcome() const
    {
        return curRound * getTotalHP();   
    }

    int curRound = 0;
    Map m;
    std::vector< Person > persons;
};

int getOutcomeElfWin(const std::string& s)
{
    MapState ms(s);
    int nbElfStart = ms.nbElf();

    for (int curAttack = 3; true; curAttack++)
    {
        MapState curMap = ms;
        for (Person& p : curMap.persons)
        {
            if (p.type == Elf)
                p.attack = curAttack;
        }

        curMap.playAll();
        if (curMap.nbElf() == nbElfStart)
        {
            fmt::print(fmt("Found with attack={} at round {}\n"), curAttack, curMap.curRound-1);
            curMap.print();
            return curMap.getOutcome();
        }
    }

    return 1;
}


int getOutcome(const std::string& s)
{
    MapState ms(s);

    ms.playAll();

    return ms.getOutcome();
}

int main(int, char**)
{

    std::cout << u"Hello" L"World";
    std::cout << u"Hello" U"World";

   expectTest(getOutcome(fromFile("dec15_1.txt")), 27730);
   expectTest(getOutcome(fromFile("dec15_2.txt")), 36334);
   expectTest(getOutcome(fromFile("dec15_3.txt")), 39514);
   expectTest(getOutcome(fromFile("dec15_4.txt")), 27755);
   expectTest(getOutcome(fromFile("dec15_5.txt")), 28944);
   expectTest(getOutcome(fromFile("dec15_6.txt")), 18740);
    expectTest(getOutcome(fromFile("dec15_7.txt")), 182376);


    expectTest(getOutcomeElfWin(fromFile("dec15_1.txt")), 4988);

   expectTest(getOutcomeElfWin(fromFile("dec15_3.txt")), 31284);
   expectTest(getOutcomeElfWin(fromFile("dec15_4.txt")), 3478);
   expectTest(getOutcomeElfWin(fromFile("dec15_5.txt")), 6474);
   expectTest(getOutcomeElfWin(fromFile("dec15_6.txt")), 1140);
   expectTest(getOutcomeElfWin(fromFile("dec15_7.txt")), 42);

    printResult();
    fmt::print("Done\n");

    return 0;
}
