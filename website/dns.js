/*
 * Dynamic Network of Speakers
 *
 * A dynamic network of Speakers
 * With MQTT
 *
 */

function include(filename) {
    if (document.querySelectorAll('[src="' + filename + '"]').length > 0 > 0) {
        return;
    }
	var head = document.getElementsByTagName('head')[0];
	script = document.createElement('script');
	script.src = filename;
	script.type = 'text/javascript';
	head.appendChild(script);
}

include('./gui.js');
include('./devices.js');
include('./objects.js');

DNS = (function (global) {
    /* Broker settings */
    // Check broker CONFIG
    if (CONFIG.use_broker < 1 || CONFIG.use_broker > Object.keys(CONFIG.broker).length) {
        console.log("Config error, bad USE_BROKER parameter. Using default broker instead");
        CONFIG.use_broker = 1;
    }
    var broker_mqtt = {
        url: CONFIG.broker[CONFIG.use_broker].uri,
        port: CONFIG.broker[CONFIG.use_broker].port_ws,
        client_name: ''
    };
    /* Random name generator */
    function rand(min,max) {return Math.floor(Math.random()*(max-min+1)+min);};
    var generate_name = function () {
        return CONFIG.name_website+parseInt(rand(1, 999));
    };
    broker_mqtt.client_name=generate_name(); // Set site name
    var connected = false; // Connection status
    var connect_options = {
        timeout: 3,
        onSuccess: function () {
            console.log("MQTT Connected as: "+client._getClientId());
            console.log("MQTT Connected with: "+CONFIG.broker[CONFIG.use_broker].broker);
            connected = true;
            subscribe_list();// Connection succeeded; subscribe to our topics
            DNS.send(DNS.topic("request_online"), '1');    //get initial devices
            DNS.send(DNS.topic("request_client_data"), topic("answer_site"));    //get initial devices parameters
            //update device info
        },
        onFailure: function (message) {
            connected = false;
            console.log("MQTT Connection failed: " + message.errorMessage);
        }
    };
    /* MQTT Topics (See: topic spec sheet and config file)*/
    var topic = function (topic_name) {
        var topic_root = "root";
        if(isEmpty(topic_name)){
            console.log("Topic request was empty");
            throw "Topic request was empty";
            return 0;
        }
        if(isEmpty(CONFIG.topics[topic_root])){
            console.log("The root topic: \""+topic_root+"\" was not found in the config");
            throw "The root topic: \""+topic_root+"\" was not found in the config";
            return 0;
        }
        if (topic_name == topic_root) {
            return CONFIG.topics[topic_root];
        }
        if(isEmpty(CONFIG.topics[topic_name])){
            console.log("Topic \""+topic_name+"\" was not found in the config");
            throw "Topic \""+topic_name+"\" was not found in the config";
            return 0;
        }
        return (CONFIG.topics[topic_root]+CONFIG.topics[topic_name]);
    }
    var client; // MQTT Client
    /* Init the DNS */
    var init = function () {
        welcome();      // Say hello!
        connect_MQTT(); // Connect to the broker
        GUI.init();     // initialize the GUI
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
        client = new Paho.MQTT.Client(broker_mqtt.url,
                                      broker_mqtt.port,
                                      broker_mqtt.client_name);
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
        /* Todo: add a reconnect system/proper disconnect */
    };
    /* Welcome message */
    var welcome = function () {
        console.log(CONFIG.name_short+" v"+
                    CONFIG.version.major+"."+
                    CONFIG.version.minor+"."+
                    CONFIG.version.revision);
    };
    /* Call when message is recieved */
    var message_recieve = function (message) {
        switch (message.destinationName) {
            case topic("online"):
                CLIENT.online(message.payloadString);
                GUI.draw_speakers_from_data();
                //console.log("Client online: "+message.payloadString);
                break;
            case topic("offline"):
                CLIENT.offline(message.payloadString);
                GUI.draw_speakers_from_data();
                //console.log("Client offline: "+message.payloadString);
                break;
            case topic("music_volume"):
                $("#volume_slider").val(message.payloadString);//set slider value
                //console.log("Volume: "+message.payloadString);
                break;
            case topic("clients_data")+'/'+broker_mqtt.client_name: // Filter own data message
                break;
            case topic("music_sources"):
                var info_music_sources = JSON.parse(message.payloadString);
                console.log("Music Sources: ");console.log(info_music_sources);
                OBJECT.set_all(info_music_sources);
                GUI.draw_available_objects();
                break;
            default:
                if(message.destinationName.indexOf(topic("request"))>-1){ // Request topic
                    console.log("Request: "+message.destinationName+" | "+message.payloadString);
                    return;
                } else if(message.destinationName.indexOf(topic("answer_site"))>-1){ // Got answer
                    var device_date=message.payloadString;
                    if (!device_date) {
                        console.log("Empty device string!");
                        return;
                    }
                    try {
                        device_date=JSON.parse(device_date); // Produces a SyntaxError
                    } catch (error) {
                        console.log("Answer to object error:");
                        console.log(error.message);
                        return;
                    }
                    if (isEmpty(device_date)) {
                        console.log("Empty device data!");
                        return;
                    }
                    for (var client_name in device_date) {
                        if (!isEmpty(device_date[client_name])) { // Check if object is empty
                            CLIENT.set_objects(client_name, device_date[client_name]); // Set data of object
                            GUI.draw_speakers_from_data(); // Redraw
                            //console.log("Got personnal data from: "+client_name);
                            //console.log(device_date[client_name]);
                        }
                    }
                    return;
                } else if (message.destinationName.indexOf(topic("clients_data")+'/')>-1) { // Got new clients data
                    var info_clients = JSON.parse(message.payloadString);
                    CLIENT.set_all(info_clients); // Set date
                    GUI.draw_speakers_from_data(); // Redraw
                    //console.log('New clients data');
                    return;
                }
                console.log("Got unfiltred message from: "+message.destinationName+" | "+message.payloadString);
        }
    };
    /* Send message over MQTT */
    var send = function (topic, payload, retain = false) {
        message = new Paho.MQTT.Message(String(payload));
        message.destinationName = String(topic);
        message.retained = !!retain;
        client.send(message);
    };
    /* Send all clients data */
    var send_clients_data = function (data) {
        send(DNS.topic("clients_data")+'/'+broker_mqtt.client_name, data);
    };
    /* Subscribe to a topic */
    var subscribe = function (topic) {
        client.subscribe(topic, {qos: 1});//qos Quality of Service
    };
    /* Unsubscribe from a topic */
    var unsubscribe = function (topic) {
        client.unsubscribe(topic, {});
    };
    /* Get the client ID */
    var clientid = function () {
        return broker.client_name;
    };
    /* Initial subscribed topics */
    var subscribe_list = function () {
        subscribe(topic("online"));
        subscribe(topic("offline"));
        subscribe(topic("music_volume"));
        subscribe(topic("music_sources"));
        subscribe(topic("clients_data")+'/+');

        subscribe(topic("answer_site")+'/#');
        //subscribe(topic.root+'/#'); // Debug for message check
    };

    return { // Bind functions to the outside world
        init                : init,
        deinit              : deinit,
        topic               : topic,
        subscribe           : subscribe,
        send                : send,
        send_clients_data   : send_clients_data,
        clientid            : clientid
    };
})(window);
