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

    var init = function () {
        draw_speakers();
    };
    var draw_speakers = function () {
        clear();
        var online = CLIENT.get_online();
        Object.keys(online).forEach(function(key) {
            if(isEmpty(online[key])){
                //draw in speaker list
                add_speaker_list(key);//send id
            } else {
                //something else :P
            }
        });
    };

    var clear = function () {
        clear_speaker_list();
        clear_draw_area();
    };
    var add_speaker_list = function (speaker) {
        speaker_list.append( "<div class='speaker' id='"+speaker+"'>"+speaker.replace('speak_', '')+"</div>" );
        $('#'+speaker).draggable({ containment: "#speaker_list", scroll: false });
    };
    var del_speaker_list = function (speaker) {
        $('#'+speaker).remove();
    };
    var clear_speaker_list = function () {
        speaker_list.empty();
    };

    var add_draw_area = function (obj) {
    };
    var del_draw_area = function (obj) {
    };
    var clear_draw_area = function () {
    };


    return {
        init: init,
        draw_speakers: draw_speakers,
        clear: clear
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
