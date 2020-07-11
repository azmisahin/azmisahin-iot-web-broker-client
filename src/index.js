// MQTT Library
var mqtt = require("mqtt");

// Broker address is being restructured according to the current environment.
var MQTT_URL = process.env.MQTT_URL || "mqtt://broker-test.kiwimobility.com";

// Creating unique identity.
var clientId = "app-" + Math.random().toString(16).substr(2, 8);

// Connection definitions are being configured.
var options = {
  port: 1883,
  clientId: clientId,
};

// Router
var hub = "iot";
var topics = ["iot/1001", "iot/1002", "iot/1003"];

// Create a client connection
var client = mqtt.connect(MQTT_URL, options);

/**
 * Socket Event Handler
 * @param {socket} client Client Information
 * @param {array} topic Topic List
 */
function eventHandler(client, topic) {
  // subscribe to a topic
  client.subscribe(topic, function () {
    // when a message arrives, do something with it
    client.on("message", function (topic, data, packet) {
      console.log("topic:", topic);
      console.log("data:", data.toString());
      //console.info("packet:", packet);
      console.log("time:", new Date().toUTCString());
      console.log("==================================================");
    });
  });
}

// When connected
client.on("connect", function () {
  // Main Topic = hub
  eventHandler(client, hub);

  // topics.forEach((topic) => {
  //   // All topic
  //   eventHandler(client, topic);
  // });
});

/**
 * Send Data
 */
function sendData(topic, data) {
  // Send
  client.publish(topic, data, () => {});
}

sendData(hub + "/1001", "1");
