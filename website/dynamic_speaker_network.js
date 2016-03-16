var DNS_VERSION = {
    major: 0,
    minor: 0,
    revison: 1
};

function include(filename) {
	var head = document.getElementsByTagName('head')[0];
	script = document.createElement('script');
	script.src = filename;
	script.type = 'text/javascript';
	head.appendChild(script)
}

include('./libs/jquery-2.2.1.min.js');
include('./libs/mqttws31.js');

DNS = (function (global) {
    /* Broker settings */
    var broker_mqtt = {
        url: "test.mosquitto.org",
        port: 8080
    };
    var connected = false;
    var connect_options = {
        timeout: 3,
        onSuccess: function () {
            console.log("MQTT Connected as: "+client._getClientId());
            connected = true;
            subscribe_tmp();// Connection succeeded; subscribe to our topic
        },
        onFailure: function (message) {
            connected = false;
            console.log("MQTT Connection failed: " + message.errorMessage);
        }
    };
    /* Topic settings */
    var topic = {
        root: "ESEiot/DNS/"
    };
    var client; // MQTT Client
    /* start the DNS */
    var start = function () {
        welcome();
        connect_MQTT();
        return 0;
    };
    /* stops the DNS */
    var stop = function () {
        disconnect_MQTT();
    };
    /* Connect MQTT */
    var connect_MQTT = function () {
        if(connected) {
            console.log("Already connected");
            return;
        };
        client = new Messaging.Client(broker_mqtt.url, broker_mqtt.port,generate_name());
        client.onMessageArrived = message_recieve;
        client.onConnectionLost = conn_lost_MQTT;
        client.connect(connect_options);
    };
    /* Disconnect MQTT */
    var disconnect_MQTT = function () {
        client.disconnect();
        connected = false;
    };
    /* Connection lost MQTT */
    var conn_lost_MQTT = function (responseObject) {
        console.log("connection lost: " + responseObject.errorMessage);
    };
    /* Welcome message */
    var welcome = function () {
        console.log("DNS v"+window.DNS_VERSION.major+"."+window.DNS_VERSION.minor+"."+window.DNS_VERSION.revison);
    };
    /* Call when mess is recieved */
    var message_recieve = function (message) {
        console.log(message.destinationName+"   "+message.payloadString);
    };

    /* TEMP SUB */
    var subscribe_tmp = function () {
        client.subscribe(topic.root+'#', {qos: 1});//qos Quality of Service
    };
    /* Random gen */
    function rand(min,max) {
        return Math.floor(Math.random()*(max-min+1)+min);
    };
    var generate_name = function () {
        return "site_"+parseInt(rand(1, 999));
    };
    return {
        start: start,
        stop: stop
    };
})(window);
