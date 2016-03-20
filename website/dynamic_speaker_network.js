var DNS_VERSION = {
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

include('./devices.js');
include('./objects.js');

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
            subscribe_list();// Connection succeeded; subscribe to our topics
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
        /* Stream topics */
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
        switch (message.destinationName) {
            case topic.info_client_online:
                console.log("Client came online: "+message.payloadString);
                CLIENT.online(message.payloadString);
                break;
            case topic.info_client_offline:
                console.log("Client went offline: "+message.payloadString);
                CLIENT.offline(message.payloadString);
                break;
            default:
                if(message.destinationName.indexOf(topic.answer)>-1){//answer topic
                    //one for the distance answer
                    //one for the angle answer
                    var tmp = message.destinationName;
                    console.log("Answer!! "+tmp);
                    tmp=tmp.replace(topic.answer+'/distance/', '');
                    console.log("Answer!! "+tmp);

                }
                console.log("Got unfiltred message from: "+message.destinationName+" | "+message.payloadString);
        }
    };
    var send = function (topic, payload) {
        if(!(typeof topic=='string')){
            console.log('Wrong topic!');
            return -1;
        }
        message = new Messaging.Message(payload);
        message.destinationName = topic;
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
        subscribe(topic.info_client_online);
        subscribe(topic.info_client_offline);

        subscribe(topic.answer+'/#');
        subscribe(topic.root+'/#');
    };
    /* Random gen */
    function rand(min,max) {
        return Math.floor(Math.random()*(max-min+1)+min);
    };
    var generate_name = function () {
        return "site_"+parseInt(rand(1, 999));
    };
    return {
        init: init,
        deinit: deinit,
        topic: topic,
        send: send
    };
})(window);
