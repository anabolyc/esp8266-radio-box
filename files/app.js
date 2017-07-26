(function ($) {
    var app = angular.module("default-app", []); 
    app.controller("radio-controller", ['$q', '$http', '$scope', function($q, $http, $scope) {

        var self = $scope;
        
        self.pub = {            

        };
        
        self.togglePower = function() {
            console.log("togglePower");
        };

        (function() {
            console.log("Hello!");
        })();

    }]);
})(jQuery);
