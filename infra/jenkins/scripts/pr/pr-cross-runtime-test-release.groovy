/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Environment var      |  conf.json
// ----------------------------------------
// env.repo_branch      |
// env.repo_credential  |  repo.credential
// env.repo_gitUrl      |  repo.gitUrl
// env.repo_httpsUrl    |  repo.httpsUrl

pipeline {
  agent none

  options {
    timeout(time:3, unit: 'HOURS')
  }

  stages {
    stage('build') {
      agent {
        docker {
          label 'builder'
          image 'nnfw/nnas'
          args '--privileged -v nnfw-${EXECUTOR_NUMBER}-externals:/externals -v nnfw-${EXECUTOR_NUMBER}-rootfs:/rootfs'
        }
      }

      environment {
        EXTERNAL_DIR = "/externals"
        ROOTFS_DIR = "/rootfs/arm.xenial"
        TARGET_ARCH = "armv7l"
        BUILD_TYPE = "release"
        CROSS_BUILD = 1
        NPROCS = 4
      }

      stages {
        stage('checkout') {
          steps {
            checkout([
              $class: 'GitSCM',
              branches: [[name: "${ghprbActualCommit}"]],
              userRemoteConfigs: [[
                name: "origin",
                credentialsId: "${env.repo_credential}",
                url: "${env.repo_gitUrl}",
                refspec: ('+refs/pull/*:refs/remotes/origin/pr/*')
              ]]
            ])
          }
        }

        stage('prepare rootfs') {
          steps {
            sh '''#!/bin/bash

if [[ ! -d $ROOTFS_DIR ]]; then
  mkdir rootfs
  wget -nv http://npu.mooo.com/archive/rootfs/rootfs_arm_ubuntu_xenial_20190826.tar.gz -O rootfs.tar.gz
  tar -zxf rootfs.tar.gz -C rootfs/
  mv rootfs/arm $ROOTFS_DIR
  rm rootfs.tar.gz
fi
            '''
          }
        }

        stage('build') {
          steps {
            sh '''#!/bin/bash

# for internal server
EXTERNAL_DOWNLOAD_SERVER=http://npu.mooo.com/archive/sources
make -f Makefile.template all install build_test_suite

            '''

            archiveArtifacts artifacts: 'Product/out/test-suite.tar.gz'
            stash name: "test-suite", includes: "Product/out/test-suite.tar.gz"
          }
        }
      }
    }

    stage('test') {
      environment {
        MODELFILE_SERVER = "http://npu.mooo.com/archive/tflite_test_model"
      }

      parallel {
        stage('acl_cl') {
          agent { label 'xu4_test_slave' }

          steps {
          step([$class: 'WsCleanup'])
          unstash "test-suite"
          sh '''#!/bin/bash

tar -zxf Product/out/test-suite.tar.gz -C ./
./infra/scripts/test_ubuntu_runtime.sh --backend acl_cl --tflite-loader

            '''
          }
        }

        stage('acl_neon') {
          agent { label 'xu4_test_slave' }

          steps {
            step([$class: 'WsCleanup'])
            unstash "test-suite"
            sh '''#!/bin/bash

tar -zxf Product/out/test-suite.tar.gz -C ./
./infra/scripts/test_ubuntu_runtime.sh --backend acl_neon

            '''
          }
        }

        stage('cpu') {
          agent { label 'xu4_test_slave' }

          steps {
            step([$class: 'WsCleanup'])
            unstash "test-suite"
            sh '''#!/bin/bash

tar -zxf Product/out/test-suite.tar.gz -C ./
./infra/scripts/test_ubuntu_runtime.sh --backend cpu

            '''
          }
        }

        stage('mixed') {
          agent { label 'xu4_test_slave' }

          steps {

            step([$class: 'WsCleanup'])
            unstash "test-suite"
            sh '''#!/bin/bash

tar -zxf Product/out/test-suite.tar.gz -C ./
/infra/scripts/test_ubuntu_runtime_mixed.sh

            '''
          }
        }
      }
    }
  }
}
