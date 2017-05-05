#include "labelkey.h"
#include "../data.h"
#include "../node.h"

#include <QRegularExpression>

LabelKey::LabelKey(): Key("label", QVariant::String){
    //QStringList invalidChars;
    //invalidChars << "*" << "." << "[" << "]"<< ";" << "|" << "," <<  "%";
    //invalidChars << "\"" << "'"  << "/" << "\\" << "=" << ":" << " " << "<" << ">" << "\t";
}   

QVariant LabelKey::validateDataChange(Data* data, QVariant data_value){
    auto new_label = data_value.toString();
    Node* node = 0;

    //Get the node this data is attached to
    auto entity = data->getParent();
    if(entity && entity->isNode()){
        node = (Node*) entity;
    }

    //Get rid of spaces and tabs
    new_label.replace(" ", "_");
    new_label.replace("\t", "_");

    //If its not attached to a node, the value is valid
    if(!node){
        return new_label;
    }

    //Check for _XX
    QRegularExpression regex("^" + new_label + "_([0-9]+)?$");

    //Match counts
    bool exact_match = false;
    QList<int> matched_numbers;

    foreach(auto sibling, node->getSiblings()){
        auto sibling_label = sibling->getDataValue("label").toString();
        if(sibling_label == new_label){
            exact_match = true;
        }else{
            //Check if the sibling label = new_label_XX
            auto re_match = regex.match(sibling_label);
            if(re_match.hasMatch()){
                QString number_str = re_match.captured(1);
                //We know the capture group has to be a number
                int number = number_str.toInt();
                matched_numbers << number;
            }
        }
    }

    if(!exact_match){
        //If there is no exact match, the label is fine!
        return new_label;
    }else{
        //Sort the matched_numbers so we can see if there is a gap
        qSort(matched_numbers);
        //If there is no gaps, the position this label will need will be last (+1)
        int new_number = matched_numbers.size() + 1;
        
        for(int i = 1; i < matched_numbers.size() + 1; i++){
            //If we find a gap in the numbers
            if(!matched_numbers.contains(i)){
                new_number = i;
                break;
                
            }
        }

        new_label += "_" + QString::number(new_number);
        return new_label;
    }
}