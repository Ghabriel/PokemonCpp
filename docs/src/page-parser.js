"use strict";
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (Object.hasOwnProperty.call(mod, k)) result[k] = mod[k];
    result["default"] = mod;
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
var fs = __importStar(require("fs"));
var baseFolder = 'pages';
var parsedFolder = '..';
var templateFile = fs.readFileSync('template.html').toString();
var files = fs.readdirSync(baseFolder);
var _loop_1 = function (file) {
    fs.readFile(baseFolder + "/" + file, function (err, data) {
        if (err) {
            throw err;
        }
        var fileContent = data.toString();
        var parsedContent = templateFile.replace('{{content}}', fileContent);
        fs.writeFile(parsedFolder + "/" + file, parsedContent, function (writeErr) {
            if (writeErr) {
                throw writeErr;
            }
        });
    });
};
for (var _i = 0, files_1 = files; _i < files_1.length; _i++) {
    var file = files_1[_i];
    _loop_1(file);
}
