function doQuery(fieldIndex, queryTag) {
    // collect data
    var readyData = {};
    $("#queryTable_" + fieldIndex + " tr:not(:first-child)").each(function() {
        var isEnabled = $(this).find(":nth-child(1) input").prop("checked");
        if (!isEnabled) return;

        var fieldName = $(this).find(":nth-child(2)").attr("queryName");
        var fieldValue = $(this).find(":nth-child(3) input").val();

        readyData[fieldName] = fieldValue;
    });

    var jsonData = JSON.stringify(readyData);

    // raise post
    $.post(window.location,
        {
            tag: queryTag,
            data: jsonData
        },
        function (data, status) {
            // remove data
            $("#resultTable_" + fieldIndex + " tr:not(:first-child)").remove();

            // check
            if (!data['status']) {
                alert("Fail to query!");
                return;
            }

            // check overflow
            if (data['overflow']) $("#resultTableOverflow_" + fieldIndex).show();
            else $("#resultTableOverflow_" + fieldIndex).hide();

            // insert data
            for(var i = 0; i < data['data'].length; i++) {
                $("#resultTable_" + fieldIndex).append("<tr></tr>");
                for(var j = 0; j < data['data'][i].length; j++) {
                    $("#resultTable_" + fieldIndex + " tr:last").append("<td></td>");
                    $("#resultTable_" + fieldIndex + " tr:last td:last").text(data['data'][i][j]);
                }
            }
        });
}