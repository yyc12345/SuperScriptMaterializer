function tabControlSwitcher(tabcontrolIndex, neededIndex) {
    //disable all
    $(".tabnavigation_" + tabcontrolIndex).each(function () {
        $(this).removeClass("tabitem-activated").addClass("tabitem-deactivated");
    });
    $(".tabpanel_" + tabcontrolIndex).each(function () {
        $(this).hide();
    });

    // set current
    $("#tabnavigation_" + tabcontrolIndex + "_" + neededIndex).removeClass("tabitem-deactivated").addClass("tabitem-activated");
    $("#tabpanel_" + tabcontrolIndex + "_" + neededIndex).show();
}