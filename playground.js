// Run this to receive a png image stream from your drone.

var arDrone = require('ar-drone');
var cv = require('opencv');
var http    = require('http');
var fs = require('fs');

drone_ip = "192.168.42.2"
var options = {ip:drone_ip};
//var options = {}
var client = arDrone.createClient(options);
//client.takeoff()
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

pngStream
.on('error', function(err) {
  console.log("There was an error: " + err)
})
.on('data', function(pngBuffer) {
  console.log("her");
  lastPng = pngBuffer;
});

var i = 0;
var server = http.createServer(function(req, res) {
  // SAVE ONE IN EVERY 5 of these to do recognition on.
  if (i % 5 == 0) {
    cv.readImage( lastPng, function(err, im) {
      im.save("./image_dir/img.jpg");
    }); // End readImage
  }
  i++

  if (!lastPng) {
    res.writeHead(503);
    res.end('Did not receive any png data yet.');
    return;
  }

  res.writeHead(200, {'Content-Type': 'image/png'});
  res.end(lastPng);
});

server.listen(5555, function() {
  console.log('Serving latest png on port 5555 ...');
});
