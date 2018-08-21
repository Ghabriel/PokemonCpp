"use strict";
window.addEventListener('load', function () {
    var sidebar = document.getElementById('sidebar');
    var content = document.getElementById('content');
    var links = sidebar.getElementsByTagName('a');
    var _loop_1 = function (i) {
        var link = links[i];
        link.addEventListener('click', function (e) {
            var relativeUrl = link.href.substr(link.href.indexOf('docs/'));
            $(content).load(relativeUrl);
            e.preventDefault();
        });
    };
    for (var i = 0; i < links.length; i++) {
        _loop_1(i);
    }
});
