def sparkDeviceId = "FIXME";
def sparkAccessToken = "FIXME";
def jenkinsViewName = "FIXME";

def build = Thread.currentThread().executable;
println("buildLamp system Groovy script");
def lampResult = hudson.model.Result.SUCCESS;
def somethingIsBuilding = false;
def badProjectsMap = [:];
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
def reqData = "&access_token=" + sparkAccessToken + "&args=${lampResultStr},${phase}";

print("sending Data: ");
println(reqData);

def url = new URL("https://api.spark.io/v1/devices/" + sparkDeviceId + "/ci");
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
