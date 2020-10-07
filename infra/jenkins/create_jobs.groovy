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

def jsonParser = new groovy.json.JsonSlurper()

// Jenkins job-builder's parameter (string)
//
// If below parameters are not defined or empty, use default setting in conf.json
//
// TARGET_REPO_HTTPS: build target repo https
// TARGET_GIT_URL: build target git url.
// TARGET_BRANCH: remote branch name to test
// BUILD_CREDENTIAL: jenkins credential for git clone

// Read configuration file and get job list and repo information
currentPath = 'infra/jenkins/'
def jsonConf = readFileFromWorkspace(currentPath + 'conf.json')
println "raw conf : $jsonConf"
def jobs = jsonParser.parseText(jsonConf)['jobs']
def repo = jsonParser.parseText(jsonConf)['repo']

// Get base information from job parameter
// Otherwise, use repo information from conf.json
if (isVarExists('TARGET_REPO_HTTPS'))
{
  read_repoHttps = getEnvString('TARGET_REPO_HTTPS')
  if (read_repoHttps != "")
  {
    repo.repoHttps = read_repoHttps
  }
}
if (isVarExists('TARGET_BRANCH'))
{
  read_branch = getEnvString('TARGET_BRANCH')
  if (read_branch == "")
  {
    repo.branch = getEnvString('GIT_BRANCH')
  }
  else
  {
    repo.branch = read_branch
  }
}
if (isVarExists('BUILD_CREDENTIAL'))
{
  read_credential = getEnvString('BUILD_CREDENTIAL')
  if (read_credential != "")
  {
    repo.credential = read_credential
  }
}

println "target branch : $repo.branch"
println "target repo https: $repo.repoHttps"

// -------------------------------------------------------------
// Define static & local functions

def isVarExists(def nameStr) {
  return (getBinding().hasVariable(nameStr))
}

def getEnvString(def nameStr) {
  if(isVarExists(nameStr))
  {
    return getBinding().getVariable(nameStr)
  }
  else
  {
    println "Cannot find parameter " + nameStr + ". You should set job parameter"
    assert false
  }
}

def setPropertyConfig(def job, def prop, def repo)
{
  def blocker = prop.buildBlockers?:false
  def preGroovy = prop.groovyBeforeBuild?:false
  def preEnvSet = ""

  preEnvSet = preEnvSet + "repo_gitUrl=" + repo.repoHttps + ".git\n"
  preEnvSet = preEnvSet + "repo_credential=" + repo.credential + "\n"
  preEnvSet = preEnvSet + "repo_branch=" + repo.branch

  job.with {
    properties {
      githubProjectProperty {
        projectUrlStr(repo.repoHttps)
      }

      if (blocker)
      {
        buildBlockerProperty {
          blockingJobs(blocker.join('\n'))
          blockLevel('GLOBAL')
          scanQueueFor('DISABLED')
          useBuildBlocker(true)
        }
      }

      if (!(prop.concurrentBuild?:false))
      {
        disableConcurrentBuilds()
      }

      if (preGroovy)
      {
        envInjectJobProperty {
          keepBuildVariables(true)
          keepJenkinsSystemVariables(true)
          overrideBuildParameters(false)
          on(true)
          info {
            propertiesFilePath('')
            propertiesContent(preEnvSet)
            scriptFilePath('')
            scriptContent('')
            loadFilesFromMaster(false)
            secureGroovyScript {
              script(preGroovy ? readFileFromWorkspace(currentPath + preGroovy) : '')
              sandbox(true)
            }
          }
        }
      }
    }
  }
}

def setJobParameter(def job, def prop)
{
  if (prop.stringParams?:false || prop.fileParams?:false)
  {
    job.with {
      parameters {
        if( (prop.stringParams?:false) )
          {
            paramMap = prop.stringParams;
            if( paramMap )
              {
                paramMap.each { name, value ->
                stringParam(name, value, '')
              }
            }
          }
          if( (prop.fileParams?:false) )
          {
            prop.fileParams.each { fileName ->
            fileParam(fileName, '')
          }
        }
      }
    }
  }
}

def setBaseConfig(def job, def prop, def repo)
{
  job.with {
    logRotator(prop.daysToKeep?:7, prop.numToKeep?:-1, prop.artifactDaysToKeep?:-1, prop.artifactNumToKeep?:-1)
  }

  // job description
  if (prop.description?:false)
  {
    job.with {
      description(prop.description)
    }
  }

  // Set job.property configuration
  setPropertyConfig(job, prop, repo)

  // Set job parameter with default value
  setJobParameter(job, prop)
}

def setTrigger(def job, def prop, def gitBranch)
{
  job.with {
    triggers {
      if (prop.gitHubPushTrigger?:false)
      {
        gitHubPushTrigger()
      }

      if (prop.periodicTriggered?:false) {
        cron("${prop.periodicTriggered.period}")
      }

      if (prop.upstreamTrigger?:false)
      {
        // Condition: SUCCESS, UNSTABLE, FAILURE
        upstream("${prop.upstreamTrigger.job}", "${prop.upstreamTrigger.condition}")
      }

      if (prop.ghprbTrigger?:false)
      {
        def conf = prop.ghprbTrigger
        ghprbTrigger {
          buildDescTemplate(conf.buildDesc)
          triggerPhrase(conf.triggerPhrase?:'')
          useGitHubHooks(true)
          permitAll(true)
          adminlist('')
          whitelist('')
          orgslist('')
          cron('H/5 * * * *')
          onlyTriggerPhrase(conf.onlyTriggerPhrase?:false)
          autoCloseFailedPullRequests(false)
          displayBuildErrorsOnDownstreamBuilds(true)
          commentFilePath('')
          skipBuildPhrase(conf.skipBuildPhrase)
          whiteListTargetBranches {
            ghprbBranch {
              branch(gitBranch)
            }
          }
          blackListCommitAuthor('')
          allowMembersOfWhitelistedOrgsAsAdmin(false)
          blackListLabels(conf.blackListLabels?:'')
          whiteListLabels('')
          gitHubAuthId('')
          includedRegions('')
          excludedRegions(conf.excludeFilesRegex?:'')
          commitStatusContext(conf.displayName)
          extensions {
            if( conf.buildStatus?:false )
            {
              ghprbBuildStatus {
                messages {
                  conf.buildStatus.each { res, msg ->
                    ghprbBuildResultMessage {
                      result(res)
                      message(msg)
                    }
                  }
                }
              }
            }
            ghprbSimpleStatus {
              commitStatusContext(conf.displayName)
              showMatrixStatus(false)
              statusUrl('')
              triggeredStatus('')
              startedStatus('')
              addTestResults(false)
              msgSuccess('')
              msgFailure('')
              if( conf.completedStatus?:false )
              {
                completedStatus {
                  conf.completedStatus.each { res, msg ->
                    ghprbBuildResultMessage {
                      result(res)
                      message(msg)
                    }
                  }
                }
              }
            }
          }
        }
      }

      if (prop.pollSCM?:false)
      {
        def conf = prop.pollSCM
        pollSCM {
          scmpoll_spec(conf.schedule)
          ignorePostCommitHooks(conf.ignoreCommit?:true)
        }
      }
    }
  }
}

// -------------------------------------------------------------

// Get folder name to check test jobs
def createJobName = getEnvString('JOB_NAME')
def isTestFolder = false
if (createJobName.find('/test/'))
{
  println "Generate jobs in test folder"
  isTestFolder = true
}

// Generate jobs
jobs.each { jobConf ->
  // Read configuration file for each job
  def jsonJobProperty = jsonParser.parseText(readFileFromWorkspace(currentPath + jobConf))

  println "generate " + jobConf

  // Create newJob object
  def cmd = ""
  jsonJobProperty.scriptfile.each{ fileName -> cmd += (readFileFromWorkspace(currentPath + fileName)+'\n') }
  def newJob = pipelineJob(jsonJobProperty.name) {
    definition {
      cps {
        sandbox()
        script(cmd)
      }
    }
  }

  if (repo.containsKey("jobDisabled") && repo.jobDisabled.containsKey(jsonJobProperty.name))
  {
    newJob.with {
      disabled(repo.jobDisabled[jsonJobProperty.name])
    }
  }
  if (isTestFolder)
  {
    newJob.with {
      disabled(true)
    }
  }
  if(jsonJobProperty.disable?:false)
  {
    newJob.with {
      disabled(true)
    }
  }

  // Set job base configuration
  setBaseConfig(newJob, jsonJobProperty, repo)

  // Set trigger configuration
  setTrigger(newJob, jsonJobProperty, repo.branch)
}

def helpMessage = readFileFromWorkspace(currentPath + 'pr-help.md')
def helpJob = job('pr-help') {
  properties {
    githubProjectProperty {
      projectUrlStr(repo.repoHttps)
    }
  }
  logRotator(-1, 1, -1, -1)
  label('master')
  triggers {
    ghprbTrigger {
      buildDescTemplate('help message')
      triggerPhrase('@nnfw-bot(?i)\\W+help.*')
      useGitHubHooks(true)
      permitAll(true)

      adminlist('')
      whitelist('')
      orgslist('')
      cron('H/5 * * * *')
      onlyTriggerPhrase(true)
      autoCloseFailedPullRequests(false)
      displayBuildErrorsOnDownstreamBuilds(true)
      commentFilePath('')
      skipBuildPhrase('')
      whiteListTargetBranches {
        ghprbBranch {
          branch(repo.branch)
        }
      }
      blackListCommitAuthor('')
      allowMembersOfWhitelistedOrgsAsAdmin(false)
      blackListLabels('')
      whiteListLabels('')
      gitHubAuthId('')
      includedRegions('')
      excludedRegions('')

      commitStatusContext('help-command')

      extensions {
        ghprbBuildStatus {
          messages {
            ghprbBuildResultMessage {
              result('SUCCESS')
              message(helpMessage)
            }
          }
        }
        ghprbSimpleStatus {
          commitStatusContext('help-command')
          showMatrixStatus(false)
          statusUrl('')
          triggeredStatus('')
          startedStatus('')
          addTestResults(false)
          msgSuccess('')
          msgFailure('')
        }
      }
    }
  }

  if (isTestFolder)
  {
    disabled(true)
  }
}
