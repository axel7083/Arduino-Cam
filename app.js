var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
var bodyParser = require("body-parser");

var indexRouter = require('./routes/index.js');

var app = express();

express.static('public');


const location_hide = require('location-hide');
path = require('path'),
   // filePath = path.join(__dirname, 'public/html/index.html'); // <-- FILE IMPORT HERE
fs = require('fs');
randomize = require('randomatic');


app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());
// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

app.enable('trust proxy');
//app.use(logger('dev'));
//app.use(express.json());
//app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
//app.use(express.static(path.join(__dirname, '')));

//app.use(express.static(path.join(__dirname, '/')));
app.use(express.static('/public'));
console.log("__dirname" + __dirname);
app.use(express.static(path.join(__dirname, '/public')));

app.disable('x-powered-by');

app.use('/', indexRouter);



// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler with detail
/**
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});**/

// error handler without detail
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});



module.exports = app;


