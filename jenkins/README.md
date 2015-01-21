### Setup Jenkins Job

Open your Jenkins instance, go to _Manage Jenkins_ > _Manage Plugins_ and install the [_Groovy Postbuild_](https://wiki.jenkins-ci.org/display/JENKINS/Groovy+Postbuild+Plugin) plugin.

Create a new View that contains all the jobs you want to summarize the build status on the lamp.

Create a new Job (type: _Freestyle project_) with following settings:
* Source Code Management: _none_
* Build Triggers: _Build periodically_, Schedule: "`* * * * *`"
* Build > Add Build step: _Execute groovy script_. Copy the contents of [/jenkins/buildlamp.groovy](buildlamp.groovy) in the Groovy command text box and change the `FIXME` values.
