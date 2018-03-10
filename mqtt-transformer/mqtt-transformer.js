/* Forwards messages from all available MQTT channels to InfluxDB. */

// CONFIGURATION

var broker_address = 'mqtt://mqtt.local' /* name of your MQTT server */
var influx_hostname = 'influxdb.local' /* name of your InfluxDB server */
var influx_db = 'influx_db' /* name of your InfluxDB database */

var mdns_service_name = 'mqtt2slack'
var http = require('http');

// MDNS

var mqtt = require('mqtt')
var client

function process_message(topic, message) {
	var datetime = new Date()
  if (message.indexOf("disconnected") !== -1 ) return;
	console.log("topic: " +topic+ ", message: " + message.toString())
  var dict = JSON.parse(message.toString());
  dict = JSON.parse(message);
  if (typeof(dict.humidity) !== "undefined") {
    influx('humidity,location=' + dict.shortIdentifier + ' value=' + dict.humidity);
  }
}

function init_mqtt(broker_address, channel) {
	console.log("MQTT connecting to: " + broker_address)
	client = mqtt.connect(broker_address)
	client.on('connect', function () {
		console.log("MQTT subscribing to all available channels...")
		client.subscribe("#");
	})
	client.on('message', function (topic, message) {
	  // message is Buffer
	  process_message(topic, message)
	})
}

function influx(data) {
  var http = require('http');
	var options = {
  		host: influx_hostname,
		port: '8086',
		path: '/write?db='+influx_db,
		method: 'POST'
	};
	callback = function(response) {
	var str = ''
	  response.on('data', function (chunk) {
	    str += chunk;
	  });
    response.on('error', function (error) {
	    console.log(error);
	  });
	  response.on('end', function () {
	    console.log(str);
	  });
	};
	var flux = http.request(options, callback);
	flux.write(data);
	flux.end();
  console.log("» Influx: " + data);
}

console.log("Initializing " + broker_address + " MQTT broker");
init_mqtt(broker_address, "#");

process.on('uncaughtException', function(err) {
  console.log('Caught exception: ' + err);
});
