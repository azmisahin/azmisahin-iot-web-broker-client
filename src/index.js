// required when running on node.js
var mqtt = require("mqtt");

var client = mqtt.connect("mqtt://8ac98f73:7ae7c0ecf6148c3a@broker.shiftr.io", {
  clientId: "javascript",
});

client.on("connect", function () {
  console.log("client has connected!");

  client.subscribe("/iot-status");

  setInterval(function () {
    client.publish("/iot-sensor", "check");
  }, 1000);
});

client.on("message", function (topic, message) {
  console.log("new message:", topic, message.toString());
});
