/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"

Program::Program(){
    jumped=0;
    currentLine=0;
    finishedFlag=0;
    restrictedNames.insert("REM");
    restrictedNames.insert("LET");
    restrictedNames.insert("PRINT");
    restrictedNames.insert("INPUT");
    restrictedNames.insert("END");
    restrictedNames.insert("GOTO");
    restrictedNames.insert("IF");
    restrictedNames.insert("THEN");
    restrictedNames.insert("RUN");
    restrictedNames.insert("LIST");
    restrictedNames.insert("CLEAR");
    restrictedNames.insert("QUIT");
    restrictedNames.insert("HELP");
}

Program::~Program(){
    clear();
}

void Program::clear() {
    // Replace this stub with your own code
    //todo
    for(std::map<int,std::pair<std::string,Statement*> >::iterator it=lines.begin();it!=lines.end();it++){
        if(it->second.second!=nullptr){
            delete it->second.second;
        }
    }
    lines.clear();
    lineNumbers.clear();
    restrictedNames.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    // Replace this stub with your own code
    //todo
    if(lines.count(lineNumber)){
        if(lines[lineNumber].second!=nullptr){
            delete lines[lineNumber].second;
        }
        lines[lineNumber].first=line;
    }
    else{
        lines[lineNumber]=std::make_pair(line,nullptr);
        lineNumbers.insert(lineNumber);
    }
}

void Program::removeSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    if(lines.count(lineNumber)){
        if(lines[lineNumber].second!=nullptr){
            delete lines[lineNumber].second;
        }
        lines.erase(lineNumber);
        lineNumbers.erase(lineNumber);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    if(lines.count(lineNumber)){
        return lines[lineNumber].first;
    }
    else{
        return "";
    }
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Replace this stub with your own code
    //todo
    if(lines.count(lineNumber)){
        if(lines[lineNumber].second!=nullptr){
            delete lines[lineNumber].second;
        }
        lines[lineNumber].second=stmt;
    }
    else{
        error("Line number does not exist");
    }
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
    // Replace this stub with your own code
    //todo
    if(lines.count(lineNumber)){
        return lines[lineNumber].second;
    }
    else{
        return nullptr;
    }
}

int Program::getFirstLineNumber() {
    // Replace this stub with your own code
    //todo
    if(lineNumbers.empty()){
        return -1;
    }
    else{
        return *lineNumbers.begin();
    }
}

int Program::getNextLineNumber(int lineNumber) {
    // Replace this stub with your own code
    //todo
    std::set<int>::iterator it=lineNumbers.upper_bound(lineNumber);
    if(it==lineNumbers.end()){
        return -1;
    }
    else{
        return *it;
    }
}

bool Program::hasLine(int lineNumber)const{
    return bool(lines.count(lineNumber));
}

void Program::jumpToNextLine(){
    if(jumped){
        jumped=0;
        return;
    }
    int lineNumber=getNextLineNumber(currentLine);
    if(lineNumber==-1){
        finishedFlag=1;
        return;
    }
    currentLine=lineNumber;
}

void Program::list()const{
    for(std::set<int>::iterator it=lineNumbers.begin();it!=lineNumbers.end();it++){
        std::cout<<lines.at(*it).first<<std::endl;
    }
}

void Program::jumpTo(int lineNumber){
    jumped=1;
    currentLine=lineNumber;
}

int Program::getCurrentLine()const{
    return currentLine;
}

void Program::endProgram(){
    finishedFlag=1;
}

void Program::run(EvalState& state){
    currentLine=getFirstLineNumber();
    finishedFlag=0;
    while(!finishedFlag){
        /*
        if(lines[currentLine].second==nullptr){
            std::cerr<<"fatal: nullpointer"<<std::endl;
        }
        std::cerr<<lines[currentLine].first<<std::endl;
        std::cerr<<"executing line "<<currentLine<<std::endl;
        */
        lines[currentLine].second->execute(state,*this);
        //std::cerr<<"execution of line "<<currentLine<<" done"<<std::endl;
        jumpToNextLine();
    }
}

bool Program::isValidName(const std::string name){
    return !restrictedNames.count(name);
}

//more func to add
//todo


