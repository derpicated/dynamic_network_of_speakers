/*
 * Global Device storage
 *
 * This holds device information
 * In this use case, a device is a client.
 */

/* Debug text */
//console.log = function() {};

CLIENT = (function (global) {
    var CLIENTS = {};
    /* client online */
    var online = function (clientid) {
        if(CLIENTS[clientid] == null){
            console.log("Client "+clientid+" came online");
            CLIENTS[clientid]={};
        }
    };
    /* client offline */
    var offline = function (clientid) {
        if(CLIENTS[clientid] == null){return -1;}
        console.log("Client "+clientid+" went offline");
        delete CLIENTS[clientid];
    };
    var offline_all = function () {
        CLIENTS = {};
    };
    /* Get online clients */
    var get_online = function () {
        return CLIENTS;
    };
    /* Set object for client */
    var set_object = function (clientid, objectid, distance=0, angle=0) {
        online(clientid);
        var tmp_obj = {
            [objectid]: {
                'distance': distance,
                'angle': angle
            }
        };
        $.extend( true, CLIENTS[clientid], tmp_obj );
    };
    var set_all = function (clients) {
        CLIENTS = clients;
    };
    /* Delete single object from client */
    var del_object = function (clientid, objectid) {
        if (get_objects(clientid)[objectid] == null){return -1;}
        delete CLIENTS[clientid][objectid];
    };
    /* Delete all objects from client */
    var del_objects = function (clientid){
        if (get_objects(clientid) == null){return -1;}
        for (var obj in get_objects(clientid)){
            del_object(clientid, obj);
        }
    };
    /* Get all objects from client */
    var get_objects = function (clientid) {
        if (CLIENTS[clientid] == null){return -1;}
        return CLIENTS[clientid];
    };
    /* Get single object from client */
    var get_object = function (clientid, objectid) {
        if (get_objects(clientid)[objectid] == null){return -1;}
        return (get_objects(clientid)[objectid]);
    }
    /* Debug print */
    var debug_print = function () {
        console.log("CLIENT DEBUG PRINT");
        console.log(CLIENTS);
    };

    /* Outside interface */
    return {
        online: online,
        offline: offline,
        offline_all: offline_all,
        get_online: get_online,
        set_object: set_object,
        set_all: set_all,
        del_object: del_object,
        del_objects: del_objects,
        get_objects: get_objects,
        debug: debug_print
    };
})(window);
