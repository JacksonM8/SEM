@NonCPS
def nodeNames() {
  return jenkins.model.Jenkins.instance.nodes.collect { node -> node.name }
}

def getLabels(String name){
    def computer = Jenkins.getInstance().getComputer(name)
    def node = computer.getNode()
    if(computer.isOnline()){
        return node.getLabelString()
    }
    return ""
}

def names = nodeNames()
//filter nodes
def filtered_names = []
for(n in names){
    if(getLabels(n).contains("re") && getLabels(n).contains("test_env")){
        filtered_names << n
        print("Got Node: " + n)
    }
}

if(filtered_names.isEmpty()){
    print("###### Error: No valid nodes found ######")
}

stage('Checkout'){
    def builders = [:]
    for(n in filtered_names){
        def node_name = n
        builders[node_name] = {
            node(node_name){
                dir('re'){
                    deleteDir()
                    checkout([$class: 'GitSCM', branches: [[name: '*/develop']], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: false, reference: '', trackingSubmodules: false]], submoduleCfg: [], userRemoteConfigs: [[credentialsId: "${GIT_CREDENTIAL_ID}", url: 'https://github.com/cdit-ma/re.git']]])
                }
            }
        }
    }
    parallel builders
}

stage('Build'){
    def builders = [:]
    for(n in filtered_names){
        def node_name = n
        builders[node_name] = {
            node(node_name){
                withEnv(['CMAKE_MODULE_PATH=' + pwd() + '/re/cmake_modules']) {
                    dir('re/build'){
                        sh 'cmake ..'
                        sh 'make -j6'
                    }
                }
            }
        }
    }
    parallel builders
}

stage('Test'){
    def builders = [:]
    for(n in filtered_names){
        def node_name = n
        builders[node_name] = {
            node(node_name){
                print('Running tests here')
            }
        }
    }
    parallel builders
}