#ifndef GRAPHMLPARSER_H
#define GRAPHMLPARSER_H

#include <pugixml.hpp>
#include <string>
#include <vector>
#include <map>


class GraphmlParser{

    public:
        GraphmlParser(std::string filename);
        std::vector<std::string> FindNodes(std::string kind, std::string parent_id = "");
        std::vector<std::string> FindEdges(std::string kind ="");

        std::string GetAttribute(std::string id, std::string attribute_name);
        std::string GetDataValue(std::string id, std::string key_name);

        std::string GetParentNode(std::string id);
        

    private:
        std::map<std::string, std::string> attribute_map_;
        pugi::xml_document doc;
        std::map<std::string, std::string> data_lookup_;
        std::map<std::string, std::string> attr_lookup_;


};
#endif //"GRAPHMLPARSER_H"