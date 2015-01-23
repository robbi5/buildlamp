![buildlamp](http://buildlamp.org/img/buildlamp-banner.jpg)

## buildlamp — visual build notifications everywhere
* wireless
* programmable
* beautiful

### What we wanted

We wanted a noticeable feedback when some jobs from our Continous Integration (Jenkins) failed. When anything goes horribly wrong, the whole office should be alerted by a light turning red.

### What we got

...is exactly this (and more). Since 2012 we built and tried different lamps with a web interface and wifi connection. The latest versions in this repository are affordable and easy to build.

The lamps are have a HTTP API so you can control them with the [jenkins job](/jenkins) or change the color, let them fade and blink in your own applications.

### What's to find here

In the subfolders you can find different types of buildlamps and a manual to make them Jenkins ready. Additionally, the HTTP API makes it easy to use the lamps for all kind of purposes, like a "do not disturb" lamp, or a lamp which shows your computers cpu temperature or... — you can most likely think of better uses.

* The [Spark Core](/spark) variant — Wifi, Cloud connected
* The [Arduino Ethernet](/arduino-ethernet) variant — Ethernet, local server

Try it out, build your own buildlamp and share what you did via the [issue tracker](https://github.com/robbi5/buildlamp/issues) or tweet us [@buildlamporg](https://twitter.com/buildlamporg). :bulb: 
