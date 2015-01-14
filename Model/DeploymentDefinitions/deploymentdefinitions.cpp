#include "deploymentdefinitions.h"
#include <QDebug>
#include "assemblydefinitions.h"
#include "hardwaredefinitions.h"

DeploymentDefinitions::DeploymentDefinitions():Node()
{
}

DeploymentDefinitions::~DeploymentDefinitions()
{

}

bool DeploymentDefinitions::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool DeploymentDefinitions::canAdoptChild(Node *child)
{

    AssemblyDefinitions* assemblyDefinitions = dynamic_cast<AssemblyDefinitions *>(child);
    HardwareDefinitions* hardwareDefinitions = dynamic_cast<HardwareDefinitions *>(child);


    if(!hardwareDefinitions && !assemblyDefinitions){
        qWarning() << "Cannot Adopt anything outside of Assembly Definitions and Hardware Definitions";
        return false;
    }

    return Node::canAdoptChild(child);
}
