from pyparsing import *
import argparse
import os
import sys

def parse_token(s, loc, tokens):
    lineno_ = lineno(loc, s)
    colno_ = col(loc, s)
    print(s,loc,tokens, lineno_, colno_)

def parse_file(file):
    # Read the input data
    TEXT     = file.read()
    print(TEXT)

    # Define a simple grammar for the text, multiply the first col by 2
    identifier = Word(alphas, alphas+nums+"_$").setParseAction(parse_token)
    point = Literal( "." )
    fnumber = Combine( Word( "+-"+nums, nums ) +
                   Optional( point + Optional( Word( nums ) ) ))
    newline = White("\n")
    lpar  = Literal( "(" )
    rpar  = Literal( ")" )
    lcro  = Literal( "[" )
    rcro  = Literal( "]" )
    lbra  = Literal( "{" )
    rbra  = Literal( "}" )
    pp_if  = Literal( "#if" )
    pp_ifdef  = Literal( "#ifdef" )
    pp_else  = Literal( "#else" )
    pp_endif  = Literal( "#endif" )
    pp_unknown  = Literal( "#" )
    semicolon  = Literal( ";" )
    elements = identifier | lpar | rpar | lcro | rcro | lbra | rbra | semicolon | pp_if | pp_ifdef | pp_endif | pp_else | pp_unknown | fnumber
    blocks   = ZeroOrMore(elements) + newline
    grammar  = OneOrMore(blocks)

    # Parse the results
    result = grammar.parseString( TEXT )
    # This gives a list of lists
    # [['cat', 6], ['dog', 10], ['foo', 14]]




parser = argparse.ArgumentParser(description='Parse CPP files.')
parser.add_argument('files',
                    metavar='FILES',
                    type=argparse.FileType('r'),
                    nargs='+',
                    help='files to open')
args = parser.parse_args()
print(args.files)

for file in args.files:
    parse_file(file)
