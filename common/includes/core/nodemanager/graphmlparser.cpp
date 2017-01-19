#include "graphmlparser.h"
#include <pugixml.hpp>

#include <iostream>

GraphmlParser::GraphmlParser(const std::string filename){
    auto result = doc.load_file(filename.c_str());

    auto keys = doc.select_nodes("/graphml/key");
    for(auto key : keys){
        std::string name = key.node().attribute("attr.name").value();
        std::string id = key.node().attribute("id").value();

        attribute_map_[name] = id;
    }
}

std::vector<std::string> GraphmlParser::find_nodes(std::string kind, std::string parent_id){

    std::string search = ".//node";

    if(parent_id.length() > 0){
        search += "[@id='"+ parent_id + "']/";
    }
    search += "/data[@key='" + attribute_map_["kind"] + "' and .='" + kind +"']/..";
    std::vector<std::string> out;

    for(auto n : doc.select_nodes(search.c_str())){
        out.push_back(n.node().attribute("id").value());
    }
    return out;
}

std::vector<std::string> GraphmlParser::find_edges(std::string kind){
    std::string search = ".//edge[data[@key='" + 
                            attribute_map_["kind"] + "' and .='" + kind +"']]";

    std::vector<std::string> out;

    for(auto n : doc.select_nodes(search.c_str())){
        out.push_back(n.node().attribute("id").value());
    }
    return out;
}

std::string GraphmlParser::get_attribute(std::string id, std::string attribute_name){
    std::string search = ".//*[@id='" + id + "']";
    std::string out;
    try{
        auto res = doc.select_node(search.c_str());
        out = res.node().attribute(attribute_name.c_str()).value();
    } catch(...){}
    return out;
}

std::string GraphmlParser::get_data_value(std::string id, std::string key_name){
    std::string search = ".//*[@id='" + id + "']/data[@key='" + attribute_map_[key_name] + "']";
    std::string out;
    try{
        auto res = doc.select_node(search.c_str());
        out = res.node().child_value();
    } catch(...){}
    return out;
}