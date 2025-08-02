/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include <cstdlib>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    //todo
    std::string opt;
    int lineNumber;
    opt=scanner.nextToken();
    TokenType typ=scanner.getTokenType(opt);
    if(typ==NUMBER){
        // this line has a line number
        try{
            lineNumber=stringToInteger(opt);
        }
        catch(...){
            error("INVALID NUMBER");
        }
        if(!scanner.hasMoreTokens()){
            // deleting line
            if(!program.hasLine(lineNumber)){
                // line does not exist
                //error("SYNTAX ERROR");
            }
            else{
                // line exists, so we delete it
                program.removeSourceLine(lineNumber);
            }
        }
        else{
            // this is a line with codes
            Statement* stmt=parseStatement(scanner,line,program);
            if(!program.hasLine(lineNumber)){
                // line does not exist, so we add a new line
                program.addSourceLine(lineNumber,line);
                program.setParsedStatement(lineNumber,stmt);
            }
            else{
                // line exists, so we replace it
                program.removeSourceLine(lineNumber);
                program.addSourceLine(lineNumber,line);
                program.setParsedStatement(lineNumber,stmt);
            }
        }
    }
    else{
        // this line has no line number
        // it can only be a command or a directly executed statement
        if(opt=="RUN"){
            program.run(state);
        }
        else if(opt=="LIST"){
            program.list();
        }
        else if(opt=="CLEAR"){
            program.clear();
            state.Clear();
        }
        else if(opt=="QUIT"){
            program.clear();
            state.Clear();
            exit(0);
        }
        else if(opt=="HELP"){
            // some help messages here
            //todo
        }
        else{
            scanner.saveToken(opt);
            Statement* stmt=parseInstantStatement(scanner,program);
            stmt->execute(state,program);
            delete stmt;
        }
    }
}

/*
to compile:
g++ -std=c++17 -g Basic.cpp evalstate.cpp exp.cpp parser.cpp program.cpp statement.cpp Utils/error.cpp Utils/strlib.cpp Utils/tokenScanner.cpp -o Basic

8.1 15:14 TODO: DEBUG FIBONACCI!!!
8.1 22:34 TODO: SOLVE MEMORY LEAK!!!

*/