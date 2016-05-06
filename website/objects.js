/*
 * Global Object storage
 *
 * This Holds all the Objects that are placed on the website
 * The format is
 * ObjName : URI
 * The URI is an URI to an music file of some sort.
 */

/* Debug text */
//console.log = function() {};

OBJECT = (function (global) {
    var OBJECTS = {};
    /* Add/set object */
    var set = function (objectid, uri) {
        if (isValid(objectid)) { // Object not drawn.
            OBJECTS[objectid]=uri;
        }
        //console.log("Object "+objectid+" : "+uri+" set");
        send_objects(); // Send new data
    };
    var set_all = function (objects) {
        for (var object_name in objects) {
            if(!object_name || !isValid(object_name)){ // Object not drawn.
                console.log("NOT GOOD");
                delete objects[object_name]
            }
        }
        OBJECTS=objects;
    };
    /* Delete object */
    var del = function (objectid) {
        if(OBJECTS[objectid] == null){return -1;}
        delete OBJECTS[objectid];
        //console.log("Object "+objectid+" deleted");
        send_objects(); // Send new data
    };
    var del_all = function () {
        OBJECTS = {};
    };
    /* Get all objects */
    var get_objects = function () {
        return OBJECTS;
    };
    /* Get single object */
    var get_object = function (objectid) {
        if (get_objects()[objectid] == null){return -1;}
        return (get_objects()[objectid]);
    };

    /* Send the sources */
    var send_objects = function () {
        DNS.send(DNS.topic.info_music_sources, JSON.stringify(get_objects()), true);
    };

    /* Debug print */
    var debug_print = function () {
        console.log("OBJECT DEBUG PRINT");
        console.log(OBJECTS);
    };
    var isValid = function (str){
        return !/[~`!#$%\^&*+=\-\[\]\\';,/{}|\\":<>\? ]/g.test(str);
    }

    return { // Bind functions to the outside world
        set         : set,
        set_all     : set_all,
        del         : del,
        del_all     : del_all,
        get_objects : get_objects,
        get_object  : get_object,
        send_objects: send_objects,
        debug       : debug_print
    };
})(window);
