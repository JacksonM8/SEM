#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <vector>
#include <map>

enum ATTRIBUTE_TYPE{
    AT_STRING = 0,
    AT_INTEGER = 1,
    AT_BOOLEAN = 2,
    AT_DOUBLE = 3,
    AT_STRINGLIST = 4
};

namespace{
    class Attribute;
}

struct Attribute{
    ATTRIBUTE_TYPE type;
    std::string name;
    std::string value;
    std::vector<std::string> s;
    int i;
    double d;
    
    std::string get_string(){
        if(type == AT_STRING && s.size() == 1){
            return s[0];
        }
        return std::string();
    };
};

#endif //ACTIVATABLE_H