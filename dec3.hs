import Test.Hspec

import Data.List
import Data.List.Split
import Data.Maybe
import Control.Arrow

import Debug.Trace

type Offset = (Int, Int)
type Size = (Int, Int)
type Rect = (Offset, Size)
type Claim = (Int, Rect)

type Fabric = [[Int]]

lineToClaim :: String -> Claim
lineToClaim s = claim
    where 
        splitted = splitOneOf "#@,:x" s
        intList = map read (filter (not . null) splitted)
        (cid:ox:oy:w:h:_) = intList
        claim = (cid, ((ox,oy),(w,h)))

getSize :: [Claim] -> Size
getSize = foldl maxWith (0, 0) 
    where 
        maxWith (w, h) (_, ((ox,oy),(rw,rh))) = (max w (ox+rw), max h (oy+rh))

applyClaim :: Fabric -> Claim -> Fabric
applyClaim = doWork
    where
        doWork :: Fabric -> Claim -> Fabric
        doWork f (_, ((ox,0),(rw,0))) = f 
        doWork (r:rs) (cid, ((ox,0),(rw,rh))) = applyRow ox rw r : doWork rs (cid, ((ox,0),(rw,rh-1)))
                where 
                    applyRow :: Int -> Int -> [Int] -> [Int]
                    applyRow _ 0 r = r
                    applyRow 0 w (x:xs) = x+1 : applyRow 0 (w-1) xs
                    applyRow ox w (x:xs) = x : applyRow (ox-1) w xs
        doWork (r:rs) (cid, ((ox,oy),(rw,rh))) =               r : doWork rs (cid, ((ox,oy-1),(rw,rh)))

countOverlap :: Fabric -> Int
countOverlap f = sum $ map sum processed
    where
        processed :: Fabric 
        processed = map (map doProc) f
        doProc :: Int -> Int
        doProc x = if x >1 then 1 else 0

strToClaimList :: String -> [Claim]
strToClaimList s = map lineToClaim (lines s)

claimListStrToFab :: String -> Fabric
claimListStrToFab = claimListToFab.strToClaimList

claimListToFab :: [Claim] -> Fabric
claimListToFab claimList = foldl applyClaim fabStart claimList
    where 
        claimsSize = getSize claimList
        fabw = fst claimsSize
        fabh = snd claimsSize
        fabStart :: Fabric
        fabStart = replicate fabh (replicate fabw 0)

getOverlap :: String -> Int
getOverlap = countOverlap.claimListStrToFab

extractChild :: Int -> Int -> [a] -> [a]
extractChild ox sx l = take sx (drop ox l)

getClaimSubFab :: Fabric -> Claim -> Fabric
getClaimSubFab f (cid, ((ox,oy),(rw,rh))) = map (extractChild ox rw) (extractChild oy rh f)

fabIsIntact :: Fabric -> Bool
fabIsIntact = all rowIsIntact
    where 
        rowIsIntact = all (<= 1)

findIntactClaim :: String -> Int      
findIntactClaim s = (fst.fst.head.filter (fabIsIntact.snd)) (zip claimList claimSubfabList)
    where 
        claimList = strToClaimList s
        fab = claimListToFab claimList
        claimSubfabList :: [Fabric]
        claimSubfabList = map (getClaimSubFab fab) claimList

spec :: Spec
spec = 
    describe "Sample tests" $ 
        it "runs" $ do
            getOverlap "#1 @ 1,3: 4x4\n#2 @ 3,1: 4x4\n#3 @ 5,5: 2x2"  `shouldBe` 4
            findIntactClaim "#1 @ 1,3: 4x4\n#2 @ 3,1: 4x4\n#3 @ 5,5: 2x2"  `shouldBe` 3
            

            do
                s <- readFile "dec3.txt"
                getOverlap s `shouldBe` 96569
                findIntactClaim s `shouldBe` 33
    
main = hspec spec
