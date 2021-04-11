{-
 - Library with context-free grammar specific functions and types.
 - Author: Patrik Nemeth, xnemet04
 -}
module Lib where

import System.IO

-- Record of a context-free grammar
data Cfg = Cfg {
    nterm :: [Char],    -- List of non-terminal symbols.
    term :: [Char],     -- List of terminal symbols.
    start :: Char,      -- The starting symbol.
    rules :: [(Char, String)] -- List of rules of the CFG as (symbol,[symbol]).
} deriving (Show)

-- Print a string with a comma inserted between each character
-- and a newline at the end. If an empty string is supplied,
-- print just a newline. (i.e. "hello" -> "h,e,l,l,o\n"
printStringWithCommas :: [Char] -> IO ()
printStringWithCommas []     = putChar '\n'
printStringWithCommas (x:[]) = do
    putChar x
    putChar '\n'
printStringWithCommas (x:xs) = do
    putChar x
    putChar ','
    printStringWithCommas xs

-- A printing function for the correct output format of the CFG rules.
-- (i.e. A->ab)
printCfgRules :: [(Char, String)] -> IO ()
printCfgRules []     = return ()
printCfgRules (x:xs) = do
    putChar (fst x)
    putStrLn ("->" ++ (snd x))
    printCfgRules xs

-- Print a CFG in the correct format.
printCfg :: Cfg -> IO ()
printCfg cfg = do
    printStringWithCommas (nterm cfg)
    printStringWithCommas (term cfg)
    putChar (start cfg)
    putChar '\n'
    printCfgRules (rules cfg)

-- Checks if every element of `alpha` is contained in
-- the `set` string. The input `set` is implicitly extended to include
-- an empty string as well ('#') - hence the iter(ation).
inIterSet :: String -> String -> Bool
inIterSet alpha set = all (\x -> x `elem` iterSet) alpha
    where iterSet = set ++ "#"

-- Uses `inIterSet` to check the same thing, only over an array of `alphas`.
checkAlphas :: [String] -> String -> Bool
checkAlphas [] _                 = False
checkAlphas (alpha : alphas) set = inIterSet alpha set || checkAlphas alphas set

-- NOT A COMPREHENSIVE CHECK - SEE NOTE BELOW
-- Checks if a CFG generates an empty language by checking if the starting CFG
-- symbol is on the left side of any of the CFG rules.
-- NOTE: As this function is used only after the first step of the algorithm
-- (when calling the program with option `-2`), this form of checking for an empty
-- language should be sufficient.
emptyLanguage :: Cfg -> Bool
emptyLanguage cfg = isInLRules (start cfg) (rules cfg)

-- Checks if a non-terminal nterm is in the rules on the left side.
-- The second argument is an array of tuplified rules.
isInLRules :: Char -> [(Char, String)] -> Bool
isInLRules _ []                = False
isInLRules nterm (rule : rest) =
    (nterm == fst rule) || isInLRules nterm rest