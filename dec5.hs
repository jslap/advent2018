import Test.Hspec

import Data.List
import Data.Ord
import Data.Char
-- import Data.List.Split
-- import Data.Maybe
-- import Control.Arrow
import Data.Time

import Debug.Trace

isCombo :: Char -> Char -> Bool
isCombo c1 c2 = (toLower c1 == toLower c2) && (c1 /= c2)

minimizePolyOnce :: String -> String
minimizePolyOnce (x:y:xs) 
    | isCombo x y = minimizePolyOnce xs
    | otherwise = x : minimizePolyOnce (y:xs)
minimizePolyOnce s = s


minimizePoly :: String -> String
minimizePoly s 
    | s == next = s
    | otherwise = trace (show(length s)) minimizePoly next
    where next = minimizePolyOnce s

minimizePolySize :: String -> Int
minimizePolySize = length.minimizePoly

spec :: Spec
spec = 
    describe "Sample tests" $ 
        it "runs" $ do
            minimizePolySize "dabAcCaCBAcCcaDA"  `shouldBe` 10

            do
                s <- readFile "dec5.txt"
                minimizePolySize s `shouldBe` 85296
    
main = hspec spec
