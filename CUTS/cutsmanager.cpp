#include "cutsmanager.h"
#include <QFile>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QDir>
#include <QDebug>
#include <QPair>
#include <QProcess>
#include <QEventLoop>
#include <QFileInfo>

CUTSManager::CUTSManager(QString xalanPath)
{
    setXalanPath(xalanPath);
    executingProcessCount = 0;
    MAX_EXECUTING_PROCESSES = 1;
}

CUTSManager::~CUTSManager()
{

}

void CUTSManager::setXalanPath(QString xalanPath)
{
    //Enforce a trailing /
    if(!xalanPath.endsWith("/")){
        xalanPath += "/";
    }
    this->xalanPath = xalanPath;
}

void CUTSManager::setTransformPath(QString transformPath)
{
    //Enforce a trailing /
    if(!transformPath.endsWith("/")){
        transformPath += "/";
    }
    this->transformPath = transformPath;

}

void CUTSManager::setThreadLimit(int limit){
    MAX_EXECUTING_PROCESSES = limit;
}

void CUTSManager::runTransforms(QString graphml_path, QString output_path)
{
    QDir dir(output_path);
    if (!dir.exists()) {
        qCritical() << "Making Directory";
        dir.mkpath(".");
    }

    QFile xmlFile(graphml_path);

    if (!xmlFile.exists() || !xmlFile.open(QIODevice::ReadOnly))
        return;

    //Set output Path
    outputPath = output_path;

    //Try preprocess the IDL
    QString outputFile = preProcessIDL(graphml_path);

    if(doesFileExist(outputFile)){
        //Run The rest of the transforms
        processGraphML(outputFile);
    }
}

void CUTSManager::processFinished(int code, QProcess::ExitStatus)
{
    QProcess* process = dynamic_cast<QProcess*>(sender());
    if(process){
        //Get the Process, so we can find the matching output file path.
        QString outputFilePath = processHash[process];
        //Emit a Signal saying that the file has been produced.
        emit generatedFile(outputFilePath, code == 0);
    }
    //Decrement the currentRunningCount
    executingProcessCount--;

    //Move onto the next items in the Queue.
    processQueue();
}

void CUTSManager::processGraphML(QString graphml_file)
{
    QFile xmlFile(graphml_file);
    if (!xmlFile.exists() || !xmlFile.open(QIODevice::ReadOnly))
        return;

    graphmlPath = graphml_file;
    //Check for Deployed Components.
    bool allComponents = false;


    QXmlQuery* query = new QXmlQuery();
    query->bindVariable("doc", &xmlFile);

    QHash<QString, QString> keyIDs;
    QList<QPair<QString, QString> > edges;
    QHash<QString, QString> componentDefs;

    QStringList deployedComponentDefs;
    QStringList deployedComponentInstances;
    QStringList hardwareIDs;
    QStringList IDLs;

    //Get the List of Keys.
    QXmlResultItems* key_xml = getQueryList(query, "doc($doc)//gml:graphml/gml:key[@for='node']");
    QXmlItem item = key_xml->next();

    while (!item.isNull()) {
        QString ID = getQuery(query, "@id/string()", &item);
        QString keyName = getQuery(query, "@attr.name/string()", &item);

        keyIDs[keyName] = ID;
        //Next Item
        item = key_xml->next();
    }

    QXmlResultItems* edge_xml = getQueryList(query, "doc($doc)//gml:edge");
    item = edge_xml->next();

    while (!item.isNull()) {
        QString source = getQuery(query, "@source/string()", &item);
        QString target = getQuery(query, "@target/string()", &item);

        edges.append(QPair<QString, QString>(source, target));

        //Next Item
        item = edge_xml->next();
    }

    //Get the list of Hardware*
    QXmlResultItems* idl_xml = getQueryList(query, "doc($doc)//gml:node[gml:data[@key='" + keyIDs["kind"] + "' and string()='IDL']]");
    item = idl_xml->next();

    while (!item.isNull()) {
        //Get ID of the Hardware(Node|Cluster)
        QString label = getQuery(query, "gml:data[@key='" + keyIDs["label"] + "']/string()", &item);
        IDLs << label;

        //Next Item
        item = idl_xml->next();
    }


    //Get the list of Hardware*
    QXmlResultItems* hardware_xml = getQueryList(query, "doc($doc)//gml:node[gml:data[@key='" + keyIDs["kind"] + "' and starts-with(string(),'Hardware')]]");
    item = hardware_xml->next();

    while (!item.isNull()) {
        //Get ID of the Hardware(Node|Cluster)
        hardwareIDs << getQuery(query, "@id/string()", &item);;

        //Next Item
        item = hardware_xml->next();
    }

    //Get Component Definitions
    QXmlResultItems* component_xml = getQueryList(query, "doc($doc)//gml:node[gml:data[@key='" + keyIDs["kind"] + "' and string() = 'Component']]");
    item = component_xml->next();

    while (!item.isNull()){
        QString ID = getQuery(query, "@id/string()", &item);
        QString label = getQuery(query, "gml:data[@key='" + keyIDs["label"] + "']/string()", &item);
        componentDefs[ID] = label;
        item = component_xml->next();
    }

    //Get Component Instances
    QXmlResultItems* componentInstance_xml = getQueryList(query, "doc($doc)//gml:node[gml:data[@key='" + keyIDs["kind"] + "' and string() = 'ComponentInstance']]");
    item = componentInstance_xml->next();

    while (!item.isNull()){
        QString longLabel;
        QString definitionName;

        bool isDeployed = false;
        QString ID = getQuery(query, "@id/string()", &item);

        QPair<QString,QString> edge;
        foreach(edge, edges){
            if(componentDefs.keys().contains(edge.second) && edge.first == ID){
                definitionName = componentDefs[edge.second];
            }
        }

        QXmlItem parent = item;

        //Recurse up parents!
        while(!parent.isNull()){
            QString label = getQuery(query, "gml:data[@key='" + keyIDs["label"] + "']/string()", &parent);
            QString parentID = getQuery(query, "@id/string()", &parent);
            QString kind = getQuery(query, "gml:data[@key='" + keyIDs["kind"] + "']/string()", &parent);

            //Check if deployed.
            foreach(edge, edges){
                if(hardwareIDs.contains(edge.second) && edge.first == parentID){
                    isDeployed = true;
                }
            }

            //If kind isn't a Component or an Assembly, break loop.
            if(!kind.startsWith("Component")){
                break;
            }
            //Setup LongLabel
            if(longLabel.isEmpty()){
                longLabel = label;
            }else{
                longLabel = label + "%%" + longLabel;
            }
            //Select parent of parent.
            QXmlResultItems* parent_xml = getQueryList(query, "../..", &parent);
            parent = parent_xml->next();
        }


        //Add it to the list of used ComponentDefs
        if((allComponents || isDeployed) && !deployedComponentDefs.contains(definitionName)){
            deployedComponentDefs << definitionName;
        }
        //Add instance to the list of used ComponentInstances
        if((allComponents || isDeployed) && !deployedComponentInstances.contains(longLabel)){
            deployedComponentInstances << longLabel;
        }
        item = componentInstance_xml->next();
    }

    generateComponentArtifacts(deployedComponentDefs);
    generateComponentInstanceArtifacts(deployedComponentInstances);
    generateIDLArtifacts(IDLs);

    QStringList mpcFiles;
    //Construct a list of MPC files
    foreach(QString component, deployedComponentDefs){
        mpcFiles << component + "Impl.mpc";
    }
    foreach(QString IDL, IDLs){
        mpcFiles << IDL + ".mpc";
    }


    generateModelArtifacts(mpcFiles);
}

QString CUTSManager::wrapQuery(QString query)
{
    return "declare namespace gml = \"http://graphml.graphdrawing.org/xmlns\"; " + query;
}

QString CUTSManager::getQuery(QXmlQuery* query, QString queryStr, QXmlItem* item)
{
    QString value;
    if(item && !item->isNull()){
        query->setFocus(*item);
    }
    query->setQuery(wrapQuery(queryStr));
    query->evaluateTo(&value);
    return value.trimmed();
}

bool CUTSManager::doesFileExist(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo = QFileInfo(file);
    return fileInfo.isFile() && fileInfo.size() > 0;
}

QXmlResultItems *CUTSManager::getQueryList(QXmlQuery* query, QString queryStr, QXmlItem* item)
{
    QXmlResultItems* results = new QXmlResultItems();
    if(item && !item->isNull()){
        query->setFocus(*item);
    }
    query->setQuery(wrapQuery(queryStr));
    query->evaluateTo(results);
    return results;
}

void CUTSManager::generateComponentArtifacts(QStringList components)
{
    QStringList transforms;
    transforms << "mpc" << "cpp" << "h";
    foreach(QString transform, transforms){
        foreach(QString component, components){
            QStringList parameters;
            parameters << "File" << component + "Impl." + transform;
            QString outputFile = outputPath + component + "Impl." + transform;
            QString xslFile = transformPath + "graphml2" + transform + ".xsl";
            queueXSLTransform(graphmlPath, outputFile, xslFile, parameters);
        }
    }
}

void CUTSManager::generateComponentInstanceArtifacts(QStringList componentInstances)
{
    QStringList transforms;
    transforms << "dpd";
    foreach(QString transform, transforms){
        foreach(QString componentInstance, componentInstances){
            QStringList parameters;
            parameters << "ComponentInstance" << componentInstance;
            QString outputFile = outputPath + componentInstance + "%%QoS.dpd";
            QString xslFile = transformPath + "graphml2" + transform + ".xsl";
            queueXSLTransform(graphmlPath, outputFile, xslFile, parameters);
        }
    }

}

void CUTSManager::generateIDLArtifacts(QStringList idls)
{
    QStringList transforms;
    transforms << "idl" << "mpc";
    foreach(QString transform, transforms){
        foreach(QString idl, idls){
            QStringList parameters;
            parameters << "File" <<  idl + "." + transform;
            QString outputFile = outputPath + idl + "." + transform;
            QString xslFile = transformPath + "graphml2" + transform + ".xsl";
            queueXSLTransform(graphmlPath, outputFile, xslFile, parameters);
        }
    }

}

void CUTSManager::generateModelArtifacts(QStringList mpcFiles)
{
    QString modelName = getGraphmlName(graphmlPath);

    queueXSLTransform(graphmlPath, outputPath + modelName + ".cdd", transformPath + "graphml2cdd.xsl", QStringList());
    queueXSLTransform(graphmlPath, outputPath + modelName + ".cdp", transformPath + "graphml2cdp.xsl", QStringList());
    queueXSLTransform(graphmlPath, outputPath + modelName + ".ddd", transformPath + "graphml2ddd.xsl", QStringList());

    QStringList mwcParams;
    mwcParams << "FileList";
    mwcParams << mpcFiles.join(",");
    queueXSLTransform(graphmlPath, outputPath + modelName + ".mwc", transformPath + "graphml2mwc.xsl", mwcParams);
}

QString CUTSManager::preProcessIDL(QString inputFilePath)
{
    //Start a QProcess for this program
    QProcess* process = new QProcess(this);
    process->setWorkingDirectory(transformPath);

    QString outFileName = outputPath + getGraphmlName(inputFilePath) + ".graphml";

    //Construct the arguments for the xsl transform
    QStringList arguments;
    arguments << "-jar" << xalanPath + "xalan.jar";
    arguments << "-in" << inputFilePath;
    arguments << "-xsl" << transformPath + "PreprocessIDL.xsl";
    arguments << "-out" << outFileName;

    //Construct a wait loop to make sure this transform happens first.
    QEventLoop waitLoop;
    connect(process, SIGNAL(finished(int)), &waitLoop, SLOT(quit()));

    //Execute the QProcess
    process->start("java", arguments);

    //Wait for The process to exit the loop.
    waitLoop.exec();

    //Return the filepath of the new Graphml file.
    return outFileName;
}

void CUTSManager::queueXSLTransform(QString inputFilePath, QString outputFilePath, QString xslFilePath, QStringList parameters)
{
    QStringList arguments;
    arguments << "-jar" << xalanPath + "xalan.jar";
    arguments << "-in" << inputFilePath;
    arguments << "-xsl" << xslFilePath;
    arguments << "-out" << outputFilePath;
    if(parameters.size() > 0){
        arguments << "-param";
        arguments += parameters;
    }

    //Emit that we are to Generate this file.
    emit generatingFile(outputFilePath);

    //Construct and fill a Struct to contain the parameters for the spawned Process.
    ProcessStruct ps;
    ps.arguments = arguments;
    ps.outputFilePath = outputFilePath;
    ps.program = "java";
    queue.append(ps);

    //Step into the queue.
    processQueue();
}

/**
 * @brief CUTS::processQueue Ensures that there are MAX_EXECUTING_PROCESSES number of QProcesses currently executing, until there is nothing left in the queue to execute.
 */
void CUTSManager::processQueue()
{
    //While there isn't enough processes currently executing
    while(executingProcessCount <  MAX_EXECUTING_PROCESSES){
        if(queue.isEmpty()){
            //Queue is empty, so don't continue;
            break;
        }
        //Enqueue next process.
        ProcessStruct ps = queue.takeFirst();
        executeProcess(ps.program, ps.arguments, ps.outputFilePath);
    }
}

/**
 * @brief CUTS::executeProcess Executes a Program, with Arguments.
 * @param program - The Program to execute
 * @param arguments - The list of arguments for the program.
 * @param outputFilePath - The desired output filepath for the file.
 */
void CUTSManager::executeProcess(QString program, QStringList arguments, QString outputFilePath)
{
    //Start a QProcess for this program
    QProcess* process = new QProcess(this);
    process->setWorkingDirectory(transformPath);

    //Connect the Process' finished Signal
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));

    //Execute the QProcess
    process->start(program, arguments);

    //Store the Process in the hash so we can workout when all files are finished
    processHash[process] = outputFilePath;
    //Increment the number or currently running Process
    executingProcessCount++;
}

QString CUTSManager::getGraphmlName(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo = QFileInfo(file);
    return fileInfo.baseName();
}

QString CUTSManager::getGraphmlPath(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo = QFileInfo(file);
    return fileInfo.absolutePath();
}
