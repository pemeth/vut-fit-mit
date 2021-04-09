{-
 - FLP project "simplify-bkg"
 - Author: Patrik Nemeth, xnemet04
 -}

import Data.Char
import Data.List
import System.Environment
import System.Console.GetOpt
import System.Exit
import System.IO

import InputParser
import Lib
import Simplifier

main = do
    (opt, file) <- getArgs >>= argParse

    cfg <- if null file then
            -- No file specified, take input from stdin
            collectInput stdin
        else
            -- Take input from file
            withFile file ReadMode collectInput

    if opt == JustPrint then do
        -- Option '-i'
        printCfg cfg
        exitWith ExitSuccess
    else
        return () -- do nothing

    let nt = makeSetNt "" (nterm cfg) (term cfg) (rules cfg)

    -- Step 2 of algorithm 4.3 from TIN
    cfgBarG <- getBarG cfg nt

    if opt == StepOne then do
        -- Option '-1'
        printCfg cfgBarG
        exitWith ExitSuccess
    else
        return () --do nothing

    -- The rest is reached only with option '-2'

    if not ((start cfg) `elem` nt) then do
        -- Empty language -> print cfgBarG and exit
        printCfg cfgBarG
        exitWith ExitSuccess
    else
        return ()

    -- Step 3 of algorithm 4.3 from TIN
    let v = makeSetV [(start cfgBarG)] (rules cfgBarG)

    cfgG' <- getG' cfgBarG v
    printCfg cfgG'
