module InputParser where
import Data.Char
import System.Console.GetOpt
import System.Exit
import System.IO

import Lib

-- Program argument flag types
data Flag = JustPrint | StepOne | StepTwo deriving (Show, Eq)

-- The program options
flags =
    [Option ['i'] [] (NoArg JustPrint)
        "print out the loaded CFG"
    ,Option ['1'] [] (NoArg StepOne)
        "print out CFG after algo 4.1"
    ,Option ['2'] [] (NoArg StepTwo)
        "print out CFG after algo 4.3 or 4.1 if it generates empty language"]

-- Parser for the program options.
-- Returns a tuple of an option and a Maybe filename. If no filename was given,
-- returns a tuple of an option and a Nothing.
argParse :: [String] -> IO (Flag, String)
argParse argv =
    case getOpt Permute flags argv of
        ((o:[]), (n:[]), [])    ->
            return (o, n)  -- One arg and a file
        ((o:[]), [], [])    ->
            return (o, "")  -- One arg and load CFG from stdin
        (_, _, _)           -> do
            let hdr = "\nsimplify-bkg opt [input]\n where opt is one of:"
            ioError (userError (usageInfo hdr flags))

-- Get the input from Handle `hdl` and construct a Cfg record.
collectInput :: Handle -> IO Cfg
collectInput hdl = do
    line <- hGetLine hdl
    let nterm = charsByComma line
    if not (all isUpper nterm) then
        error "Non-terminals must be uppercase letters"
    else
        return ()

    line <- hGetLine hdl
    let term = charsByComma line
    if not (all isLower term) then
        error "Terminals must be lowercase letters"
    else
        return ()

    line <- hGetLine hdl
    if null line then
        error "No starting symbol"
    else
        if not (null (tail line)) || not (isUpper (head line)) then
            error "Starting symbol is either not uppercase or is not a single character"
        else
            return ()
    let start = head line

    rls <- getRules hdl (term ++ nterm)
    let rules = tuplifyRules rls

    if not (start `elem` nterm) then
        error "The starting symbol is not one of the given non-terminals"
    else
        return ()

    return (Cfg nterm term start rules)

-- Takes a String of Chars separated by commas and returns the same String
-- without the commas (i.e. "f,o,o" -> "foo"). Multiple commas are fine,
-- but multiple Chars without commas between each of them results in an error.
charsByComma :: String -> String
charsByComma "" = []
charsByComma (',':"") = []
charsByComma (s:"") = s : []
charsByComma (s:ss)
    | s == ','          = charsByComma ss
    | head ss /= ','    = error "Char not succeeded by comma"
    | otherwise         = s : charsByComma ss

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

-- Check validity of the left hand side of a rule
validLRule :: Char -> Bool
validLRule nterm = isUpper nterm

-- Check validity of the right hand side of a rule
validRRule :: String -> String -> Bool
validRRule ('-':'>':'#':[]) _       = True
validRRule ('-':'>':[]) _           = False
validRRule ('-':'>':rest) symbols   = inIterSet rest symbols
validRRule _ _                      = False

-- Check if an input String is a valid CFG rule
validRule :: String -> String -> Bool
validRule (x:xs) symbols = validLRule x && validRRule xs symbols
validRule _ _ = False

-- A loop to get all the CFG rules from Handle `hdl`.
-- Finishes parsing if an empty line or EOF is encountered.
getRules :: Handle -> String -> IO [String]
getRules hdl symbols = do
    ineof <- hIsEOF hdl
    if ineof then
        return []
    else do
        line <- hGetLine hdl
        if validRule line symbols then
            case filterRule line of
                Nothing -> return []
                Just aString -> do
                    nextLine <- getRules hdl symbols
                    return (aString : nextLine)
        else
            error "Invalid CFG rule encountered"

-- Returns Nothing on empty input string or a stripped down version of
-- a rule - just the terminal and nonterminal symbols without
-- the arrow ("->") symbol.
filterRule :: String -> Maybe String
filterRule "" = Nothing
filterRule line = Just (filter (\x -> isAlpha x || x == '#') line)