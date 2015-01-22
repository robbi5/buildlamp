def jenkinsViewName = "FIXME";
def buildlampVariant = "FIXME"; // use SPARK or ARDUINO
// if your buildlamp is the spark variant:
def sparkDeviceId = "FIXME";
def sparkAccessToken = "FIXME";
// if your buildlamp is the arduino variant:
def arduinoIP = "FIXME";

def build = Thread.currentThread().executable;
println("buildLamp system Groovy script");
def lampResult = hudson.model.Result.SUCCESS;
def somethingIsBuilding = false;
def badProjectsMap = [:];
if (buildlampVariant != "SPARK" && buildlampVariant != "ARDUINO") {
  println("FAILURE: buildlampVariant is undefined. Please look at the Groovy Source again.");
  return;
}
println("-------");

hudson.model.Hudson.instance.getView(jenkinsViewName).getItems().each { i ->
  print(i.getName());

  if (i.class.getName().is("hudson.maven.MavenModule")) {
    println(" [IGNORED] type not supported");
    return;
  }
  
  if (i.getName().equals(build.getProject().getName())) {
    println(" [IGNORED] it's me!");
    return;
  }
  
  if (i.disabled) {
    println(" [DISABLED]");
    return;
  }
  
  if (i.getLastBuild().isBuilding()) {
    somethingIsBuilding = true;
    println(" [BUILDING]");
    return;
  }

  def jobResult = i.getLastBuild().getResult();
  println(" [" + jobResult + "]");

  if (jobResult == "ABORTED") {
    return;
  }

  def prevLampResult = lampResult;
  lampResult = lampResult.combine(jobResult);
  if (lampResult != prevLampResult) {
    badProjectsMap[i.getName()] = jobResult;
  }
}
println("-------");
print("lampResult: "); println(lampResult);
print("somethingIsBuilding: "); println(somethingIsBuilding);

def lampResultStr = "SUCCESS";

build.setResult(lampResult);
if (lampResult == hudson.model.Result.SUCCESS) {
  lampResultStr = "SUCCESS";
} else if (lampResult == hudson.model.Result.UNSTABLE) {
  lampResultStr = "UNSTABLE";
} else if (lampResult == hudson.model.Result.FAILURE) {
  lampResultStr = "FAILURE";
}

if (badProjectsMap.size() > 0) {
  println("-------");
  println("Projects that changed the Lamp Status:")
  badProjectsMap.each {
    println("  $it.key: $it.value")
  }
  println("-------");
}

def phase = somethingIsBuilding ? "STARTED" : "FINISHED";
def reqData = "&args=${lampResultStr},${phase}";
if (buildlampVariant == "SPARK") {
  reqData = "&access_token=" + sparkAccessToken + reqData;
}

print("sending Data: ");
println(reqData);

def buildlampUrl = "";
if (buildlampVariant == "SPARK") {
  buildlampUrl = "https://api.spark.io/v1/devices/" + sparkDeviceId + "/ci";
} else if (buildlampVariant == "ARDUINO") {
  buildlampUrl = "http://" + arduinoIP + "/ci";
}

def url = new URL(buildlampUrl);
def connection = url.openConnection();
connection.connectTimeout = 10000 ;
connection.setRequestMethod("POST");
connection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
connection.doOutput = true;
def writer = new OutputStreamWriter(connection.outputStream);
writer.write(reqData);
writer.close();
connection.connect();
def postResponse = connection.content.text;

println("Response:");
println(postResponse);
