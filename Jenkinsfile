node('builder'){
    docker.image('riddleandcode/wallet-builder').inside('--privileged') {
        checkout scm
        version = readFile "${env.WORKSPACE}/version"
        stage('Generating build') {
            sh 'mkdir -p build && cd build && cmake ../  -DCMAKE_TOOLCHAIN_FILE=/opt/toolchain-rpi.&& cmake -DTARGET_GROUP=test '
        }
        dir('build')
        {
            stage('Build') {
                sh 'make'
            }
            /*
            stage('Testing') {
                sh 'make check'
                sh 'xsltproc /opt/ctest/ctest2junix.xsl tests/Testing/$(head -1 tests/Testing/TAG)/Test.xml > CTestResults.xml '
                junit 'CTestResults.xml'
                cobertura coberturaReportFile: 'coverage.xml'
            }
            */
            stage("Upload to Artifactory"){
                rtUpload (
                    serverId: "ArtifactoryOSS", // Obtain an Artifactory server instance, defined in Jenkins --> Manage:
                    spec: """{
                        "files": [
                            {
                                "pattern": "src/libataes.a",
                                "target": "generic-local/release-candidate/ataes132/${version}/libataes.a"
                            },
                            {
                                "pattern": "src/libataesdyn.so",
                                "target": "generic-local/release-candidate/ataes132/${version}/libataes.so"
                            }
                        ]
                    }"""
                )
                rtPublishBuildInfo (
                    serverId: "ArtifactoryOSS"
                )
           }
        }
    }
}
