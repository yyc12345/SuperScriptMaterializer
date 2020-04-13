previousHighlight = ""

function highlightLink(target) {
    realTarget = ".target" + target

    if (previousHighlight != "") {
        //need restore
        $(previousHighlight).each(function() {
            if ($(this).hasClass("link-blink")) {
                $(this).attr("stroke", "black")
            }
            if ($(this).hasClass("link-blinkDelay")) {
                $(this).attr("fill", "black")
            }
        });
    }

    //apply new highlight
    $(realTarget).each(function() {
        if ($(this).hasClass("link-blink")) {
            $(this).attr("stroke", "yellow")
        }
        if ($(this).hasClass("link-blinkDelay")) {
            $(this).attr("fill", "yellow")
        }
    });

    previousHighlight = realTarget
    //cancel event seperate
    event.stopPropagation();
}

function queryInfo(obj) {
    $.post(window.location,
        {
            operation: "info",
            target: obj
        },
        function(data, status) {
            //set id
            $("#propertyWindow-id").text(obj);

            //set data
            $("#propertyWindow-container").empty()
            for (var key in data) {
                $("#propertyWindow-container").append("<p><b>" + key + ":</b><br />" + data[key] +"</p>")
            }

            $("#propertyWindow-main").show();
        });
}

function closePropertyWindow() {
    $("#propertyWindow-main").hide();
}
