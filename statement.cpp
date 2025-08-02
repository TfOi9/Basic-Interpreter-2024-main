/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

//todo
int stringToInt(std::string str){
    return stringToInteger(str);
}

LetStatement::LetStatement(TokenScanner& scanner, Program &program){
    var=scanner.nextToken();
    //std::cerr<<var<<std::endl;
    //std::cerr<<program.isValidName(var)<<std::endl;
    if(!program.isValidName(var)){
        error("SYNTAX ERROR");
    }
    //std::cerr<<var<<std::endl;
    try{
        scanner.verifyToken("=");
    }
    catch(...){
        error("SYNTAX ERROR");
    }
    try{
        exp=parseExp(scanner);
    }
    catch(...){
        error("SYNTAX ERROR");
    }
    if(scanner.hasMoreTokens()){
        error("SYNTAX ERROR");
    }
}

LetStatement::~LetStatement(){
    delete exp;
}

void LetStatement::execute(EvalState &state, Program &program){
    int val=exp->eval(state);
    state.setValue(var,val);
}

PrintStatement::PrintStatement(TokenScanner& scanner){
    exp=parseExp(scanner);
    if(scanner.hasMoreTokens()){
        error("SYNTAX ERROR");
    }
}

PrintStatement::~PrintStatement(){
    delete exp;
}

void PrintStatement::execute(EvalState &state, Program &program){
    std::cout<<exp->eval(state)<<std::endl;
}

InputStatement::InputStatement(TokenScanner& scanner, Program &program){
    var=scanner.nextToken();
    if(!program.isValidName(var)){
        error("SYNTAX ERROR");
    }
    if(scanner.hasMoreTokens()){
        error("SYNTAX ERROR");
    }
}

void InputStatement::execute(EvalState &state, Program &program){
    std::string str;
    REGET:
    std::cout<<" ? ";
    std::getline(std::cin,str);
    int val;
    try{
        val=stringToInteger(str);
    }
    catch(...){
        //error("INVALID NUMBER");
        std::cout<<"INVALID NUMBER"<<std::endl;
        goto REGET;
    }
    /*
    if(!state.isDefined(var)){
        error("VARIABLE NOT DEFINED");
    }
    */
    state.setValue(var,val);
}

RemStatement::RemStatement(TokenScanner& scanner){
    // This function is intentionally left blank.
}

void RemStatement::execute(EvalState &state, Program &program){
    // This function is intentionally left blank.
    //std::cerr<<"executing rem statement"<<std::endl;
}

GotoStatement::GotoStatement(TokenScanner& scanner){
    std::string str=scanner.nextToken();
    try{
        toLine=stringToInteger(str);
    }
    catch(...){
        error("INVALID NUMBER");
    }
    if(scanner.hasMoreTokens()){
        error("SYNTAX ERROR");
    }
}

void GotoStatement::execute(EvalState &state, Program &program){
    if(!program.hasLine(toLine)){
        error("LINE NUMBER ERROR");
    }
    program.jumpTo(toLine);
}

IfStatement::IfStatement(TokenScanner& scanner,const std::string& _line){
    line=_line;
    int if_pos=-1,then_pos=-1;
    if_pos=findSubString(line,"IF");
    then_pos=findSubString(line,"THEN");
    int cmp_pos=-1,temp;
    temp=findChar(line,'=');
    if(temp!=-1){
        typ=0;
        cmp_pos=temp;
    }
    //std::cerr<<if_pos<<" "<<then_pos<<" "<<cmp_pos<<" "<<temp<<std::endl;
    temp=findChar(line,'>');
    if(temp!=-1){
        if(cmp_pos==-1){
            typ=1;
            cmp_pos=temp;
        }
        else{
            error("SYNTAX ERROR");
        }
    }
    //std::cerr<<cmp_pos<<" "<<temp<<std::endl;
    temp=findChar(line,'<');
    if(temp!=-1){
        if(cmp_pos==-1){
            typ=-1;
            cmp_pos=temp;
        }
        else{
            error("SYNTAX ERROR");
        }
    }
    //std::cerr<<cmp_pos<<" "<<temp<<std::endl;
    if(cmp_pos==-1||if_pos==-1||then_pos==-1){
        error("SYNTAX ERROR");
    }
    if(if_pos+2>cmp_pos-1||cmp_pos+1>then_pos-1||then_pos+4>line.size()-1){
        error("SYNTAX ERROR");
    }
    std::string strl,strr,strt;
    strl=cutString(line,if_pos+2,cmp_pos-1);
    strr=cutString(line,cmp_pos+1,then_pos-1);
    strt=cutString(line,then_pos+4,line.size()-1);
    TokenScanner scanl(strl),scanr(strr),scant(strt);
    scanl.ignoreWhitespace();
    scanr.ignoreWhitespace();
    scant.ignoreWhitespace();
    scant.scanNumbers();
    //std::cerr<<strl<<"/"<<strr<<"/"<<strt<<std::endl;
    try{
        expl=parseExp(scanl);
    }
    catch(...){
        error("SYNTAX ERROR");
    }
    //std::cerr<<"got expl"<<std::endl;
    try{
        expr=parseExp(scanr);
    }
    catch(...){
        error("SYNTAX ERROR");
    }
    std::string num=scant.nextToken();
    try{
        toLine=stringToInteger(num);
    }
    catch(...){
        error("INVALID NUMBER");
    }
    //std::cerr<<"got toLine"<<std::endl;
    if(scant.hasMoreTokens()){
        //std::cerr<<"there are more tokens"<<std::endl;
        error("SYNTAX ERROR");
    }
}

IfStatement::~IfStatement(){
    delete expl;
    delete expr;
}

void IfStatement::execute(EvalState &state, Program &program){
    int vall=expl->eval(state);
    int valr=expr->eval(state);
    bool satisfied=0;
    if(typ==0){
        if(vall==valr){
            satisfied=1;
        }
    }
    else if(typ==1){
        if(vall>valr){
            satisfied=1;
        }
    }
    else if(typ==-1){
        if(vall<valr){
            satisfied=1;
        }
    }
    if(satisfied){
        if(!program.hasLine(toLine)){
            error("LINE NUMBER ERROR");
        }
        else{
            program.jumpTo(toLine);
        }
    }
}

EndStatement::EndStatement(TokenScanner& scanner){
    if(scanner.hasMoreTokens()){
        error("SYNTAX ERROR");
    }
}

void EndStatement::execute(EvalState &state, Program &program){
    program.endProgram();
}

Statement* parseStatement(TokenScanner& scanner,const std::string& line,Program& program){
    std::string opt=scanner.nextToken();
    Statement* stmt=nullptr;
    if(opt=="REM"){
        stmt=new RemStatement(scanner);
    }
    else if(opt=="LET"){
        stmt=new LetStatement(scanner,program);
    }
    else if(opt=="PRINT"){
        stmt=new PrintStatement(scanner);
    }
    else if(opt=="INPUT"){
        stmt=new InputStatement(scanner,program);
    }
    else if(opt=="GOTO"){
        stmt=new GotoStatement(scanner);
    }
    else if(opt=="IF"){
        stmt=new IfStatement(scanner,line);
    }
    else if(opt=="END"){
        stmt=new EndStatement(scanner);
    }
    else{
        error("SYNTAX ERROR");
    }
    return stmt;
}
Statement* parseInstantStatement(TokenScanner& scanner,Program& program){
    std::string opt=scanner.nextToken();
    Statement* stmt=nullptr;
    if(opt=="LET"){
        stmt=new LetStatement(scanner,program);
    }
    else if(opt=="PRINT"){
        stmt=new PrintStatement(scanner);
    }
    else if(opt=="INPUT"){
        stmt=new InputStatement(scanner,program);
    }
    else{
        error("SYNTAX ERROR");
    }
    return stmt;
}