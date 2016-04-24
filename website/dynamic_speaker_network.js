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
        port: CONFIG.broker[CONFIG.use_broker].port_ws
    };
    var connected = false;
    var connect_options = {
        timeout: 3,
        onSuccess: function () {
            console.log("MQTT Connected as: "+client._getClientId());
            console.log("MQTT Connected with: "+CONFIG.broker[CONFIG.use_broker].broker);
            connected = true;
            subscribe_list();// Connection succeeded; subscribe to our topics
            DNS.send(DNS.topic.request_online, '1');    //get initial devices
            //update device info
        },
        onFailure: function (message) {
            connected = false;
            console.log("MQTT Connection failed: " + message.errorMessage);
        }
    };
    /* Topic settings */
    var topic = new function() {
        this.ese                        = 'ESEiot';
        this.root                       = this.ese+'/DNS';
        this.request                    = this.root+'/request';
        this.request_online             = this.request+'/online';
        this.request_distance           = this.request+'/distance';
        this.request_distance_objectid  = this.request_distance+'/objectid';
        this.request_information        = this.request+'/information';
        this.request_information_client = this.request_information+'/client';
        this.request_update             = this.request+'/update';
        this.request_updated_rwf        = this.request_update+'/rwf';
        this.client                     = this.root+'/client';
        this.client_speaker             = this.client+'/speaker';
        this._objects                   = '/objects';
        this.info                       = this.root+'/info';
        this.info_music                 = this.info+'/music';
        this.info_music_time            = this.info_music+'/time';
        this.info_music_time_position   = this.info_music_time+'/position';
        this.info_music_status          = this.info_music+'/status';
        this.info_music_volume          = this.info_music+'/volume';
        this.info_music_source          = this.info_music+'/source';
        this.info_client                = this.info+'/client';
        this.info_client_online         = this.info_client+'/online';
        this.info_client_offline        = this.info_client+'/offline';
        /* personnal request topics */
        this.answer                     = this.root+'/answer/site';
    };
    var client; // MQTT Client
    /* Init the DNS */
    var init = function () {
        welcome();
        connect_MQTT();
        GUI.init();
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
        console.log(CONFIG.name_short+" v"+CONFIG.version.major+"."+CONFIG.version.minor+"."+CONFIG.version.revision);
    };
    /* Call when mess is recieved */
    var message_recieve = function (message) {
        switch (message.destinationName) {
            case topic.info_client_online:
                //console.log("Client send ID: "+message.payloadString);
                CLIENT.online(message.payloadString);
                GUI.draw_speakers_from_data();
                break;
            case topic.info_client_offline:
                //console.log("Client send ID: "+message.payloadString);
                CLIENT.offline(message.payloadString);
                GUI.draw_speakers_from_data();
                break;
            case topic.info_music_volume:
                console.log("Volume: "+message.payloadString);
                $("#volume_slider").val(message.payloadString);//set slider value
                break;
            case topic.info_music_source:
                //console.log("Music Source: "+message.payloadString);
                var info_music_source = JSON.parse(message.payloadString);
                for (first in info_music_source) break; // Temp fix, this will be changed later on
                console.log("Music Name: "+first);
                console.log("Music Source: "+info_music_source[first]);
                $('#music_name').val(first);
                $('#music_uri').val(info_music_source[first]);
                break;
            default:
                if(message.destinationName.indexOf(topic.request)>-1){//request topic
                    console.log("Request: "+message.destinationName);
                    return;
                } else if(message.destinationName.indexOf(topic.answer)>-1){//answer topic
                    //one for the distance answer
                    //one for the angle answer
                    //var tmp = message.destinationName;
                    //console.log("Answer!! "+tmp);
                    //tmp=tmp.replace(topic.answer+'/distance/', '');
                    //console.log("Answer!! "+tmp);
                    return;
                }
                console.log("Got unfiltred message from: "+message.destinationName+" | "+message.payloadString);
        }
    };
    var send = function (topic, payload, retain = false) {
        message = new Paho.MQTT.Message(String(payload));
        message.destinationName = String(topic);
        message.retained = !!retain;
        client.send(message);
    };

    var send_sound_source = function (uri, name='DNS_Music') {
        var sound_source =
        "{"+
          "\""+String(name)+"\":\""+String(uri)+"\""+
        "}";
      console.log("Send Sound Source: "+sound_source);
      DNS.send(topic.info_music_source, sound_source, true);
    };

    var subscribe = function (topic) {
        client.subscribe(topic, {qos: 1});//qos Quality of Service
    };
    var unsubscribe = function (topic) {
        client.unsubscribe(topic, {});
    };
    /* Initial subscribed topics */
    var subscribe_list = function () {
        subscribe(topic.info_client_online);
        subscribe(topic.info_client_offline);
        subscribe(topic.info_music_volume);
        subscribe(topic.info_music_source);

        subscribe(topic.answer+'/#');
        //subscribe(topic.root+'/#'); // Debug for message check
    };

    /* Random gen */
    function rand(min,max) {
        return Math.floor(Math.random()*(max-min+1)+min);
    };
    var generate_name = function () {
        return CONFIG.name_website+parseInt(rand(1, 999));
    };
    return {
        init: init,
        deinit: deinit,
        topic: topic,
        send: send,
        send_sound_source:send_sound_source
    };
})(window);
