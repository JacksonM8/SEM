/**
This script will deploy re to all jenkins slaves with the re label.
This script requires the following Jenkins parameters:
- "GIT_CREDENTIAL_ID" : Username with password credential
- "GIT_URL" : String parameter (Defaulted to https://github.com/cdit-ma/re)
- "GIT_BRANCH" : Optional branch to checkout, overridden by GIT_TAG
- "GIT_TAG" : Optional version tag, ignores any branch specified
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

def PROJECT_NAME = 're'

// This method collects a list of Node names from the current Jenkins instance
@NonCPS
def nodeNames() {
  return jenkins.model.Jenkins.instance.nodes.collect { node -> node.name }
}

//Gets nodes label
def getLabels(String name){
    def computer = Jenkins.getInstance().getComputer(name)
    def node = computer.getNode()
    if(computer.isOnline()){
        return node.getLabelString()
    }
    return ""
}

//Check all required params
def checkParamsExist(){
    def git_url = ""
    git_url = env.GIT_URL
    if(git_url == null){
        print("ERROR: \"GIT_URL\" string parameter missing in build configuration")
        return false
    }
    def git_credential_id = ""
    git_credential_id = env.GIT_CREDENTIAL_ID
    if(git_credential_id == null) {
        print("ERROR: \"GIT_CREDENTIAL_ID\" credential parameter missing in build configuration")
        return false
    }
    def branch = env.GIT_BRANCH
    if(branch == null){
        print("ERROR: \"GIT_BRANCH\" string parameter missing in build configuration")
        return false
    }

    def tag = env.GIT_TAG
    if(tag == null){
        print("ERROR: \"GIT_TAG\" string parameter missing in build configuration")
        return false
    }

    return true
}

def getCredentialID(){
    return env.GIT_CREDENTIAL_ID
}

//Build git ref pointer. If no branch specified, use tag, if no tag specified use default branch (master).
def buildGitRef(String branch, String tag){
    def default_branch = "master"
    def ref_name = ""

    if(!branch){
        branch = default_branch
    }
    ref_name = "*/" + branch

    if(tag){
        ref_name = "refs/tags/" + tag
    }
    return ref_name
}

//Build git url. If no url specified, default to cdit-ma github+proj name
def buildGitUrl(String url, String proj){
    def default_git_url = "https://github.com/cdit-ma/" + proj + ".git"
    //Set git url to default if empty
    if(!url){
        return default_git_url
    }
    return url
}


def ref_name = ""
def git_url = ""
def git_credential_id = ""
def filtered_names = []

stage('Set up'){
    //Start deploy script
    if(!checkParamsExist()){
        currentBuild.result = 'FAILURE'
        return
    }

    //Get nodes to deploy to
    def names = nodeNames()
    for(n in names){
        if(getLabels(n).contains(PROJECT_NAME)){
            filtered_names << n
            print("Got Node: " + n)
        }
    }

    //Build git url and ref
    git_credential_id = getCredentialID()
    git_url = buildGitUrl(env.GIT_URL, PROJECT_NAME)
    ref_name = buildGitRef(env.GIT_BRANCH, env.GIT_TAG)
    currentBuild.description = git_url + '/' + ref_name
}

//Checkout and stash re source
stage('Checkout'){
    node('master'){
        dir('re'){
            checkout scm
            stash include: "**", name: "re_source"
        }
    }
}

//Checkout and stash re source
stage('Checkout'){
    node('master'){
        checkout([$class: 'GitSCM', branches: [[name: "*/master"]], userRemoteConfigs: [[credentialsId: git_credential_id, url: git_url, refspec: "+refs/heads/master:refs/remotes/origin/master"]]])
        
        stash include: "**", name: "re_source"
    }
}

//Unstash re source on all re nodes
stage('Deploy'){
    def builders = [:]
    for(n in filtered_names){
        def node_name = n
        builders[node_name] = {
            node(node_name){
                dir("${RE_PATH}"){
                    unstash "re_source"
                }
            }
        }
    }
    parallel builders
}

//Build re on all re nodes
stage('Build'){
    def builders = [:]
    for(n in filtered_names){ 
        def node_name = n
        builders[node_name] = {  
            node(node_name){
                dir("${RE_PATH}" + '/build'){
                    sh 'cmake ..'
                    sh 'make -j6'
                }
            }
        }
    }
    parallel builders
}
