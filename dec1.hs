import Test.Hspec

import Data.String.Utils
import qualified Data.Set as Set
import Data.Maybe
import Data.Ord
import Data.List

import Debug.Trace

strToInt :: String -> Int
strToInt = read.replace "+" ""

diffList :: String -> [Int]
diffList = map strToInt.lines

freqCompute :: String -> Int
freqCompute = sum.diffList


freqList :: String -> [Int]
freqList s = scanl (+) 0 (cycle(diffList s))


dup :: Ord a => [a] -> Maybe a
dup xs = dup' xs Set.empty
    where 
        dup' [] _ = Nothing
        dup' (x:xs) s = if Set.member x s 
                            then Just x
                            else dup' xs (Set.insert x s)

firstDupFreq' :: String -> Int
firstDupFreq' = fromJust.dup.freqList

enumerate :: [Int] -> [(Int, Int)]
enumerate = zip [0..]

show' l = unwords (map show l)

firstDupFreq'' :: String -> Int
firstDupFreq'' s 
    | totalSum == 0 = (fromJust.dup.freqList) s
    | otherwise = 
        -- (trace (show' (filter isJust aa))) 
        -- (trace (show' (enumerate firstPass)))
        (fst.(minimumBy (comparing  snd)).(map fromJust).(filter isJust)) aa
    where
        firstPass :: [Int] 
        firstPass = scanl (+) 0 (diffList s)
        totalSum :: Int
        totalSum = last firstPass
        cycleSize :: Int
        cycleSize = length firstPass
        aa :: [Maybe (Int, Int)]
        aa = [  findRepeatElem si sj | si <- enumerate firstPass, sj <- enumerate firstPass]
        findRepeatElem :: (Int, Int) -> (Int, Int) -> Maybe (Int, Int)
        findRepeatElem (i,si)  (j,sj)
            | i == j = Nothing
            | i==0 && j==cycleSize-1 = Nothing
            | j==0 && i==cycleSize-1 = Nothing
            | divCycle >= 0 && (sj-si) `mod` totalSum == 0 = 
                -- trace (show "elt: " ++ show (i,j,si,sj, divCycle, Just (divCycle*totalSum + si, divCycle*cycleSize + i)))
                Just (divCycle*totalSum + si, divCycle*cycleSize + i)
            | otherwise = Nothing
            where divCycle = (sj-si) `div` totalSum

firstDupFreq = firstDupFreq'

spec :: Spec
spec = 
    describe "Sample tests" $ 
        it "runs" $ do
            freqCompute ""  `shouldBe` 0
            freqCompute "0"  `shouldBe` 0
            freqCompute "+1"  `shouldBe` 1
            freqCompute "-2"  `shouldBe` -2
            freqCompute "-2\n+1"  `shouldBe` -1
            do
                s <- readFile "dec1.txt"
                freqCompute s `shouldBe` 425
            firstDupFreq "+1\n -1" `shouldBe` 0
            firstDupFreq "+3\n +3\n +4\n -2\n -4" `shouldBe` 10
            firstDupFreq "-6\n +3\n +8\n +5\n -6" `shouldBe` 5
            firstDupFreq "+7\n +7\n -2\n -7\n -4" `shouldBe` 14
            do
                s <- readFile "dec1.txt"
                firstDupFreq s `shouldBe` 57538

main = hspec spec
