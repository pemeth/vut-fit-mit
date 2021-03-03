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

