#include "utils.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

QString Utils::readTextFile(QString file_path){
    QString file_data = "";
    QFile file(file_path);
    QFileInfo fileInfo(file);

    if(file.exists()){
        if(file.open(QFile::ReadOnly | QFile::Text)){
            QTextStream fileStream(&file);
            file_data = fileStream.readAll();
            file.close();
        }
    }
    return file_data;
}