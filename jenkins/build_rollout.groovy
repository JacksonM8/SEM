/**
This script will generate a new rollout.zip by pulling from the CDIT-MA orginisation's github server.
This script should be used to generate a new rollout for deployment to airgapped networks.
The .zip generated by this job can be uploaded to the airgapped network by using the "upload_new_rollout" job.
This script requires the following Jenkins parameters:
- "GIT_CREDENTIALS_ID" : Username with password credential
- "GIT_URL" : String parameter (Defaulted to https://github.com/cdit-ma/SEM)
- "SEM_TAG" : String parameter. Version of SEM environment to checkout
- "SEM_BRANCH" : String parameter. Branch of SEM environment to checkout.
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

@Library('cditma-utils')
import cditma.Utils



node("master"){
    final GIT_USER = env.GIT_CREDENTIALS_ID
    final GIT_BRANCH = env.SEM_BRANCH ? env.SEM_BRANCH : env.GIT_TAG
    final CDITMA_GIT_URL = "https://github.com/cdit-ma/"
    final ROLLOUT_FILE_NAME = "re-" + GIT_BRANCH + "-rollout.tar.gz"

    //Set the Label
    currentBuild.description = GIT_BRANCH
    print("GIT_USER: " + GIT_USER)
    print("GIT_BRANCH: " + GIT_BRANCH)
    stage("Checkout"){
        //Check out Scripts
        dir("scripts"){
            git(
                branch: 'develop',
                credentialsId: GIT_USER,
                url: CDITMA_GIT_URL + "scripts"
            )
        }
        dir("re"){
            git(
                branch: GIT_BRANCH,
                credentialsId: GIT_USER,
                url: CDITMA_GIT_URL + "re"
            )
        }
    }
    stage("Archive"){
        files = []

        dir("scripts"){
            Utils.runScript('git bundle ../artifacts/scripts.bundle develop')
            Utils.runScript('git-archive-all ../artifacts/scripts.tar.gz')
            files += 'scripts.bundle'
            files += 'scripts.tar.gz'
        }
        dir("re"){
            Utils.runScript('git bundle ../artifacts/re.bundle ' + GIT_BRANCH)
            Utils.runScript('git-archive-all ../artifacts/re.tar.gz')
            files += 're.bundle'
            files += 're.tar.gz'
        }
        dir("artifacts"){
            //Create archive
            Utils.runScript('tar -czf ' + ROLLOUT_FILE_NAME + ' ' + files.join(' '))
            archiveArtifacts(ROLLOUT_FILE_NAME)
            deleteDir();
        }
    }
}