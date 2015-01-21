# buildlamp — Spark Core and NeoPixels variant
## Required Parts

* [Spark Core Prototyping Bundle](https://store.spark.io/?product=prototyping-bundle) (includes breadboard, also gets you an Spark Photon for the next generation buildlamp)
* [5v USB power supply](http://www.adafruit.com/products/501)
* [NeoPixel Stick](http://www.adafruit.com/products/1426) or multiple [breadboard friendly NeoPixels](http://www.adafruit.com/products/1312)
* [Break-away headers](http://www.adafruit.com/products/392) for soldering on neopixels
* [breadboard wires](http://www.adafruit.com/products/153)

## Instructions

### Preparing the NeoPixels
* Solder the break-away headers on your breadboard friendly NeoPixels or the NeoPixel Stick

### Assemble on Breadboard
![spark-neopixel-breadboard](https://cloud.githubusercontent.com/assets/172415/5819579/32925982-a0bf-11e4-972f-549e74845e50.png) ![spark-neopixel-453](https://cloud.githubusercontent.com/assets/172415/5835355/d44bec72-a167-11e4-91bf-c99b1a6cf410.jpg)

### Setup your Spark Core
Visit https://www.spark.io/start and proceed until you get to "Put Code on Your Core"

### Upload buildlamp code on the Spark Core
In the Spark Web IDE, press the "Create new App" button and give it a name.
Then copy the latest code from [/spark/buildlamp.ino](../spark/buildlamp.ino). 

Now you have to add the NeoPixels library: Click on Libraries, search for NeoPixel, press the "Include in App" button and select your app.

The App is now ready, you can push the code with the button in the top left.

### Get Spark Core ID and Access Token

In the Spark Web IDE, click on "Cores" in the bottom left. Look for your core, click on ">" and get the _Device ID_.

Then go to Settings and copy the _Access Token_.

### Setup Jenkins Job

Open your Jenkins instance, go to _Manage Jenkins_ > _Manage Plugins_ and install the [_Groovy Postbuild_](https://wiki.jenkins-ci.org/display/JENKINS/Groovy+Postbuild+Plugin) plugin.

Create a new View that contains all the jobs you want to summarize the build status on the lamp.

Create a new Job (type: _Freestyle project_) with following settings:
* Source Code Management: none
* Build Triggers: Build periodically, Schedule: "`* * * * *`"
* Build > Add Build step: Execute groovy script. Copy the contents of [/jenkins/buildlamp.groovy](../jenkins/buildlamp.groovy) in the Groovy command text box and change the `FIXME` values.

### Look at your lamp

If its no longer blue, *party*! :tada: 
