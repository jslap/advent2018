import Test.Hspec

import Data.List
import Data.Ord
import Data.Char
-- import Data.List.Split
-- import Data.Maybe
-- import Control.Arrow
import Data.Time

import Debug.Trace


type TimeS = LocalTime
type Guard = Int

data GuardEventType = StartShift Guard | Fall | Wake deriving (Show)
type GuardEvent = (TimeS, GuardEventType)

type DaySched = (TimeS, (Guard, [Bool]))
type FullSched = [DaySched]

show' :: [Bool] -> String
show' = concatMap (\x -> if x then "#" else ".")

-- [1518-09-09 00:02] Guard #1283 begins shift
parseTimeS :: String -> TimeS
parseTimeS s = 
    -- trace ("parseTime : " ++  s)
    parseTimeOrError True defaultTimeLocale "[%Y-%m-%d %H:%M" s

parseGuard :: String -> Guard
parseGuard s = read subStrGuard
    where
        subStrGuard = (takeWhile isDigit . tail.dropWhile (/='#')  ) s

getEvent :: String -> GuardEvent
getEvent s = (parseTimeS timePart, ev s )
    where
        timePart = takeWhile (/=']') s
        ev s
            | "#" `isInfixOf` s     = StartShift (parseGuard s)
            | "wakes" `isInfixOf` s = Wake
            | otherwise             = Fall

getEvents :: String -> [GuardEvent]
getEvents s = map getEvent (lines s)

splitAtShift :: GuardEvent -> GuardEvent -> Bool
splitAtShift _ (_, StartShift x) = False
splitAtShift _ _ = True

shiftToDaySched :: [GuardEvent] -> DaySched
shiftToDaySched [] = undefined
shiftToDaySched ((t, StartShift g):xs) = (t, (g, getRest (replicate 60 False) xs))
    where
        getRest ::  [Bool] -> [GuardEvent] -> [Bool]
        getRest sched [] = sched
        getRest sched ( (t1, Fall) : (t2, Wake) : xs) = getRest newSched xs
            where 
                getMinute (LocalTime _ (TimeOfDay _ m _)) = m 
                minStart = getMinute t1
                minEnd = getMinute t2
                newSched = take minStart sched ++ replicate (minEnd-minStart) True ++ drop minEnd sched

hasSameGuard :: DaySched -> DaySched -> Bool
hasSameGuard e1 e2 = fst(snd e1) == fst(snd e2)

schedByGuard :: FullSched -> [[DaySched]]
schedByGuard fs = groupBy hasSameGuard $ sortOn (fst.snd) fs

guardList :: FullSched -> [Guard]
guardList = nub.sort.map (fst.snd)

getBigSleeper :: FullSched -> Guard
getBigSleeper fs = fst.last $ sortOn snd sleepByGuard
    where
        computeSleep :: FullSched -> (Guard, Int)
        computeSleep fullArr@((ts, (g, arr)):_) = (g, countSleep (map (snd.snd) fullArr))
        countSleep :: [[Bool]] -> Int
        countSleep  = sum.map (length.filter id)
        sleepByGuard = map computeSleep (schedByGuard fs)

mostSleptMinute :: FullSched -> Guard -> (Int, Int)
mostSleptMinute fs g = maximum (zip  sumSleepPerMinute [0..])
    where 
        guardSched = filter ( (== g).fst.snd  ) fs
        schedOnly = map (snd.snd) guardSched
        sumSleepPerMinute = map sum . transpose $ map (map fromEnum) schedOnly

getSched :: [GuardEvent] -> FullSched
getSched l = map shiftToDaySched groupedEvents
    where 
        sortedEvents = sortOn fst l
        groupedEvents = groupBy splitAtShift sortedEvents
        fullSched = map shiftToDaySched groupedEvents

getBestGuardSol :: String -> Int
getBestGuardSol s = bigSleeper * mostSlept
    where 
        sched = getSched $ getEvents s
        bigSleeper = getBigSleeper sched
        mostSlept = snd $ mostSleptMinute sched bigSleeper

zipMeWith :: [a] -> (a -> b) -> [(a,b)]
zipMeWith l f = zip l (map f l)

getBestGuardSol2 :: String -> Int
getBestGuardSol2 s = fst bestGuard*(snd.snd) bestGuard
    where 
        sched = getSched $ getEvents s
        gl = guardList sched
        gSleptMinute = zipMeWith gl (mostSleptMinute sched)
        bestGuard = maximumBy (comparing (fst.snd)) gSleptMinute
        

spec :: Spec
spec = 
    describe "Sample tests" $ 
        it "runs" $ do
            getBestGuardSol "[1518-11-01 00:00] Guard #10 begins shift\n[1518-11-01 00:05] falls asleep\n[1518-11-01 00:25] wakes up\n[1518-11-01 00:30] falls asleep\n[1518-11-01 00:55] wakes up\n[1518-11-01 23:58] Guard #99 begins shift\n[1518-11-02 00:40] falls asleep\n[1518-11-02 00:50] wakes up\n[1518-11-03 00:05] Guard #10 begins shift\n[1518-11-03 00:24] falls asleep\n[1518-11-03 00:29] wakes up\n[1518-11-04 00:02] Guard #99 begins shift\n[1518-11-04 00:36] falls asleep\n[1518-11-04 00:46] wakes up\n[1518-11-05 00:03] Guard #99 begins shift\n[1518-11-05 00:45] falls asleep\n[1518-11-05 00:55] wakes up"  `shouldBe` 240
            getBestGuardSol2 "[1518-11-01 00:00] Guard #10 begins shift\n[1518-11-01 00:05] falls asleep\n[1518-11-01 00:25] wakes up\n[1518-11-01 00:30] falls asleep\n[1518-11-01 00:55] wakes up\n[1518-11-01 23:58] Guard #99 begins shift\n[1518-11-02 00:40] falls asleep\n[1518-11-02 00:50] wakes up\n[1518-11-03 00:05] Guard #10 begins shift\n[1518-11-03 00:24] falls asleep\n[1518-11-03 00:29] wakes up\n[1518-11-04 00:02] Guard #99 begins shift\n[1518-11-04 00:36] falls asleep\n[1518-11-04 00:46] wakes up\n[1518-11-05 00:03] Guard #99 begins shift\n[1518-11-05 00:45] falls asleep\n[1518-11-05 00:55] wakes up"  `shouldBe` 4455

            do
                s <- readFile "dec4.txt"
                getBestGuardSol s `shouldBe` 85296
                getBestGuardSol2 s `shouldBe` 1
    
main = hspec spec
