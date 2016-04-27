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
    var speaker_list = $("#speaker_list");
    var draw_area = $("#draw_area");
    /* Scale factor for In Real Life to Virtual
     * Default: 1:1
     */
    var scale_irl=1;
    var scale_virt=1;
    var init = function () {
        draw_speakers_from_data();
        /* Update the data after every x px moved while clicked and dragged */
        var draw_update_move_every=100;//px
        var draw_update_move_default=draw_update_move_every;
        var list_update_move_every=draw_update_move_default*4;//px
        var list_update_move_default=list_update_move_every;
        draw_area.mousemove(function( event ) {
            if (event.buttons) {
                draw_update_move_every--;
                if (draw_update_move_every==0) {
                    update_speaker_list();
                    draw_update_move_every=draw_update_move_default;
                }
            }
        });
        speaker_list.mousemove(function( event ) {
            if (event.buttons) {
                list_update_move_every--;
                if (list_update_move_every==0) {
                    update_speaker_list();
                    list_update_move_every=list_update_move_default;
                }
            }
        });
    };
    /* Update the local speaker list and send this */
    var update_speaker_list = function () {
        CLIENT.set_all(make_data_from_drawing());
        console.log('Updating speaker list!');
        //console.log(CLIENT.get_online());
        DNS.send(DNS.topic.info_clients_site, JSON.stringify(CLIENT.get_online()));
    };
    /* Clear the drawn speaker/objects
     * param: 'not_speakers' or 'not_objects'
     */
    var clear = function (not_x = '') {
        console.log(not_x);
        empty_speaker_list(not_x);
        empty_draw_area(not_x);
    };
    /* Draw speakers from CLIENT data */
    var draw_speakers_from_data = function () {
        empty_draw_area(); // Clear the draw area
        var nr_of_objects=0;
        var object_tmp = $("<div class='object' id='object_tmp'></div>").hide().appendTo("body"); //add temp object
        var object_width = parseInt($('.object').css('width').replace('px',''));
        var object_height= parseInt($('.object').css('height').replace('px',''));
        object_tmp.remove(); //remove temp object
        //Delete unused speakers
        $('.speaker').each(function(key, value){
            if (!(CLIENT.get_online()[$(this).attr('id')])) {
                console.log("Deleting unused drawn: "+$(this).attr('id'));
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
                        if (isEmpty($('#'+object_name))) { // Object not drawn.
                            // Draw object in center of draw area
                            draw_object(object_name, ((draw_area.width()-object_width)/2), ((draw_area.height()-object_height)/2));
                        }
                        // Draw speaker from object
                        del_speaker(speaker_name); // Delete speaker
                        var left =$('#'+object_name).offset().left-draw_area.offset().left-rectangular(obj_value.distance, obj_value.angle).x;
                        var top  =$('#'+object_name).offset().top-draw_area.offset().top+rectangular(obj_value.distance, obj_value.angle).y;
                        draw_speaker(speaker_name, draw_area, left, top); // Draw speaker
                    } else { // Not first object of speaker
                        if (isEmpty($('#'+object_name))) { // Object not drawn, so draw it.
                            // Draw the object from speaker
                            del_object(object_name);//delete object
                            var left =$('#'+speaker_name).offset().left-draw_area.offset().left+rectangular(obj_value.distance, obj_value.angle).x;
                            var top  =$('#'+speaker_name).offset().top-draw_area.offset().top-rectangular(obj_value.distance, obj_value.angle).y;
                            draw_object(object_name, left, top); //draw object
                        }
                    }
                    nr_of_objects++;
                }, speaker_name);
            } else { // Speaker has no object(s)
                if (isEmpty($('#'+speaker_name))) { // Speaker not drawn
                    //draw in speaker list
                    draw_speaker(speaker_name, speaker_list, calc_next_speaker_list_pos().left, calc_next_speaker_list_pos().top);
                } // If already drawn, don't do anything
            }
        });
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
            speaker=$('#'+speaker_name);
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
        del_speaker(speaker_name); //Delete speaker, if it exists
        var speaker_class = "<div class='speaker noselect' id='"+speaker_name+"'>"+speaker_name.replace(CONFIG.name_speaker, '')+"</div>";
        destination.append(speaker_class);
        var speaker = $('#'+speaker_name);
        speaker.offset({ top: destination.offset().top+off_top, left: destination.offset().left+off_left}); // Set speaker offset
        speaker.draggable({ // Make speaker dragable in the list and draw area (top div)
            containment: "#top",
            scroll: false,
            snap: "#speaker_list",
            snapMode: "both"
        });
        // Make speaker dropable in varius places
        draw_area.droppable({
            drop: function( event, ui ) {
                var offset = ui.offset;
                var speaker_object= $(ui.helper[0]);
                speaker_object.detach().appendTo(draw_area);
                speaker_object.offset({ top: offset.top, left: offset.left});
                update_speaker_list();
            }
        });
        speaker_list.droppable({
            drop: function( event, ui ) {
                var offset = ui.offset;
                var speaker_object= $(ui.helper[0]);
                speaker_object.detach().appendTo(speaker_list);
                speaker_object.offset({ top: offset.top, left: offset.left});
                update_speaker_list();
            }
        });
    };
    /* Delete single speaker */
    var del_speaker = function (speaker_name) {
        $('#'+speaker_name).remove();
    };
    var draw_object = function (object_name, off_left=0, off_top=0) {
        del_object(object_name); // Delete object, if it exists
        var object_class = "<div class='object noselect' id='"+object_name+"' onclick='GUI.load_object_properties(\""+object_name+"\")'>"+object_name+"</div>";
        draw_area.append(object_class);
        var object = $('#'+object_name);
        object.offset({ top: draw_area.offset().top+off_top, left: draw_area.offset().left+off_left}); // Set offset
        object.draggable({ // Make dragable in the draw area
            containment: "#draw_area",
            scroll: false,
            snap: "#draw_area",
            snapMode: "both"
        });
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
        OBJECT.set(obj_name, obj_uri);
        var next_pos = calc_next_object_draw_area();
        draw_object(obj_name, next_pos.left, next_pos.top);
        update_speaker_list();
        load_object_properties(obj_name);
    };
    /* Delete a object via the GUI delete button */
    var delete_object_button = function (object_name) {
        del_object(object_name);
        update_speaker_list();
    };
    /* Delete single object */
    var del_object = function (object_name) {
        $('#'+object_name).remove();
        OBJECT.del[object_name];
    };
    /* Clear objects and speakers respectively */
    var empty_speaker_list = function (not_x = '') {
        if (!(not_x=='not_objects')) {
            $('#speaker_list .object').remove();
            //console.log('clear speaker list: obj');
        }
        if (!(not_x=='not_speakers')) {
            $('#speaker_list .speaker').remove();
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
    /* Calculate the x and y value for next speaker */
    var calc_next_speaker_list_pos = function () {
        var pos={
            top:30,
            left:5
        };
        var number_of_speakers=0;
        var width=speaker_list.width();
        var height=speaker_list.height();
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
            left = $(key).css('left').replace('px', '')-speaker_list.position().left;
            top = $(key).css('top').replace('px', '')-speaker_list.position().top;
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
    /* Calculate the x and y value for next object */
    var calc_next_object_draw_area = function () {
        var pos={
            top:0,
            left:0
        };
        var number_of_objects=0;
        var width=draw_area.width();
        var height=draw_area.height();
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
            left = $(key).css('left').replace('px', '')-draw_area.position().left;
            top = $(key).css('top').replace('px', '')-draw_area.position().top;
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
    /* Convert virt value to IRL value with scale */
    var scale_to_irl = function (value) {
        return ((value*scale_irl)/scale_virt);
    };
    /* Convert IRL value to VIRT value with scale */
    var scale_to_virt = function (value) {
        return ((value*scale_virt)/scale_irl);
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
        update_speaker_list     : update_speaker_list,
        draw_speakers_from_data : draw_speakers_from_data,
        make_data_from_drawing  : make_data_from_drawing,
        save_object_properties  : save_object_properties,
        load_object_properties  : load_object_properties,
        delete_object_button    : delete_object_button,
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
