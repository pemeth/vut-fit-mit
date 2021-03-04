{-
 - FLP project "simplify-bkg"
 - Author: Patrik Nemeth, xnemet04
 -}

import Data.Char

-- Record of a context-free grammar
data Cfg = Cfg {
    nterm :: [Char],
    term :: [Char],
    start :: Char,
    rules :: [String]
} deriving (Show)

-- Returns Nothing on empty input string or a stripped down version of
-- a rule - just the terminal and nonterminal symbols without
-- the arrow ("->") symbol.
-- TODO Rework this, so that it returns a tuple of the two sides of a rule.
filterRule :: String -> Maybe String
filterRule "" = Nothing
filterRule line = Just (filter (\x -> isAlpha x || x == '#') line)

-- A loop to get all the CFG rules from stdin.
getRules :: IO [String]
getRules = do
    line <- getLine
    case filterRule line of
        Nothing -> return []
        Just aString -> do
            nextLine <- getRules
            return (aString : nextLine)

-- A helper function for tuplifyRules. It takes the first letter of a rule,
-- which is a non-terminal and separates it into a tuple for the left and
-- right sides of a rule.
tuplifyRules' :: String -> Maybe (Char, String)
tuplifyRules' "" = Nothing
tuplifyRules' (nterm : rest) = Just (nterm, rest)

-- Take a [String] of CFG rules and return a [(Char, String)] of the same
-- rules, only separated into tuples of left/right sides of the respective
-- rules.
tuplifyRules :: [String] -> [(Char, String)]
tuplifyRules [] = []
tuplifyRules (x : xs) =
    case tuplifyRules' x of
        Nothing -> []
        Just tuple -> tuple : (tuplifyRules xs)

main = do
    let cfg = Cfg "" "" ' ' []
    line <- getLine
    let nterm cfg = words line

    line <- getLine
    let term cfg = words line

    line <- getLine
    let start cfg = head line

    rls <- getRules
    let rules cfg = rls

    print (nterm cfg)
    print (term cfg)
    print (start cfg)
    print (rules cfg)

