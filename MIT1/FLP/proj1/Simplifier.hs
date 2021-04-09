module Simplifier where

import Data.Char
import Data.List

import Lib

-- Checks if a non-terminal nterm is in the rules on the left side.
-- The second argument is an array of tuplified rules.
isInLRules :: Char -> [(Char, String)] -> Bool
isInLRules _ [] = False
isInLRules nterm (rule : rest) =
    (nterm == fst rule) || isInLRules nterm rest

-- Returns an array of the right sides of rules for a specific non-terminal.
getRRulesOfNterm :: Char -> [(Char, String)] -> [String]
getRRulesOfNterm _ [] = []
getRRulesOfNterm nterm (rule : rules)
    | nterm == fst rule = (snd rule) : (getRRulesOfNterm nterm rules)
    | otherwise         = getRRulesOfNterm nterm rules

-- Step (2) of algorithm 4.1 from TIN scripts
buildNextNi :: String -> String -> String -> [(Char, String)] -> String
buildNextNi _ [] _ _ = []
buildNextNi ni (ntA : ntAs) terms rules
    | addToNi   =
        let currNi = ni ++ (ntA : []) in
            ntA : (buildNextNi currNi ntAs terms rules)
    | otherwise = buildNextNi ni ntAs terms rules
    where
        addToNi = checkAlphas (getRRulesOfNterm ntA rules) (terms ++ ni)

-- Builds the 'Nt' set according to algorithm 4.1 from TIN.
makeSetNt :: String -> String -> String -> [(Char, String)] -> String
makeSetNt ni nterm term rules
    | ni' /= ni = nub (ni' ++ (makeSetNt ni' nterm term rules))
    | otherwise = []
    where
        ni' = buildNextNi ni nterm term rules

-- Builds an iteration of the 'V' (so technically builds Vi)
-- set for algorithm 4.2.
buildNextVi :: String -> [(Char,String)] -> String
buildNextVi _ [] = []
buildNextVi vi (rule : rules)
    | (fst rule) `elem` vi  =
        let currVi = nub (snd rule ++ vi) in
            currVi ++ (buildNextVi currVi rules)
    | otherwise             = buildNextVi vi rules

-- Builds the 'V' set based on algorithm 4.2 from TIN.
-- Takes an initialization value of Vi (the start symbol of the CFG), which
-- is an array of one non-terminal symbol. Returns the 'V' set.
makeSetV :: String -> [(Char,String)] -> String
makeSetV "" _ = error "Algorithm needs an initialization value"
makeSetV vi rules
    | vi' /= vi = nub (vi' ++ (makeSetV vi' rules))
    | otherwise = vi
    where
        -- Filter out empty strings (#)
        vi' = filter (\x -> isAlpha x) (buildNextVi vi rules)

-- Create the "bar G" CFG as per algorithm 4.3, step 2 from TIN.
-- Called `barG` because of the bar above the G.
getBarG :: Monad m => Cfg -> String -> m Cfg
getBarG cfg nt = do
    let rulesOld = rules cfg
    let rulesNew' = filter (\x -> fst x `elem` nt) rulesOld
    let rulesNew = filter (\x -> inIterSet (snd x) (nt ++ (term cfg))) rulesNew'

    return (Cfg nt (term cfg) (start cfg) rulesNew)

-- Create the "G'" CFG as per algorithm 4.3.
getG' :: Monad m => Cfg -> [Char] -> m Cfg
getG' cfg v = do
    let ntermNew = nub (intersect v (nterm cfg))
    let termNew = nub (intersect v (term cfg))
    let rulesOld = rules cfg
    let rulesNew' = filter (\x -> fst x `elem` ntermNew) rulesOld
    let rulesNew = filter (\x -> inIterSet (snd x) v) rulesNew'

    return (Cfg ntermNew termNew (start cfg) rulesNew)