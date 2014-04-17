// Run this to capture a stream of png images.

var arDrone = require('ar-drone');
var cv = require('opencv');

var options = {}
var client = arDrone.createClient(options);
client.disableEmergency();
var pngStream = client.getPngStream();
var processingImage = false;
var lastPng;
var navData;
var startTime = new Date().getTime();
var log = function(s){
  var time = ( ( new Date().getTime() - startTime ) / 1000 ).toFixed(2);
  console.log(time+" \t"+s);
}

var random = new Date().getTime()
var pngCount = 0

pngStream
.on('error', function(err) {
  console.log("There was an error: " + err)
})
.on('data', function(pngBuffer) {
  cv.readImage(pngBuffer, function(err, im) {
    im.save(random + "-" + pngCount)
    pngCount++
  }); // End readImage
});
