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


struct Attribute{
    ATTRIBUTE_TYPE type;
    std::string name;
    std::string value;
    std::vector<std::string> s;
    int i;
    double d;

    Attribute(){};
    Attribute(ATTRIBUTE_TYPE type_i, std::string name_i){
        this->type = type_i;
        this->name = name_i;
    }
    
    std::string get_string(){
        if(type == AT_STRING && s.size() == 1){
            return s[0];
        }
        return std::string();
    };

    std::string& string(){
        if(type == AT_STRING && s.size() == 1){
            return s[0];
        }
        return value;
    };
    void set_string(std::string string){
        if(type == AT_STRING){
            if(s.size() == 0){
                s.push_back(string);
            }else{
                s[0] = string;
            }
        }
    }
};

/*
Attribute* construct_attribute(std::string name, ATTRIBUTE_TYPE type){
    Attribute* attr = new Attribute();
    attr->name = name;
    attr->type = type;
    return attr;
};*/

#endif //ACTIVATABLE_H