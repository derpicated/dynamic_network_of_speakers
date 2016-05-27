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
        return CONFIG.website_prefix+parseInt(rand(1, 999));
    };
    broker_mqtt.client_name=generate_name(); // Set site name
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
    var connected = false; // Connection status

    //new Paho.MQTT.Message(String(payload));
    //Paho.MQTT.message
    //message.destinationName = String(topic);
    //message.retained = !!retain;

    /* Lastwill for first object of site */
    var lastwill = new Paho.MQTT.Message("");
    lastwill.destinationName= topic("clients_data_first_object")+'/'+broker_mqtt.client_name;
    lastwill.retained = true;
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
        },
        willMessage: lastwill
    };
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
                CLIENT.online(message.payloadString.replace(/[~`!#$%\^&*+=\-\[\]\\';,/{}|\\":<>\? ]/g,'')); // remove special chars spaces
                GUI.draw_speakers_from_data();
                //console.log("Client online: "+message.payloadString);
                break;
            case topic("offline"):
                CLIENT.offline(message.payloadString.replace(/[~`!#$%\^&*+=\-\[\]\\';,/{}|\\":<>\? ]/g,'')); // remove special chars spaces
                GUI.draw_speakers_from_data();
                //console.log("Client offline: "+message.payloadString);
                break;
            case topic("music_volume"):
                GUI.set_volume_slider(message.payloadString);
                //console.log("Volume: "+message.payloadString);
                break;
            case topic("clients_data")+'/'+broker_mqtt.client_name: // Filter own data message
                break;
            case topic("clients_data_first_object")+'/'+broker_mqtt.client_name: // Filter own data message
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
                } else if(message.destinationName.indexOf(topic("answer_site"))>-1){ // Got device data from client
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
                        }
                    }
                    return;
                } else if (message.destinationName.indexOf(topic("clients_data")+'/')>-1) { // Got new clients data
                    try {
                        var info_clients = JSON.parse(message.payloadString); // syntax error
                        CLIENT.set_all(info_clients); // Set date
                    } catch (error) {
                        console.log("Clients data wrong message error:");
                        console.log(error.message);
                    }
                    //console.log(info_clients);
                    //GUI.draw_speakers_from_data(); // No need ro redraw here, wait for the message with first obj location
                    return;
                } else if (message.destinationName.indexOf(topic("clients_data_first_object")+'/')>-1) { // Got first object pos
                    try {
                        if(isEmpty(message.payloadString)){
                            GUI.draw_speakers_from_data();// empty, redraw because no speakers drawn
                            return;
                        } else {
                            var object = JSON.parse(message.payloadString);
                            GUI.first_object(object.object_offset_left, object.object_offset_top); // Set the first object
                            GUI.draw_speakers_from_data(object.object_offset_left, object.object_offset_top); // Redraw
                        }
                    } catch (error) {
                        console.log("First object wrong message error:");
                        console.log(error.message);
                    }
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
    /* Send first position of first object */
    var send_first_position = function (left, top) {
        if(left && top){
            first_object = {
                object_offset_left: 0,
                object_offset_top: 0
            };
            first_object.object_offset_left = left;
            first_object.object_offset_top = top;
            send(DNS.topic("clients_data_first_object")+'/'+broker_mqtt.client_name, JSON.stringify(first_object), true);
        } else {
            console.log("send empty");
            send(DNS.topic("clients_data_first_object")+'/'+broker_mqtt.client_name, "", true); // send empty data
        }
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
        subscribe(topic("clients_data_first_object")+'/+');

        subscribe(topic("answer_site")+'/#');
        //subscribe(topic("root")+'#'); // Debug for message check
    };

    return { // Bind functions to the outside world
        init                : init,
        deinit              : deinit,
        topic               : topic,
        subscribe           : subscribe,
        send                : send,
        send_clients_data   : send_clients_data,
        send_first_position : send_first_position,
        clientid            : clientid
    };
})(window);

/* Check if object is empty */
function isEmpty(obj) {
    // null and undefined are "empty"
    if (obj == null) return true;
    // Assume if it has a length property with a non-zero value
    // that that property is correct.
    if (obj.length > 0)    return false;
    if (obj.length === 0)  return true;
    // Otherwise, does it have any properties of its own?
    // Note that this doesn't handle
    // toString and valueOf enumeration bugs in IE < 9
    for (var key in obj) {
        if (hasOwnProperty.call(obj, key)) return false;
    }
    return true;
}
