var MQTT_MISC_TOOLS_VERSION = {
    major: 0,
    minor: 0,
    revison: 1
};

/* Debug text */
//console.log = function() {};

function include(filename) {
	var head = document.getElementsByTagName('head')[0];
	script = document.createElement('script');
	script.src = filename;
	script.type = 'text/javascript';
	head.appendChild(script)
}

include('./libs/jquery-2.2.1.min.js');
include('./libs/mqttws31.js');

MQTT_MISC_TOOLS = (function (global) {
    /* Broker settings */
    var broker_mqtt = {
        url: "test.mosquitto.org",
        port: 8080
    };
    var connected = false;
    var clear_retained_check = false;
    var connect_options = {
        timeout: 3,
        onSuccess: function () {
            console.log("MQTT Tools Connected as: "+client._getClientId());
            connected = true;
            subscribe_list();// Connection succeeded; subscribe to  topics
        },
        onFailure: function (message) {
            connected = false;
            console.log("MQTT Tools Connection failed: " + message.errorMessage);
        }
    };
    var client; // MQTT Client
    /* Init */
    var init = function () {
        welcome();
        connect_MQTT();
        return 0;
    };
    /* stops the DNS */
    var deinit = function () {
        disconnect_MQTT();
    };
    /* Connect MQTT */
    var connect_MQTT = function () {
        if(connected) {
            console.log("Already connected");
            return;
        };
        client = new Paho.MQTT.Client(broker_mqtt.url, broker_mqtt.port,generate_name());
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
        console.log("MQTT Tools v"+window.MQTT_MISC_TOOLS_VERSION.major+"."+window.MQTT_MISC_TOOLS_VERSION.minor+"."+window.MQTT_MISC_TOOLS_VERSION.revison);
    };
    /* Call when mess is recieved */
    var message_recieve = function (message) {
        if (clear_retained_check) {
            if (message.payloadString) {
                clear_retained(message.destinationName);
            }
        }
        console.log("Got message from: "+message.destinationName+" | "+message.payloadString);
    };
    var send = function (topic, payload, retain = false) {
        if(!(typeof topic=='string')){
            console.log('Wrong topic!');
            return -1;
        }
        message = new Paho.MQTT.Message(payload);
        message.destinationName = topic;
        message.retained = retain;
        client.send(message);
    };

    var subscribe = function (topic) {
        client.subscribe(topic, {qos: 1});//qos Quality of Service
    };
    var unsubscribe = function (topic) {
        client.unsubscribe(topic, {});
    };
    /* Initial subscribed topics */
    var subscribe_list = function () {
        //
    };

    var clear_retained = function (topic) {
        //make client
        send(topic, '', true);
        console.log("DELETE!");
    };

    var spam = function (topic, nr_of_times) {
        for (var i = 0; i < nr_of_times; i++) {
            send(topic, rand(0, 1000).toString());
        }
    }

    /* Random gen */
    function rand(min,max) {
        return Math.floor(Math.random()*(max-min+1)+min);
    };
    var generate_name = function () {
        return "mqtt_tools_"+parseInt(rand(1, 999));
    };
    return {
        init: init,
        deinit: deinit,
        subscribe, subscribe,
        clear_retained: clear_retained,
        clear_retained_check: clear_retained_check,
        spam: spam,
        send: send
    };
})(window);
