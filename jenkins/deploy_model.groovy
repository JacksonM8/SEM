//This script requires the following Jenkins plugins:
//-Pipeline: Utility Steps

//Requires following parameters in jenkins job:
// -String parameter: MASTER_NODE
// -String parameter: EXECUTION_TIME
// -String parameter: EXPERIMENT_NAME
// -String parameter: ENVIRONMENT_MANAGER_ADDRESS

//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
import hudson.model.Computer.ListPossibleNames

def utils = new Utils(this);

def masterNode = "${MASTER_NODE}"
def executionTime = "${EXECUTION_TIME}"
def experimentNameArg = "${EXPERIMENT_NAME}"
def experimentID = env.BUILD_ID
def buildDir = "run" + experimentID
def experimentName = experimentID
def environmentManagerAddress = env.ENVIRONMENT_MANAGER_ADDRESS

if(!experimentNameArg.isEmpty()){
    experimentName = experimentNameArg
}

//Deployment plans
def loganServers = [:]
def loganClients = [:]
def loganServers_shutdown = [:]
def loganClients_shutdown = [:]
def nodeManagers = [:]
def compileCode = [:]
def addrMap = [:]
def libLocationMap = [:]

def fail_flag = false
def failureList = []

def file = "model.graphml"

//TODO: Change this back to 're'
def reNodes = utils.getLabelledNodes("envmanager_test")

//XXX: Problems ahoy here
//In case where node has multiple network interfaces, we're cooked.
for(def i = 0; i < reNodes.size(); i++){
    def nodeName = reNodes[i];
    def nodeIPLookup = jenkins.model.Jenkins.instance.getNode(nodeName)
    def ip_addr_list = nodeIPLookup.computer.getChannel().call(new ListPossibleNames())
    addrMap[nodeName] = ip_addr_list[0]
}

withEnv(["model=''"]){
    node(masterNode){

        def workspacePath = pwd()
        def reGenPath = "${RE_PATH}" + "/re_gen"
        def saxonPath = reGenPath

        def middlewareString = ' middlewares='
        def fileString = ' -s:' + workspacePath + "/" + file
        def jarString = 'java -jar '  + saxonPath + '/saxon.jar -xsl:' + reGenPath
        
        def buildPath = workspacePath + "/" + buildDir
        unstashParam "model", file
        stash includes: file, name: 'model'

        //TODO: Fix this to actually get middlewares from somewhere
        //code gen may actually handle this automatically without args now days, ask dan
        middlewareString += "zmq,proto,rti,ospl"
        //Generate C++ code
        stage('CodeGen'){
            dir(buildPath){
                unstash 'model'
                archiveArtifacts file
                
                stage('C++ Generation'){
                    def typeGenCommand = jarString + '/g2datatypes.xsl' + fileString + middlewareString
                    if(utils.runScript(typeGenCommand) != 0){
                        failureList << "Datatype code generation failed"
                        fail_flag = true;
                    } 
                    def componentGenCommand = jarString + '/g2components.xsl' + fileString + middlewareString
                    if(utils.runScript(componentGenCommand) !=0){
                        failureList << "Component code generation failed"
                        fail_flag = true;
                    }

                    dir("lib"){
                        //Generate QOS into lib directory
                        def qosGenCommand = jarString + '/g2qos.xsl' + fileString + middlewareString
                        print(qosGenCommand)
                        if(utils.runScript(qosGenCommand) != 0){
                            failureList << "QoS generation failed"
                            fail_flag = true;
                        }
                    }
                }
            }
        }

        //Archive code gen and add to build artifacts
        stage('Archive'){
            dir(buildPath){
                stash includes: '**', name: 'codeGen'
                zip(zipFile: "archive.zip", archive: true, glob: '**')
            }
        }
    }

    stage("Build deployment plan"){
        for(def i = 0; i < reNodes.size(); i++){
            def nodeName = reNodes[i];
            def ipAddr = addrMap[nodeName]

            //Populate map of scripts to run compilation on all nodes
            compileCode[nodeName] = {
                node(nodeName){
                    dir(buildDir){
                        unstash 'codeGen'
                        libLocationMap[nodeName] = pwd()
                    }
                    dir(buildDir + "/build"){
                        if(!utils.buildProject("Ninja", "")){
                            failureList << ("cmake failed on node: " + nodeName)
                            fail_flag = true;
                        }
                    }
                }
            }

            //Populate map of scripts to run re_node_manager on all nodes
            nodeManagers[nodeName] = {
                node(nodeName){
                    def buildPath = libLocationMap[nodeName] + "/lib"
                    def master_args = ""
                    def slave_args = ""
                    def shared_args = ""
                    def command = "${RE_PATH}" + "/bin/re_node_manager"

                    //Set args required for both slaves and masters
                    shared_args += " -n " + experimentName
                    shared_args += " -e " + environmentManagerAddress
                    shared_args += " -a " + ipAddr
                    slave_args += " -l . "

                    //If we are a master, set exectuion time and deployment file location
                    if(nodeName == masterNode){
                        master_args += " -t " + executionTime
                        master_args += " -d " + file
                    }

                    command += shared_args
                    command += slave_args
                    command += master_args

                    dir(buildPath){
                        if(nodeName == masterNode){
                            unstash 'model'
                        }
                        if(utils.runScript(command) != 0){
                            failureList << ("Experiment slave failed on node: " + nodeName)
                            fail_flag = true
                        }
                    }
                }
            }
        }
    }

    //Run compilation scripts
    stage("Compiling C++"){
        parallel compileCode
    }

    //Run re_node_manager scripts
    stage("Execute Model"){
        if(!fail_flag){
            parallel nodeManagers
        }
        else{
            print("############# Execution skipped, compilation or code generation failed! #############")
        }
    }

    if(fail_flag){
        currentBuild.result = 'FAILURE'
        print("Execution failed!")
        print(failureList.size() + " Error(s)")
        for(s in failureList){
            print("ERROR: " + s)
        }
    }
    else{
        currentBuild.result = 'SUCCESS'
    }
}
