//=======================================settings
currentSettings = {
    "plink": true,
    "properties": true,
    "highlight": true,
    "keyboard": true,
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
    realTarget = ".target" + target

    if (previousHighlight != "") {
        //need restore
        $(previousHighlight).each(function () {
            if ($(this).hasClass("link-blink")) {
                $(this).attr("stroke", "black")
            }
            if ($(this).hasClass("link-blinkDelay")) {
                $(this).attr("fill", "black")
            }
            if ($(this).hasClass("link-plink")) {
                $(this).attr("stroke", "blue")
            }
            if ($(this).hasClass("link-elink")) {
                $(this).attr("stroke", "cyan")
            }
        });
    }

    // double one-click, only cancel highlight and don't apply any hightlight
    if (realTarget == previousHighlight) {
        previousHighlight = "";
    } else {
        //apply new highlight
        $(realTarget).each(function () {
            if ($(this).hasClass("link-blink")) {
                $(this).attr("stroke", "yellow")
            }
            if ($(this).hasClass("link-blinkDelay")) {
                $(this).attr("fill", "yellow")
            }
            if ($(this).hasClass("link-plink")) {
                $(this).attr("stroke", "orange")
            }
            if ($(this).hasClass("link-elink")) {
                $(this).attr("stroke", "orange")
            }
        });

        previousHighlight = realTarget
    }

    //cancel event seperate
    event.stopPropagation();
}

function queryInfo(obj) {
    $.post(window.location,
        {
            operation: "info",
            target: obj
        },
        function (data, status) {
            //set id
            $("#sidepanel-properties-id").text(obj);

            //set data
            $("#sidepanel-properties-container").empty()
            for (var key in data) {
                $("#sidepanel-properties-container").append("<p><b>" + key + ":</b><br /><pre>" + data[key] + "</pre></p>")
            }
        });
}

function sidePanelSwitcher(target) {
    // 0->property; 1->display; 2->tools
    //disable all
    $("#sidepanel-properties").hide();
    $("#sidepanel-display").hide();
    $("#sidepanel-tools").hide();
    $(".tabitem").each(function () {
        $(this).removeClass("tabitem-activated").addClass("tabitem-deactivated");
    });

    switch (target) {
        case 0:
            $("#sidepanel-properties").show();
            $(".tabitem1").each(function () {
                $(this).removeClass("tabitem-deactivated").addClass("tabitem-activated");
            });
            break;
        case 1:
            $("#sidepanel-display").show();
            $(".tabitem2").each(function () {
                $(this).removeClass("tabitem-deactivated").addClass("tabitem-activated");
            });
            break;
        case 2:
            $("#sidepanel-tools").show();
            $(".tabitem3").each(function () {
                $(this).removeClass("tabitem-deactivated").addClass("tabitem-activated");
            });
            break;
    }
}
