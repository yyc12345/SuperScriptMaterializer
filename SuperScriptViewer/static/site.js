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
