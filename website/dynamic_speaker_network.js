var DNS_VERSION = {
    major: 0,
    minor: 0,
    revison: 1
};

/* Debug text */
//console.log = function() {};

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
            DNS.send(DNS.topic.request_online, '1');    //get initial devices
            DNS.send(DNS.topic.request_info_clients, topic.answer);    //get initial devices parameters
            //update device info
        },
        onFailure: function (message) {
            connected = false;
            console.log("MQTT Connection failed: " + message.errorMessage);
        }
    };
    /* MQTT Topics (See: topic spec sheet)*/
    var topic = new function() {
        this._                          = 'ESEiot';
        this.root                       = this._+'/DNS';
        this.request                    = this.root+'/request';
        this.request_online             = this.request+'/online';
        this.request_info               = this.request+'/info';
        this.request_info_clients       = this.request_info+'/clients';
        this.info                       = this.root+'/info';
        this.info_music                 = this.info+'/music';
        this.info_music_time            = this.info_music+'/time';
        this.info_music_time_position   = this.info_music_time+'/position';
        this.info_music_status          = this.info_music+'/status';
        this.info_music_sources         = this.info_music+'/sources';
        this.info_music_volume          = this.info_music+'/volume';
        this.info_clients               = this.info+'/clients';
        this.info_clients_data          = this.info_clients+'/data';
        this.info_clients_data_site     = this.info_clients_data+'/'+broker_mqtt.client_name;
        this.info_clients_startdraw     = this.info_clients+'/startdraw';
        this.info_clients_startdraw_site= this.info_clients_startdraw+'/'+broker_mqtt.client_name;
        this.info_clients_online        = this.info_clients+'/online';
        this.info_clients_offline       = this.info_clients+'/offline';
        /* personnal request topics */
        this.answer                     = this.root+'/answer/site';
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
            case topic.info_clients_online:
                CLIENT.online(message.payloadString);
                GUI.draw_speakers_from_data();
                //console.log("Client online: "+message.payloadString);
                break;
            case topic.info_clients_offline:
                CLIENT.offline(message.payloadString);
                GUI.draw_speakers_from_data();
                //console.log("Client offline: "+message.payloadString);
                break;
            case topic.info_music_volume:
                $("#volume_slider").val(message.payloadString);//set slider value
                //console.log("Volume: "+message.payloadString);
                break;
            case topic.info_music_sources:
                var info_music_sources = JSON.parse(message.payloadString);
                console.log("Music Sources: ");console.log(info_music_sources);
                OBJECT.set_all(info_music_sources);
                GUI.draw_available_objects();
                break;
            default:
                if(message.destinationName.indexOf(topic.request)>-1){ // Request topic
                    console.log("Request: "+message.destinationName+" | "+message.payloadString);
                    return;
                } else if(message.destinationName.indexOf(topic.answer)>-1){ // Got answer
                    var device_date = JSON.parse(message.payloadString);
                    for (var client_name in device_date) {
                        if (!isEmpty(device_date[client_name])) { // Check if object is empty
                            //CLIENT.set_objects(client_name, device_date[client_name]); // Set data of object
                            //GUI.draw_speakers_from_data(); // Redraw
                            console.log("Got personnal data from: "+client_name);
                            console.log(device_date[client_name]);
                        }
                    }
                    return;
                } else if (message.destinationName.indexOf(topic.info_clients+'/')>-1) { // Got new clients data
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
    /* Subscribe to a topic */
    var subscribe = function (topic) {
        client.subscribe(topic, {qos: 1});//qos Quality of Service
    };
    /* Unsubscribe from a topic */
    var unsubscribe = function (topic) {
        client.unsubscribe(topic, {});
    };
    /* Initial subscribed topics */
    var subscribe_list = function () {
        subscribe(topic.info_clients_online);
        subscribe(topic.info_clients_offline);
        subscribe(topic.info_music_volume);
        subscribe(topic.info_music_sources);
        subscribe(topic.info_clients_data+'/+');

        subscribe(topic.answer+'/#');
        //subscribe(topic.root+'/#'); // Debug for message check
    };

    return { // Bind functions to the outside world
        init        : init,
        deinit      : deinit,
        topic       : topic,
        subscribe   : subscribe,
        send        : send
    };
})(window);
