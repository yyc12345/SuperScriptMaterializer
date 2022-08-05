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
            if (!checkByteUnit(words)) {
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
            if (!checkByteUnit(words)) {
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
    var textSp = $("#conv2Output").val().split("\n");
    var result = new Array();

    for (var index = 0; index < textSp.length; index++) {
        var lines = textSp[index].toLowerCase();
        var resultCache = new Array();

        if (!checkHexInput(lines)) {
            result[index] = "";
            continue;
        }

        var sectorCount = parseInt(lines.length / 2);
        var remainCount = lines.length % 2;

        for (var innerIndex = 0; innerIndex < sectorCount; innerIndex++) {
            if (innerIndex == 0)
                resultCache[innerIndex] = "0x" + lines.slice(-(innerIndex + 1) * 2);
            else
                resultCache[innerIndex] = "0x" + lines.slice(-(innerIndex + 1) * 2, -innerIndex * 2);
        }
        if (remainCount != 0)
            resultCache[sectorCount] = "0x0" + lines[0];

        for (var addedIndex = resultCache.length; addedIndex < 4; addedIndex++) {
            resultCache[addedIndex] = "0x00";
        }

        result[index] = resultCache.join(", ");
    }

    $("#conv2Input").val(result.join("\n"));
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
            if (!checkByteUnit(words)) {
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
    var textSp = $("#conv3Output").val().split("\n");
    var result = new Array();

    for (var index = 0; index < textSp.length; index++) {
        var declines = textSp[index];
        var resultCache = new Array();

        if (!checkDecInput(declines)) {
            result[index] = "";
            continue;
        }

        var bigInt = BigInt(declines);
        var lines = bigInt.toString(16).toLowerCase();

        var sectorCount = parseInt(lines.length / 2);
        var remainCount = lines.length % 2;

        for (var innerIndex = 0; innerIndex < sectorCount; innerIndex++) {
            if (innerIndex == 0)
                resultCache[innerIndex] = "0x" + lines.slice(-(innerIndex + 1) * 2);
            else
                resultCache[innerIndex] = "0x" + lines.slice(-(innerIndex + 1) * 2, -innerIndex * 2);
        }
        if (remainCount != 0)
            resultCache[sectorCount] = "0x0" + lines[0];

        for (var addedIndex = resultCache.length; addedIndex < 4; addedIndex++) {
            resultCache[addedIndex] = "0x00";
        }

        result[index] = resultCache.join(", ");
    }

    $("#conv3Input").val(result.join("\n"));
}

// ============================= conv 4

function doConv4_I2O() {
    $("#conv4Output").val($("#conv4Input").val().toUpperCase());
}

function doConv4_O2I() {
    $("#conv4Input").val($("#conv4Output").val().toLowerCase());
}

// ============================= utils

function checkDecInput(s) {
    var legalWords = "0123456789";
    if (s.length == 0) return false;
    for (var i = 0; i < s.length; i++) {
        if (legalWords.indexOf(s[i]) == -1)
            return false;
    }
    return true;
}

function checkHexInput(s) {
    var legalWords = "0123456789abcdef";
    for (var i = 0; i < s.length; i++) {
        if (legalWords.indexOf(s[i]) == -1)
            return false;
    }
    return true;
}

function checkByteUnit(s) {
    var legalWords = "0123456789abcdefABCDEF";
    if (s.length != 4) return false;
    if (s[0] != '0') return false;
    if (s[1] != 'x') return false;
    if (legalWords.indexOf(s[2]) == -1) return false;
    if (legalWords.indexOf(s[3]) == -1) return false;
    return true;
}