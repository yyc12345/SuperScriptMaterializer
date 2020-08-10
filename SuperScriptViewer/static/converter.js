// ============================= conv 1

function doConv1_I2O() {
    var textSp = $("#conv1Input").val().split("\n");
    var result = new Array();

    for (var index = 0; index < textSp.length; index++) {
        var lines = textSp[index];
        var successful = true;
        var resultCache = new Array();

        lineSp = lines.split(',');
        var innerIndex = lineSp.length - 1;
        for (var forwardIndex = 0; forwardIndex < lineSp.length; forwardIndex++) {
            var words = lineSp[forwardIndex];
            words = words.trim().toLowerCase();
            if (!checkByteUnit(words)){
                resultCache.length = 0;
                successful = false;
                break;
            } else 
                resultCache[innerIndex] = words;
            innerIndex--;            
        }

        if (successful) 
            result[index] = resultCache.join(", ");
        else
            result[index] = "";

    }

    $("#conv1Output").val(result.join("\n"));
}

// ============================= conv 2

function doConv2_I2O() {
    var textSp = $("#conv2Input").val().split("\n");
    var result = new Array();

    for (var index = 0; index < textSp.length; index++) {
        var lines = textSp[index];
        var successful = true;
        var resultCache = "";

        lineSp = lines.split(',');
        var innerIndex = lineSp.length - 1;
        for (var forwardIndex = 0; forwardIndex < lineSp.length; forwardIndex++) {
            var words = lineSp[forwardIndex];
            words = words.trim().toLowerCase();
            if (!checkByteUnit(words)){
                resultCache.length = 0;
                successful = false;
                break;
            } else 
                resultCache = words.substr(2, 2) + resultCache;
            innerIndex--;            
        }

        if (successful) 
            result[index] = resultCache;
        else
            result[index] = "";

    }

    $("#conv2Output").val(result.join("\n"));
}

function doConv2_O2I() {
    
}

// ============================= conv 3

function doConv3_I2O() {
    var textSp = $("#conv3Input").val().split("\n");
    var result = new Array();

    for (var index = 0; index < textSp.length; index++) {
        var lines = textSp[index];
        var successful = true;
        var resultCache = "";

        lineSp = lines.split(',');
        var innerIndex = lineSp.length - 1;
        for (var forwardIndex = 0; forwardIndex < lineSp.length; forwardIndex++) {
            var words = lineSp[forwardIndex];
            words = words.trim().toLowerCase();
            if (!checkByteUnit(words)){
                resultCache.length = 0;
                successful = false;
                break;
            } else 
                resultCache = words.substr(2, 2) + resultCache;
            innerIndex--;            
        }

        if (successful) {
            var bigInt = BigInt("0x" + resultCache);
            result[index] = bigInt.toString(10);
        }
        else
            result[index] = "";

    }

    $("#conv3Output").val(result.join("\n"));
}

function doConv3_O2I() {

}

// ============================= conv 4

function doConv4_I2O() {
    $("#conv4Output").val($("#conv4Input").val().toUpperCase());
}

function doConv4_O2I() {
    $("#conv4Input").val($("#conv4Output").val().toLowerCase());
}

// ============================= utils

function checkByteUnit(s) {
    var legalWords = "0123456789abcdefABCDEF";
    if (s.length != 4) return false;
    if (s[0] != '0') return false;
    if (s[1] != 'x') return false;
    if (legalWords.indexOf(s[2]) == -1) return false;
    if (legalWords.indexOf(s[3]) == -1) return false;
    return true;
}