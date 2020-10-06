pipeline
{
   agent { label 'amd64 && docker' }

   environment
   {
      REGISTRY="artifactory.prismtech.com/edge-docker-dev-ghead"
      REGISTRY_CREDS = credentials('929bd998-0cf3-4092-aa84-4d2fa5641938')
      IMAGE_NAME="${REGISTRY}/adlinktech/${JOB_NAME.replace('/',':')}"
      DOCKER_CONFIG="${WORKSPACE}/.docker"
      CPP_LINTER="${REGISTRY}/adlinktech/cpp-linter:0.1.0"
      APP_NAME="${JOB_NAME.split('/')[0]}"
      UNIQUE_NAME="adlinktech-${JOB_NAME.replace('/','-')}-${BUILD_ID}".toLowerCase()
      NETWORK_NAME_BASE="adlinktech-${JOB_NAME.replace('/','-')}-${BUILD_ID}"
      DOCSBUILDER="${REGISTRY}/adlinktech/docsbuilder:2.0.0"
   }

   options
   {
      timeout (time: 3, unit: 'HOURS')
      buildDiscarder (logRotator (numToKeepStr: '1'))
   }
   stages
   {
      stage('Login') {
         steps {
               withCredentials([usernamePassword(credentialsId: '929bd998-0cf3-4092-aa84-4d2fa5641938', usernameVariable: 'USERNAME', passwordVariable: 'PASSWORD')]) {
               sh ('echo $PASSWORD | docker login -u $USERNAME --password-stdin $REGISTRY')
            }
         }
      }
      // stage('Build Docs') {
      //    agent { label 'amd64 && docker'}
      //    steps {
      //       sh 'docker run --rm -u `id -u`:`id -g` -v $(pwd)/docs:/docs -v $(pwd)/vortexedge:/vortexedge ${DOCSBUILDER} --license-rst -v -d /vortexedge/metadata -o /docs/releasenotes/source/Ch-Licenses.rst'
      //       sh '${WORKSPACE}/scripts/buildDocs.sh userguide'
      //       sh '${WORKSPACE}/scripts/buildDocs.sh releasenotes html'
      //       archiveArtifacts artifacts: "vortexedge/docs/adlinktech/${APP_NAME}/**"
      //    }
      // }
      stage('Build Prod Image') {
         steps {
            // sh 'scripts/generate-appversion.sh'
            sh 'docker build --force-rm --compress --pull --build-arg BUILD_TYPE=Release --build-arg ARTIFACTORY_USERNAME=${REGISTRY_CREDS_USR} --build-arg ARTIFACTORY_PASSWORD=${REGISTRY_CREDS_PSW} -t ${IMAGE_NAME} .'
         }
      }
      stage('Push') {
         steps {
            sh 'docker push ${IMAGE_NAME}'
         }
      }
   }
}
