var express = require('express');
var router = express.Router();
var path = require('path');
const delay = require('delay');

var WebSocketServer = require('websocket').server;
var http = require('http');

var server = http.createServer(function(request, response) {
  // process HTTP request. Since we're writing just WebSockets
  // server we don't have to implement anything.
});
server.listen(81, function() { });


var browser; //websocket client to browser(s)
var arduino; //websocket client of arduino


// create the server
wsServer = new WebSocketServer({
  httpServer: server
});

// WebSocket server
wsServer.on('request', function(request) {
  var temp  = request.accept(null, request.origin);

  // This is the most important callback for us, we'll handle
  // all messages from users here.
  temp.on('message', function (message) {
    console.log(message);

    if (message.type === 'utf8' && message.utf8Data === 'ESP32') {
        arduino = temp;
        console.log("ESP32 Registred");
    }

    if (message.type === 'utf8' && message.utf8Data === 'BROWSER') {
      browser = temp;
      console.log("BROWSER Registred");
    }

    if (message.type === 'utf8' && message.utf8Data === 'Req') {
      console.log("Sending request to ESP32..");
      arduino.sendUTF("Req");
    }

    if (message.type === 'binary') {
      console.log("Sending data to browser...");
      browser.sendBytes(message.binaryData,message.binaryData.length);
    }
  });

  temp.on('close', function(connection) {
    // close user connection
  });
});


router.get('/',function(req,res){

  res.sendFile(path.join(__dirname+'/../public/websocket-ESP32.html'));

});

router.get('/test',function(req,res){
  console.log(req.body);
  res.sendStatus(200);
  arduino.sendUTF("Req");
});



module.exports = router;
