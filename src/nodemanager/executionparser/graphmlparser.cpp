#include "graphmlparser.h"
#include <pugixml.hpp>

#include <iostream>

GraphmlParser::GraphmlParser(const std::string& filename){
    auto result = doc.load_file(filename.c_str());
    legal_parse = result.status == pugi::status_ok;;
    if(!legal_parse){
        std::cerr << "GraphmlParser:Parse(" + filename + ") Error: " << result.description() << std::endl;
    }
    for(auto& key : doc.select_nodes("/graphml/key")){
        const auto &name = key.node().attribute("attr.name").value();
        const auto &id = key.node().attribute("id").value();
        attribute_map_[name] = id;
    }

    for(auto& xpath_node : doc.select_nodes("//*[@id]")){
        auto node = xpath_node.node();
        auto id = node.attribute("id").value();
        id_lookup_[id] = node;
    }
}

bool GraphmlParser::IsValid(){
    return legal_parse;
}

std::vector<std::string> GraphmlParser::FindNodes(const std::string& kind, const std::string& parent_id){
    auto search_node = doc.document_element();

    if(parent_id.length() > 0 && id_lookup_.count(parent_id)){
        search_node = id_lookup_[parent_id];
    }
    std::string search = ".//node/data[@key='" + attribute_map_["kind"] + "' and .='" + kind +"']/..";
    std::vector<std::string> out;

    for(auto& n : search_node.select_nodes(search.c_str())){
        out.push_back(n.node().attribute("id").value());
    }
    return out;
}

std::vector<std::string> GraphmlParser::FindEdges(const std::string& kind){
    std::string search = ".//edge";
    
    if(kind != ""){
        search += "[data[@key='" + attribute_map_["kind"] + "' and .='" + kind +"']]";
    }

    std::vector<std::string> out;

    for(auto& n : doc.select_nodes(search.c_str())){
        out.push_back(n.node().attribute("id").value());
    }
    return out;
}

std::string GraphmlParser::GetAttribute(const std::string& id, const std::string& attribute_name){

    std::string key = id + "|" + attribute_name;

    if(attr_lookup_.count(key)){
        return attr_lookup_[key];
    }else{
        std::string out;
        
        if(id_lookup_.count(id)){
            auto& entity = id_lookup_[id];
            if(entity){
                auto attribute = entity.attribute(attribute_name.c_str());
                if(attribute){
                    out = attribute.value();
                    attr_lookup_[key] = out;
                }else{
                    std::cerr << "GraphmlParser: Entity with ID: '" << id << "' doesn't have an attribute: '" << attribute_name << "'" << std::endl;
                }
            }
        }else{
            std::cerr << "GetAttribute" << std::endl;
            std::cerr << "GraphmlParser: No entity with ID: '" << id << "'" << std::endl;
        }
        return out;
    }
}

std::string GraphmlParser::GetDataValue(const std::string& id, const std::string& key_name){
    std::string key = id + "|" + key_name;

    if(data_lookup_.count(key)){
        return data_lookup_[key];
    }else{
        std::string out;
        
        if(id_lookup_.count(id)){
            auto& entity = id_lookup_[id];
            if(entity){
                auto search = "data[@key='" + attribute_map_[key_name] + "']";

                auto res = entity.select_node(search.c_str());
                auto node = res.node();

                if(node){
                    out = node.child_value();
                    data_lookup_[id] = out;
                }else{
                    std::cerr << "GraphmlParser: Entity with ID: '" << id << "' doesn't have an data value with key: '" << key_name << "'" << std::endl;
                }
            }
        }else{
            std::cerr << "GetDataValue" << std::endl;
            std::cerr << key_name << std::endl;
            std::cerr << "GraphmlParser: No entity with ID: '" << id << "'" << std::endl;
        }
        return out;
    }
}

std::string GraphmlParser::GetParentNode(const std::string& id){
    if(parent_id_lookup_.count(id)){
        return parent_id_lookup_[id];
    }else{
        auto search_node = doc.document_element();
        std::string out;
        if(id_lookup_.count(id)){
            search_node = id_lookup_[id];
            auto res = search_node.select_node("../..");
            out = res.node().attribute("id").value();
            parent_id_lookup_[id] = out;
        }else{
            std::cerr << "GetParentNode" << std::endl;
            std::cerr << "GraphmlParser: No entity with ID: '" << id << "'" << std::endl;
        }
        return out;
    }
}
