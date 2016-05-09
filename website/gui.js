/* Interface to access the site in a abstractive manner */

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

include('./devices.js');

GUI = (function (global) {
    var SPEAKER_LIST = $("#speaker_list");
    var DRAW_AREA = $("#draw_area");
    var OBJECT_LIST = $("#object_list");
    var UPDATE_FREQ_MOVE = 20; // Send data when an obj/speaker has moved this amount of units
    /* Scale factor for In Real Life to Virtual
     * Default: 1:1
     */
    var scale_irl=1;
    var scale_virt=1;
    var first_object_location = {
        left: DRAW_AREA.width()/2,
        top: DRAW_AREA.height()/2
    }
    var init = function () {
        bind_arrow_keys();
        draw_speakers_from_data();
    };
    /* Move object with arrows */
    var bind_arrow_keys = function () {
        var move_amount = 100;//units
        $(document).keydown(function(e) {
            // if selected do switch
            var obj_name = $('#music_name').val();
            if (obj_name &&  ($('#'+obj_name).length)) { // Check if object exists
                switch (e.which) {
                case 37:
                    $('#'+obj_name).stop().animate({
                        left: "-="+move_amount
                    }).trigger('drop'); //left arrow key
                    event.returnValue = false;
                    break;
                case 38:
                    $('#'+obj_name).stop().animate({
                        top: "-="+move_amount
                    }); //up arrow key
                    event.returnValue = false;
                    break;
                case 39:
                    $('#'+obj_name).stop().animate({
                        left: "+="+move_amount
                    }); //right arrow key
                    event.returnValue = false;
                    break;
                case 40:
                    $('#'+obj_name).stop().animate({
                        top: "+="+move_amount
                    }); //bottom arrow key
                    event.returnValue = false;
                    break;
                default:
                    event.returnValue = true;
                }
            } else {
                event.returnValue = true;
            }
        });
        return false;
    }
    /* Update the local speaker list and send this */
    var update_clients_data = function () {
        CLIENT.set_all(make_data_from_drawing());
        console.log('Updating speaker list!');
        DNS.send_clients_data(JSON.stringify(CLIENT.get_online()));
        // Send first object location
        var data = CLIENT.get_online();
        var object_offset_left;
        var object_offset_top;
        if (!isEmpty(data)) {
            for (first_speaker in data) break; // get first speaker in obj
            if (!isEmpty(data[first_speaker])) {
                for (first_object in data[first_speaker]) break; // First object
                object_offset_left = ($('#'+first_object).offset().left-DRAW_AREA.offset().left);
                object_offset_top = ($('#'+first_object).offset().top-DRAW_AREA.offset().top);
                DNS.send_first_position(object_offset_left, object_offset_top);
            }
        }
    };
    /* Clear the drawn speaker/objects
     * param: 'not_speakers' or 'not_objects'
     */
    var clear = function (not_x = '') {
        console.log(not_x);
        empty_speaker_list(not_x);
        empty_objects_list(not_x);
        empty_draw_area(not_x);
    };
    /* Draw speakers from CLIENT data
     * first_object_left is first object location from left in percentage
     * first_object_top is first object location from top in percentage
     */
    var draw_speakers_from_data = function (first_object_left = first_object_location.left, first_object_top = first_object_location.top) {
        empty_draw_area();
        var nr_of_objects=0;
        var object_tmp = $("<div class='object' id='object_tmp'></div>").hide().appendTo("body"); //add temp object
        var object_width = parseInt($('.object').css('width').replace('px',''));
        var object_height= parseInt($('.object').css('height').replace('px',''));
        object_tmp.remove(); //remove temp object
        //Delete unused speakers
        $('.speaker').each(function(key, value){
            if (!(CLIENT.get_online()[$(this).attr('id')])) {
                //console.log("Deleting unused drawn: "+$(this).attr('id'));
                $(this).remove();
            }
        });
        //Delete unused objects
        $('.object').each(function(key, value){
            if (!(OBJECT.get_objects()[$(this).attr('id')])) {
                //console.log("Deleting unused drawn: "+$(this).attr('id'));
                $(this).remove();
            }
        });
        //Loop all speakers
        $.each(CLIENT.get_online(), function(speaker_name, value){
            // Has the speaker object(s)?
            if (!isEmpty(value)) { // Speaker has object(s)
                nr_of_objects=0;
                $.each(CLIENT.get_online()[speaker_name], function(object_name, obj_value){
                    if (!nr_of_objects) { // First object
                        //draw_object(object_name, DRAW_AREA, ((DRAW_AREA.width()-object_width)*(first_object_left/100)), ((DRAW_AREA.height()-object_height)*(first_object_top/100)));
                        draw_object(object_name, DRAW_AREA, (first_object_left), (first_object_top));
                        // Draw speaker seen from object
                        var left =$('#'+object_name).offset().left-DRAW_AREA.offset().left-rectangular(obj_value.distance, obj_value.angle).x;
                        var top  =$('#'+object_name).offset().top-DRAW_AREA.offset().top+rectangular(obj_value.distance, obj_value.angle).y;
                        draw_speaker(speaker_name, DRAW_AREA, left, top); // Draw speaker
                    } else { // Not first object of speaker, so the speaker is already drawn
                        // Draw the object seen from th speaker
                        var left =$('#'+speaker_name).offset().left-DRAW_AREA.offset().left+rectangular(obj_value.distance, obj_value.angle).x;
                        var top  =$('#'+speaker_name).offset().top-DRAW_AREA.offset().top-rectangular(obj_value.distance, obj_value.angle).y;
                        draw_object(object_name, DRAW_AREA, left, top); //draw object
                    }
                    nr_of_objects++;
                }, speaker_name);
            } else { // Speaker has no object(s)
                if (isEmpty($('#'+speaker_name))) { // Speaker not drawn
                    draw_speaker(speaker_name, SPEAKER_LIST, calc_next_speaker_list_pos().left, calc_next_speaker_list_pos().top);
                } // If already drawn, don't do anything
            }
        });
        draw_available_objects(); // Draw all the objects that are available but not used
    };
    /* Make CLIENT data from drawing */
    var make_data_from_drawing = function () {
        var speakers = {};
        var left, top;
        var object, speaker;
        var speakers_keys=[];
        var objects_keys=[];
        //make speaker array and object array
        $('#draw_area .speaker').each(function(i, speaker_obj){
            speaker = $(speaker_obj);
            speakers_keys.push(speaker.attr('id'));
        });
        $('#draw_area .object').each(function(i, object_obj){
            object = $(object_obj);
            objects_keys.push(object.attr('id'));
        });
        // Sort array keys alphabetically
        speakers_keys.sort();
        objects_keys.sort();
        speakers_keys.forEach(function (speaker_name, index) { // Loop all speakers
            speaker = $('#'+speaker_name);
            speakers[speaker.attr('id')] = {}; // Set empty object
            objects_keys.forEach(function (object_name, index) { // Loop all objects
                object = $('#'+object_name);
                speakers[speaker.attr('id')][object.attr('id')] = {
                    distance: -1,
                    angle: -1
                };// Placeholder value
                left = object.offset().left-speaker.offset().left;
                top = speaker.offset().top-object.offset().top;
                speakers[speaker.attr('id')][object.attr('id')].distance=polar(left, top).distance;
                speakers[speaker.attr('id')][object.attr('id')].angle=polar(left, top).angle;
            });
        });
        $('#speaker_list .speaker').each(function(index, speaker_obj){ // Loop online idle speakers
            speaker = $(speaker_obj);
            speakers[speaker.attr('id')] = {}; // Set empty object
        });
        return speakers;
    };
    /* Draw single speaker */
    var draw_speaker = function (speaker_name, destination, off_left=0, off_top=0) {
        var speaker = $('#'+speaker_name);
        if (speaker.length) { // Check if speaker exists
            speaker.detach().appendTo(destination); // Add to x
        } else { // Draw new speaker
            var speaker_class = "<div class='speaker noselect' move_counter='0' id='"+speaker_name+"'>"+speaker_name.replace(CONFIG.speaker_prefix, '')+"</div>";
            destination.append(speaker_class);
        }
        speaker = $('#'+speaker_name); // Update reference to object
        speaker.draggable({ // Make speaker dragable in the list and draw area (top div)
            containment: "#top",
            revert: 'invalid',
            stop: function (event, ui) {
                update_clients_data();
            },
            stack: ".speaker, .object",
            scroll: false,
            snap: "#speaker_list",
            snapMode: "outer",
            drag: function (event, ui) {
                speaker.attr('move_counter', parseInt(speaker.attr('move_counter'))+1);
                if(speaker.attr('move_counter')>UPDATE_FREQ_MOVE){
                    speaker.attr('move_counter', 0);
                    update_clients_data();
                }
            }
        });
        // Make speaker dropable in varius places
        DRAW_AREA.droppable({
            drop: function( event, ui ) {
                var offset = ui.offset;
                var speaker_object= $(ui.helper[0]);
                speaker_object.detach().appendTo(DRAW_AREA);
                speaker_object.offset({ top: offset.top, left: offset.left});
            }
        });
        SPEAKER_LIST.droppable({
            accept:'.speaker',
            drop: function( event, ui ) {
                var offset = ui.offset;
                var speaker_object= $(ui.helper[0]);
                speaker_object.detach().appendTo(SPEAKER_LIST);
                speaker_object.offset({ top: offset.top, left: offset.left});
            }
        });
        speaker.offset({ top: destination.offset().top+off_top, left: destination.offset().left+off_left}); // Set speaker offset
    };
    /* Delete single speaker */
    var del_speaker = function (speaker_name) {
        $('#'+speaker_name).remove();
    };
    var draw_object = function (object_name, destination, off_left=0, off_top=0) {
        var object = $('#'+object_name);
        if (object.length) { // Check if object exists
            object.detach().appendTo(destination); // Add to destination area
        } else { // Draw new object
            var object_class = "<div class='object noselect' move_counter='0' id='"+object_name+"' onclick='GUI.load_object_properties(\""+object_name+"\")'>"+object_name+"</div>";
            destination.append(object_class);
        }
        object = $('#'+object_name);
        object.draggable({ // Make dragable in the draw area
            containment: '#top',
            revert: 'invalid',
            stop: function (event, ui) {
                update_clients_data();
            },
            stack: ".object, .speaker",
            scroll: false,
            snap: "#object_list",
            snapMode: "outer",
            drag: function (event, ui) {
                object.attr('move_counter', parseInt(object.attr('move_counter'))+1);
                if(object.attr('move_counter')>UPDATE_FREQ_MOVE){
                    object.attr('move_counter', 0);
                    update_clients_data();
                }
            }
        });
        // Make speaker dropable in varius places
        DRAW_AREA.droppable({
            drop: function( event, ui ) {
                var offset = ui.offset;
                var object_object= $(ui.helper[0]);
                object_object.detach().appendTo(DRAW_AREA);
                object_object.offset({ top: offset.top, left: offset.left});
            }
        });
        OBJECT_LIST.droppable({
            accept:'.object',
            drop: function( event, ui ) {
                var offset = ui.offset;
                var object_object= $(ui.helper[0]);
                object_object.detach().appendTo(OBJECT_LIST);
                object_object.offset({ top: offset.top, left: offset.left});
            }
        });
        object.offset({ top: destination.offset().top+off_top, left: destination.offset().left+off_left}); // Set offset
    };
    /* Draw all available objects in object list */
    var draw_available_objects = function () {
        var objects=OBJECT.get_objects();
        for (var object_name in objects) {
            if (objects.hasOwnProperty(object_name)) {
                var next_pos = calc_next_object_list_pos();
                if (isEmpty($('#'+object_name))) { // Object not drawn.
                    draw_object(object_name, OBJECT_LIST, next_pos.left, next_pos.top);
                }
            }
        }
    };
    /* Delete single object */
    var del_object = function (object_name) {
        //console.log('deleting obj: '+object_name);
        $('#'+object_name).remove();
        OBJECT.del(object_name);
    };
    /* Load object properties into the gui */
    var load_object_properties = function (object_name) {
        $('#music_name').val(object_name);
        $('#music_uri').val(OBJECT.get_object(object_name));
        // Get default values
        var object_tmp = $("<div class='object' id='object_tmp'></div>").hide().appendTo("body");
        var object_border = $('#object_tmp').css('border');
        object_tmp.remove(); //remove temp object
        $('.object').removeClass('active'); // Remove all avtive classes
        $('#'+object_name).addClass('active');
    };
    /* Save the object properties to an object */
    var save_object_properties = function () {
        var obj_name = $('#music_name').val();
        var obj_uri = $('#music_uri').val();
        if (!obj_name) {
            $('#music_name').val("Idiot, fill all the values");
            $('#music_uri').val(obj_uri);
            return;
        } else if (!obj_uri) {
            $('#music_name').val(obj_name);
            $('#music_uri').val("Idiot, fill all the values");
            return;
        }
        if (!isValid(obj_name)) {
            $('#music_name').val("Only valid characters!");
            $('#music_uri').val(obj_uri);
            return;
        }
        OBJECT.set(obj_name, obj_uri);
        var next_pos = calc_next_object_draw_area_pos();
        draw_object(obj_name, DRAW_AREA, next_pos.left, next_pos.top);
        update_clients_data();
        load_object_properties(obj_name);
    };
    /* Delete a object via the GUI delete button */
    var delete_object_button = function (object_name) {
        del_object(object_name);
        update_clients_data();
    };
    /* Clear speakers respectively */
    var empty_speaker_list = function (not_x = '') {
        if (!(not_x=='not_speakers')) {
            $('#speaker_list .speaker').remove();
            //console.log('clear speaker list: speaker');
        }
    };
    /* Clear objects respectively */
    var empty_objects_list = function (not_x = '') {
        if (!(not_x=='not_objects')) {
            $('#object_list .object').remove();
            //console.log('clear speaker list: speaker');
        }
    };
    /* Clear objects and speakers respectively */
    var empty_draw_area = function (not_x = '') {
        if (!(not_x=='not_objects')) {
            $('#draw_area .object').remove();
            //console.log('clear draw area: obj');
        }
        if (!(not_x=='not_speakers')) {
            $('#draw_area .speaker').remove();
            //console.log('clear draw area: speaker');
        }
    };
    /* Calculate the x and y value for next speaker */
    var calc_next_speaker_list_pos = function () {
        var pos={
            top:30,
            left:5
        };
        var number_of_speakers=0;
        var width=SPEAKER_LIST.width();
        var height=SPEAKER_LIST.height();
        var speaker_temp = $("<div  class='speaker' id='speaker_temp'></div>").hide().appendTo("body"); //add temp speakers
        var speaker_boundry = 20;//% (margin atound speakers)
        var speaker_width=parseInt($('.speaker').css('width').replace('px',''));
        speaker_width+=speaker_width*(speaker_boundry/100);
        var speaker_height=parseInt($('.speaker').css('height').replace('px',''));
        speaker_height+=speaker_height*(speaker_boundry/100);
        speaker_temp.remove(); //remove the temp speaker
        var left, top;
        $(".speaker", '#speaker_list').each(function (i, key) {
            number_of_speakers++;
            left = $(key).css('left').replace('px', '')-SPEAKER_LIST.position().left;
            top = $(key).css('top').replace('px', '')-SPEAKER_LIST.position().top;
            //console.log("left: "+left+" top: "+top);
            if((left>pos.left) || (top>pos.top)){
                pos.left=left;
                pos.top=top;
            }
        });
        if(number_of_speakers>0){pos.left+=speaker_width};
        // Now got biggest values for a speaker
        // if left or left+speak width is bigger as width
        // pos left is 0
        if((pos.left>width)||(pos.left+speaker_width>width)){
            pos.left=0;
            pos.top+=speaker_height;
        }
        //if top or top+speak height is bigger as height
        //top is 0 and left is 0
        if((pos.top>height)||(pos.top+speaker_height>height)){
            pos.left=0;
            pos.top=0;
        }
        return pos;
    };
    /* Calculate the x and y value for next object in the object list*/
    var calc_next_object_list_pos = function () {
        var pos={
            top:30,
            left:5
        };
        var number_of_objects=0;
        var width=OBJECT_LIST.width();
        var height=OBJECT_LIST.height();
        var object_temp = $("<div  class='object' id='object_temp'></div>").hide().appendTo("body"); //add temp speakers
        var object_boundry = 20;//% (margin atound speakers)
        var object_width=parseInt($('.object').css('width').replace('px',''));
        object_width+=object_width*(object_boundry/100);
        var object_height=parseInt($('.object').css('height').replace('px',''));
        object_height+=object_height*(object_boundry/100);
        object_temp.remove(); //remove the temp speaker
        var left, top;
        $(".object", '#object_list').each(function (i, key) {
            number_of_objects++;
            left = $(key).css('left').replace('px', '')-OBJECT_LIST.position().left;
            top = $(key).css('top').replace('px', '')-OBJECT_LIST.position().top;
            //console.log("left: "+left+" top: "+top);
            if((left>pos.left) || (top>pos.top)){
                pos.left=left;
                pos.top=top;
            }
        });
        if(number_of_objects>0){pos.left+=object_width};
        // Now got biggest values for a speaker
        // if left or left+speak width is bigger as width
        // pos left is 0
        if((pos.left>width)||(pos.left+object_width>width)){
            pos.left=0;
            pos.top+=object_height;
        }
        //if top or top+speak height is bigger as height
        //top is 0 and left is 0
        if((pos.top>height)||(pos.top+object_height>height)){
            pos.left=0;
            pos.top=0;
        }
        return pos;
    };
    /* Calculate the x and y value for next object */
    var calc_next_object_draw_area_pos = function () {
        var pos={
            top:0,
            left:0
        };
        var number_of_objects=0;
        var width=DRAW_AREA.width();
        var height=DRAW_AREA.height();
        var object_tmp = $("<div  class='object' id='object_tmp'></div>").hide().appendTo("body"); //add temp object
        var object_boundry = 20;//% (margin atound speakers)
        var object_width=parseInt($('.object').css('width').replace('px',''));
        object_width+=object_width*(object_boundry/100);
        var object_height=parseInt($('.object').css('height').replace('px',''));
        object_height+=object_height*(object_boundry/100);
        object_tmp.remove(); //remove the temp object
        pos.left=(width-object_width)/2;
        pos.top=(height-object_height)/2;
        var left, top;
        $(".object", '#draw_area').each(function (index, key) {
            number_of_objects++;
            left = $(key).css('left').replace('px', '')-DRAW_AREA.position().left;
            top = $(key).css('top').replace('px', '')-DRAW_AREA.position().top;
            //console.log("left: "+left+" top: "+top);
            if((left>pos.left) || (top>pos.top)){
                pos.left=left;
                pos.top=top;
            }
        });
        if(number_of_objects>0){pos.left+=object_width};
        // Now got biggest values for a object
        // if left or left+obj width is bigger as width
        // pos left is 0
        if((pos.left>width)||(pos.left+object_width>width)){
            pos.left=0;
            pos.top+=object_height;
        }
        //if top or top+obj height is bigger as height
        //top is 0 and left is 0
        if((pos.top>height)||(pos.top+object_height>height)){
            pos.left=0;
            pos.top=0;
        }
        return pos;
    };

    var isValid = function (str){
        return !/[~`!#$%\^&*+=\-\[\]\\';,/{}|\\":<>\? ]/g.test(str);
    }
    /************************************************************/
    /*  MATHS                                                   */
    /************************************************************/
    /* Convert virt value to IRL value with scale */
    var scale_to_irl = function (value) {
        return ((value*scale_irl)/scale_virt);
    };
    /* Convert IRL value to VIRT value with scale */
    var scale_to_virt = function (value) {
        return ((value*scale_virt)/scale_irl);
    };
    /* Convert x and y coordinates to a polar notation */
    var polar = function(x, y) {
        var polar = {
            distance:0,
            angle:0
        };
        polar.distance = Math.sqrt(Math.pow(x, 2)+Math.pow(y, 2)); // sqrt(x^2 + y^2)
        polar.angle = Math.degrees(Math.atan2(y, x)); // tan^-1(Y/X)
        return polar;
    };
    /* Convert distance and angle to a rectangular notation */
    var rectangular = function (distance, angle) {
        var rectangular = {
            x:0,
            y:0
        };
        angle=Math.radians(angle);
        rectangular.x = (distance*Math.cos(angle)); // distance cos angle
        rectangular.y = (distance*Math.sin(angle)); // distance sin angle
        return rectangular;
    };
    // Converts from degrees to radians.
    Math.radians = function(degrees) {
        return degrees * Math.PI / 180;
    };
    // Converts from radians to degrees.
    Math.degrees = function(radians) {
        return radians * 180 / Math.PI;
    };

    return { // Bind functions to the outside world
        init                    : init,
        clear                   : clear,
        empty_draw_area         : empty_draw_area,
        update_clients_data     : update_clients_data,
        draw_speakers_from_data : draw_speakers_from_data,
        draw_available_objects  : draw_available_objects,
        make_data_from_drawing  : make_data_from_drawing,
        save_object_properties  : save_object_properties,
        load_object_properties  : load_object_properties,
        delete_object_button    : delete_object_button,
        DRAW_AREA               : DRAW_AREA,
        SPEAKER_LIST            : SPEAKER_LIST,
        OBJECT_LIST             : OBJECT_LIST,
        first_object_location   : first_object_location
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
