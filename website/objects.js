/*
 * Global Object storage
 *
 * This Holds all the Objects that are placed on the website
 */

/* Debug text */
//console.log = function() {};

OBJECT = (function (global) {
    var OBJECTS = {};
    /* Add object */
    var add = function (objectid) {
        if(OBJECTS[objectid] == null){
            OBJECTS[objectid]={};
            console.log("Object "+objectid+" added");
        }
    };
    /* Delete object */
    var del = function (objectid) {
        if(OBJECTS[objectid] == null){return -1;}
        delete OBJECTS[objectid];
        console.log("Object "+objectid+" deleted");
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
    /* Set object information */
    var set_location = function (objectid, x=0, y=0) {
        add(objectid);
        OBJECTS[objectid]['location']={
            'x': x,
            'y': y
        };
    };

    /* Debug print */
    var debug_print = function () {
        console.log("OBJECT DEBUG PRINT");
        console.log(OBJECTS);
    };

    /* Outside interface */
    return {
        add: add,
        del: del,
        get_objects: get_objects,
        get_object: get_object,
        set_location: set_location,
        debug: debug_print
    };
})(window);
