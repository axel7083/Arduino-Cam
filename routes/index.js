var express = require('express');
var router = express.Router();
var path = require('path');
var JsonDB = require('node-json-db');
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

var isPic4Log = false;
var capturePic4Log = true;

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

    /**Arduino functions**/
    if (message.type === 'utf8' && message.utf8Data === 'ESP32') {
      arduino = temp;
      console.log("ESP32 Registred");
    }

    if (message.type === 'utf8' && message.utf8Data === 'Mvt') {  //numéro, jour heure minutes seconde et photo eventuel
      console.log("Movement detected");
      if(capturePic4Log) {
        isPic4Log = true; // We are requesting a picture for the current movement detected!
        arduino.sendUTF("Req");
      }
      else
      {
        var db = new JsonDB(__dirname + '/../json_data/logs',true,true);

        db.push("/logs[]", {
          date: Date.now()
        },true)
      }
    }

    /**Browser fonctions**/
    if (message.type === 'utf8' && message.utf8Data === 'BROWSER') {
      browser = temp;
      console.log("BROWSER Registred");
    }

    if (message.type === 'utf8' && message.utf8Data === 'Req') {
      console.log("Sending Req request to ESP32..");
      arduino.sendUTF("Req");
    }

    if (message.type === 'utf8' && message.utf8Data.includes('Rot')) {
      console.log("Sending Rot request to ESP32..");
      arduino.sendUTF(message.utf8Data);
    }

    if (message.type === 'utf8' && message.utf8Data.includes('Led')) {
      console.log("Sending Led request to ESP32..");
      arduino.sendUTF(message.utf8Data);
    }

    if (message.type === 'utf8' && message.utf8Data.includes('Reb')) {
      console.log("Sending Reb request to ESP32..");
      arduino.sendUTF(message.utf8Data);
    }

    if (message.type === 'utf8' && message.utf8Data.includes('Img')) {
      console.log("Changing Image Capture settings.");
      if(message.utf8Data.includes('1')) //turn it on
      {
        capturePic4Log = true
      }
      else //turn it off
      {
        capturePic4Log = false;
      }

    }


    if (message.type === 'binary') {

      if(!isPic4Log) {
        console.log("Sending data to browser...");
        browser.sendBytes(message.binaryData, message.binaryData.length);
      }
      else
      {
        var db = new JsonDB(__dirname + '/../json_data/logs',true,true);

        db.push("/logs[]", {
          date: Date.now(),
          image: message.binaryData
        },true)
      }
    }
  });

  temp.on('close', function(connection) {
    // close user connection
  });
});


router.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/../public/menu.html'));
});

router.get('/table',function(req,res){
  res.sendFile(path.join(__dirname+'/../public/table.html'));
});

router.get('/tablejs',function(req,res){
  res.sendFile(path.join(__dirname+'/../json_data/logs.json'));
});

router.get('/test',function(req,res){
  console.log(req.body);
  res.sendStatus(200);
  arduino.sendUTF("Req");
});



module.exports = router;
