import Test.Hspec

import Data.List
import Data.Maybe
import Control.Arrow

-- (NbLetterRep, NbRep) We have NbRep times a repetion of NbLetterRep letters
type RepList = [(Int, Int)]

getRepList :: String -> RepList
getRepList = map ( head &&& length).group.sort.map length.group.sort

getNbFromRepList :: Int -> RepList -> Int
getNbFromRepList _ [] = 0
getNbFromRepList n ((nbl, nbr):xs)
    | n == nbl = nbr
    | otherwise = getNbFromRepList n xs

getNbRep :: Int -> String -> Int
getNbRep n s = getNbFromRepList n (getRepList s)
    
myClamp :: Int -> Int
myClamp = min 1

computeCheck :: String -> Int
computeCheck s = product $ map (\n -> sum (map (\f ->  myClamp(f n)) partialCheckFuncList)) [2,3] 
    where 
        repLists :: [RepList]
        repLists = (map getRepList.lines) s
        partialCheckFuncList :: [Int -> Int]
        partialCheckFuncList =  map (flip getNbFromRepList) repLists
        
nbDiff :: String -> String -> Int
nbDiff [] [] = 0
nbDiff (x:xs) (y:ys)
    | x == y = nbDiff xs ys
    | otherwise = 1 + nbDiff xs ys
nbDiff [] _ = 1000
nbDiff _ [] = 1000

getSimilarity :: String -> String -> String
getSimilarity [] [] = []
getSimilarity (x:xs) (y:ys)
    | x == y = x : getSimilarity xs ys
    | otherwise = getSimilarity xs ys

getSimilar :: String -> String
getSimilar s = (snd.fromJust.find ((==1) . fst)) pairs
    where 
        boxes = lines s
        pairs = [(nbDiff x y, getSimilarity x y) | x <- boxes, y <- boxes]

spec :: Spec
spec = 
    describe "Sample tests" $ 
        it "runs" $ do
            getNbRep 2 "abcdef"  `shouldBe` 0
            getNbRep 2 "bababc"  `shouldBe` 1
            getNbRep 2 "abbcde"  `shouldBe` 1
            getNbRep 2 "abcccd"  `shouldBe` 0
            getNbRep 2 "aabcdd"  `shouldBe` 2
            getNbRep 2 "abcdee"  `shouldBe` 1
            getNbRep 2 "ababab"  `shouldBe` 0

            getNbRep 3 "abcdef"  `shouldBe` 0
            getNbRep 3 "bababc"  `shouldBe` 1
            getNbRep 3 "abbcde"  `shouldBe` 0
            getNbRep 3 "abcccd"  `shouldBe` 1
            getNbRep 3 "aabcdd"  `shouldBe` 0
            getNbRep 3 "abcdee"  `shouldBe` 0
            getNbRep 3 "ababab"  `shouldBe` 2

            computeCheck "abcdef"  `shouldBe` 0
            computeCheck "bababc"  `shouldBe` 1
            computeCheck "abbcde"  `shouldBe` 0
            computeCheck "abcccd"  `shouldBe` 0
            computeCheck "aabcdd"  `shouldBe` 0
            computeCheck "abcdee"  `shouldBe` 0
            computeCheck "ababab"  `shouldBe` 0

            computeCheck "abcdef\nbababc\nabbcde\nabcccd\naabcdd\nabcdee\nababab"  `shouldBe` 12

            nbDiff "abcde" "axcye" `shouldBe` 2
            nbDiff "fghij" "fguij" `shouldBe` 1
            nbDiff "" "" `shouldBe` 0
            getSimilarity "abcde" "axcye" `shouldBe` "ace"
            getSimilarity "fghij" "fguij" `shouldBe` "fgij"
            getSimilarity "" "" `shouldBe` ""
            
            getSimilar "abcde\nfghij\nklmno\npqrst\nfguij\naxcye\nwvxyz" `shouldBe` "fgij"

            do
                s <- readFile "dec2.txt"
                computeCheck s `shouldBe` 7657
            do
                s <- readFile "dec2.txt"
                getSimilar s `shouldBe` "ivjhcadokeltwgsfsmqwrbnuy"
    
main = hspec spec
