#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <climits>
#include <tuple>

#include "deploymentrule.h"
#include "deploymentgenerator.h"
#include "environment.h"
#include "deploymentrules/zmq/zmqrule.h"
#include "deploymentrules/dds/ddsrule.h"

#include "../nodemanager/executionparser/protobufmodelparser.h"

int main(int argc, char **argv){


    std::string model_path(argv[1]);
    ProtobufModelParser parser(model_path);
    std::cout << parser.GetDeploymentJSON() << std::endl;

    auto message = parser.ControlMessage();

    //std::cout << message->DebugString() << std::endl;

    Environment* environment = new Environment();

    DeploymentGenerator generator(*environment);
    generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Zmq::DeploymentRule(*environment)));
    generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Dds::DeploymentRule(*environment)));

    generator.PopulateDeployment(*message);


    std::cout << "==============================================" << std::endl;
    std::cout << std::endl;

    std::cout << message->DebugString() << std::endl;

    return 0;
}
