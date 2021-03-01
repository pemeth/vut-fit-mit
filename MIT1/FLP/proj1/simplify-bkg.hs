{-
 - FLP project "simplify-bkg"
 - Author: Patrik Nemeth, xnemet04
 -}


-- Record of a context-free grammar
data Cfg = Cfg {
    nterm :: [Char],
    term :: [Char],
    start :: Char,
    rules :: [String]
} deriving (Show)

