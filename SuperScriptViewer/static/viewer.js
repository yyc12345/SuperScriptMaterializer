//=======================================settings
currentSettings = {
    "plink": true,
    "properties": true,
    "highlight": true,
    "move": true
};
$(document).ready(function () {
    //read settings and apply it
    for (var key in currentSettings) {
        currentSettings[key] = localstorageAssist_Get("ssm-settings-" + key, "true") == "true";
        if (currentSettings[key]) $("#sidepanel-display-" + key).prop("checked", true);
        else $("#sidepanel-display-" + key).removeProp("checked");
    }
    //additional settings
    if (!currentSettings["plink"]) {
        $(".link-elink").hide();
        $(".link-plink").hide();
    }
});
function settingChange(target) {
    newValue = $("#sidepanel-display-" + target).prop("checked");
    currentSettings[target] = newValue;
    localstorageAssist_Set("ssm-settings-" + target, newValue);

    if (target == "plink") {
        if (currentSettings["plink"]) {
            $(".link-elink").show();
            $(".link-plink").show();
        } else {
            $(".link-elink").hide();
            $(".link-plink").hide();
        }
    }
}
function localstorageAssist_Get(index, defaultValue) {
    var cache = localStorage.getItem(index);
    if (cache == null) {
        localstorageAssist_Set(index, defaultValue);
        return defaultValue;
    } else return cache;
}
function localstorageAssist_Set(index, value) {
    localStorage.setItem(index, value);
}


//=======================================internal event
previousHighlight = "";
function highlightLink(target) {
    realTarget = ".target" + target;

    if (previousHighlight != "") {
        //need restore
        $(previousHighlight).each(function () {
            if ($(this).hasClass("link-blink")) {
                $(this).attr("stroke", "black");
            }
            if ($(this).hasClass("link-blinkDelay")) {
                $(this).attr("fill", "black");
            }
            if ($(this).hasClass("link-plink")) {
                $(this).attr("stroke", "blue");
            }
            if ($(this).hasClass("link-elink")) {
                $(this).attr("stroke", "cyan");
            }
        });
    }

    // double one-click, only cancel highlight and don't apply any hightlight
    // or user disable hightlight
    if ((realTarget == previousHighlight) || !currentSettings["highlight"]) {
        previousHighlight = "";
    } else {
        //apply new highlight
        $(realTarget).each(function () {
            if ($(this).hasClass("link-blink")) {
                $(this).attr("stroke", "yellow");
            }
            if ($(this).hasClass("link-blinkDelay")) {
                $(this).attr("fill", "yellow");
            }
            if ($(this).hasClass("link-plink")) {
                $(this).attr("stroke", "orange");
            }
            if ($(this).hasClass("link-elink")) {
                $(this).attr("stroke", "orangered");
            }
        });

        previousHighlight = realTarget;
    }

    //cancel event seperate
    event.stopPropagation();
}

// type = 0: call from cell
// type = 1: call from bb
function queryInfo(type, obj) {
    // confirm user enable this function
    if (!currentSettings["properties"])
        return;

    $.post(window.location,
        {
            operation: "info",
            tag: type,
            target: obj
        },
        function (data, status) {
            //set target
            $("#sidepanel-properties-target b").text(obj);

            //set data
            $("#sidepanel-properties-container").empty();
            for (var key in data) {
                $("#sidepanel-properties-container").append("<div class=\"propertyItem\"></div>");

                var box = $("#sidepanel-properties-container div:last");
                if (data[key]["is_setting"])
                    $(box).append("<p><code class=\"propertyItem\">S</code><b></b><i></i></p>");
                else
                    $(box).append("<p><b></b><i></i></p>");

                $(box).find("p b").text(data[key]["name"]);
                $(box).find("p i").text("(" + key + ")");

                for (var i = 0; i < data[key]['data'].length; i++) {
                    $(box).append("<p></p><pre class=\"propertyItem\"></pre>");
                    $(box).find("p:last").text(data[key]['data'][i][0]);
                    $(box).find("pre:last").text(data[key]['data'][i][1]);
                }
            }
        });
}
