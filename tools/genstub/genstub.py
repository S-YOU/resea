#!/usr/bin/env python3
import argparse
import sys
from antlr4 import *
from parser.idlLexer import idlLexer
from parser.idlParser import idlParser
from parser.idlListener import idlListener

class Listener(idlListener):
    def enterService(self, ctx):
        print(dir))
    def exitService(self, ctx):
        print(ctx)
    def enterCallDef(self, ctx):
        print(ctx)
    def enterArgList(self, ctx):
        print(ctx)
    def enterTypeDef(self, ctx):
        print(ctx)

def main(argv):
    argparser = argparse.ArgumentParser()
    argparser.add_argument('idl')
    args = argparser.parse_args()

    lexer = idlLexer(FileStream(args.idl))
    stream = CommonTokenStream(lexer)
    parser = idlParser(stream)
    tree = parser.services()
    walker = ParseTreeWalker()
    walker.walk(Listener(), tree)

if __name__ == '__main__':
    main(sys)