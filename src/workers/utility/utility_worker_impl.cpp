#include "utility_worker_impl.h"

#include <iostream>
#include <chrono>
#include <cmath>      // Math. functions needed for whets.cpp?
#include <stdarg.h>
#include <cstring>
#include "exprtk.hpp"

std::string Utility_Worker_Impl::TimeOfDayString(){
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return std::to_string(time.count() / 1000.0);
}

double Utility_Worker_Impl::TimeOfDay(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
}

double Utility_Worker_Impl::EvaluateComplexity(const char* complexity, va_list args){
    char varlist[50];

    //Pull variables from expression
    std::vector<char> variable_list = ProcessVarList(complexity);

    //construct symbol table
    exprtk::symbol_table<double> symbol_table;

    for(auto var : variable_list){
        if(var != ' '){
            double temp = va_arg(args, double);
            symbol_table.create_variable(std::string(1, var), temp);
        }
    }
    //Add infinity, pi and epsilon
    symbol_table.add_constants();

    //parse and compile symbol table and expression
    exprtk::expression<double> expression;
    expression.register_symbol_table(symbol_table);
    exprtk::parser<double> parser;
    parser.compile(complexity, expression);

    return expression.value();
}

std::vector<char> Utility_Worker_Impl::ProcessVarList(const char* complexity){
    char * cptr;
    char varlist[50];
    char mathchar[] = "()-+*/^0123456789";
    char mathfunc[24][9] = {"abs", "acos", "asin", "atan", "atan2", "cos", "cosh",
                            "exp", "log", "log10", "sin", "sinh", "sqrt", "tan", "tanh",
                            "floor", "round", "min", "max", "sig", "log2", "epsilon", "pi", "infinity" };
    //remove all math functions from complexity algorithm
    strcpy(varlist, complexity);
    for(unsigned int i = 0; i < 24; ++i){
        cptr = varlist;
        while ((cptr=strstr(cptr,mathfunc[i])) != NULL)
        memmove(cptr, cptr+strlen(mathfunc[i]), strlen(cptr+strlen(mathfunc[i]))+1);
    }
    //remove all math characters from varlist
    for(unsigned int i = 0; i < strlen(mathchar); ++i){
        cptr = varlist;
        while((cptr=strchr(cptr ,mathchar[i])) != NULL)
        memmove(cptr, cptr+1, strlen(cptr+1)+1);
    }
    //remove duplicate variables
    char * prev = varlist;
    while((*prev) != '\0'){
        cptr = prev + 1;
        while((cptr=strchr(cptr, (*prev))) != NULL){
            memmove(cptr, cptr+1, strlen(cptr+1)+1);
        }
        prev += 1;
    }
    std::string temp(varlist);
    std::vector<char> vec(temp.begin(), temp.end());
    return vec;
}
