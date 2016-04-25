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
    };
    var draw_speakers = function () {
        clear();
        var online = CLIENT.get_online();
        var next_pos;
        Object.keys(online).forEach(function(key, i) {
            if(isEmpty(online[key])){
                //draw in speaker list
                next_pos = calc_next_speaker_list_pos();
                draw_speaker(key, speaker_list, next_pos.left, next_pos.top);//send id
                //draw_speaker(key, speaker_list);
            } else {
                //something else :P
            }
        });
    };
    var clear = function () {
        empty_speaker_list();
        empty_draw_area();
    };
    /* Draw speakers from CLIENT data */
    var draw_speakers_from_data = function () {
        empty_draw_area(); // Clear the draw area
        var nr_of_objects=0;
        var object_tmp = $("<div class='object' id='object_tmp'></div>").hide().appendTo("body"); //add temp object
        var object_width = parseInt($('.object').css('width').replace('px',''));
        var object_height= parseInt($('.object').css('height').replace('px',''));
        object_tmp.remove(); //remove temp object
        //get all draws speakers
        //Delete unused speakers
        $('.speaker').each(function(key, value){
            if (!(CLIENT.get_online()[$(this).attr('id')])) {
                console.log("Deleting unused drawn: "+$(this).attr('id'));
                $(this).remove();
            }
        });
        //Loop all speakers
        $.each(CLIENT.get_online(), function(key, value){
            //has object(s)?
            var speaker_name = key;
            if (!isEmpty(value)) { // Has object(s)
                console.log("Has object");
                //do things
                //loop objects
                nr_of_objects=0;
                $.each(CLIENT.get_online()[key], function(obj, obj_value){
                    if (!nr_of_objects) {
                        //first object
                        console.log("First object");
                        //already drawn?
                        if (isEmpty($('#'+obj))) {
                            //No, speaker not yet drawn
                            //- draw object in center of draw area
                            //- draw speaker from that x and y axis
                            console.log('obj '+obj+' not drawn');
                            //var width=speaker_list.width();
                            //var height=speaker_list.height();
                            draw_object(obj, ((draw_area.width()-object_width)/2), ((draw_area.height()-object_height)/2));
                            //var speaker_width=parseInt($('.speaker').css('width').replace('px',''));
                        }
                        //draw speaker from object
                        del_speaker(speaker_name); //delete speaker
                        var left =$('#'+obj).offset().left-draw_area.offset().left-rectangular(obj_value.distance, obj_value.angle).x;
                        var top  =$('#'+obj).offset().top-draw_area.offset().top+rectangular(obj_value.distance, obj_value.angle).y;
                        draw_speaker(speaker_name, draw_area, left, top); //draw speaker
                    } else {
                        //not first obj
                        //object already drawn?
                        //yes, do nothing (speaker is alrady drwn in field)
                        //no: draw the object seen from the speaker
                        if (isEmpty($('#'+obj))) {
                            //draw the object from speaker
                            //del_object(obj);//delete object
                            var left =$('#'+speaker_name).offset().left-draw_area.offset().left+rectangular(obj_value.distance, obj_value.angle).x;
                            var top  =$('#'+speaker_name).offset().top-draw_area.offset().top-rectangular(obj_value.distance, obj_value.angle).y;
                            draw_object(obj, left, top); //draw object
                        }
                    }
                    console.log(obj);
                    console.log(obj_value);
                    nr_of_objects++;
                }, speaker_name);
            } else { // Has no objects
                console.log("Has no object");
                if (isEmpty($('#'+key))) {//not drawn
                    console.log('Speaker not drawn: '+key);
                    //draw in speaker list
                    draw_speaker(key, speaker_list, calc_next_speaker_list_pos().left, calc_next_speaker_list_pos().top);
                } //if already drawn, just leave!!!
            }
        });
    };
    /* Make CLIENT data from drawing */
    var make_data_from_drawing = function () {

    };
    /* Draw single speaker */
    var draw_speaker = function (speaker_name, destination, off_left=0, off_top=0) {
        var speaker_class = "<div class='speaker' id='"+speaker_name+"'>"+speaker_name.replace(CONFIG.name_speaker, '')+"</div>";
        destination.append( speaker_class );
        var speaker = $('#'+speaker_name);
        //set speaker offset
        speaker.offset({ top: destination.offset().top+off_top, left: destination.offset().left+off_left});
        //make speaker dragable
        speaker.draggable({
            containment: "#top",
            scroll: false,
            snap: "#speaker_list",
            snapMode: "both"
        });
        //make speaker dropable in varius things
        draw_area.droppable({
            drop: function( event, ui ) {
                var off = ui.offset;
                var pos = ui.position;
                var speak_obj= $(ui.helper[0]);
                speak_obj.detach().appendTo(draw_area);
                speak_obj.offset({ top: off.top, left: off.left});
              }
        });
        speaker_list.droppable({
            drop: function( event, ui ) {
                var off = ui.offset;
                var speak_obj= $(ui.helper[0]);
                speak_obj.detach().appendTo(speaker_list);
                speak_obj.offset({ top: off.top, left: off.left});
            }
        });
    };
    /* Delete single speaker */
    var del_speaker = function (speaker_name) {
        $('#'+speaker_name).remove();
    };
    var draw_object = function (object_name, off_left=0, off_top=0) {
        console.log('draw at: '+off_left+' '+off_top);
        var object_class = "<div class='object' id='"+object_name+"'>"+object_name+"</div>";
        draw_area.append( object_class );
        var object = $('#'+object_name);
        //set offset
        object.offset({ top: draw_area.offset().top+off_top, left: draw_area.offset().left+off_left});
        //make dragable
        object.draggable({
            containment: "#draw_area",
            scroll: false,
            snap: "#draw_area",
            snapMode: "both"
        });
    };
    /* Delete single object */
    var del_object = function (object_name) {
        $('#'+object_name).remove();
    };
    /* Clear objects and speakers respectively */
    var empty_speaker_list = function () {
        $('#speaker_list .speaker').remove();
        $('#speaker_list .object').remove();
    };
    var empty_draw_area = function () {
        $('#draw_area .speaker').remove();
        $('#draw_area .object').remove();
    };
    var polar = function(x, y) {
        var polar = {
            distance:0,
            angle:0
        };
        polar.distance = Math.sqrt(Math.pow(x, 2)+Math.pow(y, 2)); // sqrt(x² + y²)
        polar.angle = Math.degrees(Math.atan2(y, x)); // tan⁻¹(Y/X)
        return polar;
    };
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

    var calc_next_speaker_list_pos = function () {
        /* Calculate the x and y value for next speaker */
        var pos={
            top:0,
            left:0
        };
        var number_of_speakers=0;
        var width=speaker_list.width();
        var height=speaker_list.height();
        var speaker_temp = $("<div  class='speaker' id='speaker_temp'></div>").hide().appendTo("body"); //add temp speakers
        var speaker_boundry = 50;//% (margin atound speakers)
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

    return {
        init: init,
        draw_speakers: draw_speakers,
        draw_object:draw_object,
        draw_speakers_from_data: draw_speakers_from_data,
        clear: clear,
        polar: polar,
        calc_next_speaker_list_pos:calc_next_speaker_list_pos,
        rectangular: rectangular,
        scale_to_irl:scale_to_irl,
        scale_to_virt:scale_to_virt
    };
})(window);

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
