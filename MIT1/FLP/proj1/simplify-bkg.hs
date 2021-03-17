{-
 - FLP project "simplify-bkg"
 - Author: Patrik Nemeth, xnemet04
 -}

import Data.Char
import Data.List
import System.Environment
import System.Console.GetOpt

-- Record of a context-free grammar
data Cfg = Cfg {
    nterm :: [Char],
    term :: [Char],
    start :: Char,
    rules :: [(Char, String)]
} deriving (Show)

-- Returns Nothing on empty input string or a stripped down version of
-- a rule - just the terminal and nonterminal symbols without
-- the arrow ("->") symbol.
filterRule :: String -> Maybe String
filterRule "" = Nothing
filterRule line = Just (filter (\x -> isAlpha x || x == '#') line)

-- A loop to get all the CFG rules from stdin.
-- TODO rework this, so that it does not exit at empty line, but at EOF
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

-- Checks if every element of `alpha` is contained in
-- the `set` string. The input `set` is implicitly extended to include
-- an empty string as well ('#') - hence the iter(ation).
-- MAY BE USEFUL FOR ALGORITHM 4.1 (2) (the second part of the conjunction).
inIterSet :: String -> String -> Bool
inIterSet alpha set = all (\x -> x `elem` iterSet) alpha
    where iterSet = set ++ "#"

-- Uses `inIterSet` to check the same thing, only over an array of `alphas`.
checkAlphas [] _ = False
checkAlphas (alpha : alphas) set = inIterSet alpha set || checkAlphas alphas set

-- Checks if a non-terminal nterm is in the rules on the left side.
-- The second argument is an array of tuplified rules.
isInLRules :: Char -> [(Char, String)] -> Bool
isInLRules _ [] = False
isInLRules nterm (rule : rest) =
    (nterm == fst rule) || isInLRules nterm rest

-- Returns an array of the right sides of rules for a specific non-terminal.
getRRulesOfNterm :: Char -> [(Char, String)] -> [String]
getRRulesOfNterm _ [] = []
getRRulesOfNterm nterm (rule : rules) =
    if (nterm == fst rule) then
        (snd rule) : (getRRulesOfNterm nterm rules)
    else
        getRRulesOfNterm nterm rules

-- Step (2) of algorithm 4.1 from TIN scripts
buildNextNi _ [] _ _ = []
buildNextNi ni (ntA : ntAs) terms rules =
    if checkAlphas (getRRulesOfNterm ntA rules) (terms ++ ni) then
        let currNi = ni ++ (ntA : []) in ntA : (buildNextNi currNi ntAs terms rules)
    else
        buildNextNi ni ntAs terms rules

-- Builds the 'Nt' set according to algorithm 4.1 from TIN.
makeNt ni nterm term rules =
    if ni' /= ni
    then
        nub (ni' ++ (makeNt ni' nterm term rules))
    else
        []
    where ni' = buildNextNi ni nterm term rules

-- Take a String, pull out words and reduce it back to String
reduceToString = concat . words

-- Get the input from stdin and construct a Cfg record.
collectInput = do
    line <- getLine
    let nterm = reduceToString line

    line <- getLine
    let term = reduceToString line

    line <- getLine
    let start = head line

    rls <- getRules
    let rules = tuplifyRules rls

    return (Cfg nterm term start rules)

-- Program argument flag types
data Flag = Info | StepOne | StepTwo deriving Show

-- The program options
flags =
    [Option ['i'] [] (NoArg Info)
        "print out the loaded CFG"
    ,Option ['1'] [] (NoArg StepOne)
        "print out CFG after algo 4.1"
    ,Option ['2'] [] (NoArg StepTwo)
        "print out CFG after algo 4.3 or 4.1 if it generates empty language"]

-- Parser for the program options
argParse argv =
    case getOpt Permute flags argv of
        ((o:[]),n:[],[])->
            return (Just o, Just n)  -- One arg and a file
        ((o:[]),[],[])  ->
            return (Just o, Nothing)  -- One arg and load CFG from stdin
        (_,_,_)         -> do
            let hdr = "\nsimplify-bkg opt [input]\n where opt is one of:"
            ioError (userError (usageInfo hdr flags))

main = do
    (opt, file) <- getArgs >>= argParse
    -- TODO if nonOpts is empty, take input from stdin, otherwise from
    -- specified file in nonOpts

    cfg <- collectInput

    let nt = makeNt "" (nterm cfg) (term cfg) (rules cfg)

