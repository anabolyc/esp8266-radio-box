(function ($) {
    var app = angular.module("default-app", []); 
    app.controller("radio-controller", ['$q', '$http', '$scope', function($q, $http, $scope) {

        var NODE_IP = "192.168.1.90";

        var self = $scope;
        
        self.state = {  
            power: true,
            bassBoost: true,
            stereo: false,
            volume: 0,
            station: {
                band: "FM",
                freq: 102.3,
                unit: "MHz",
                title: "Radio Wrocław",
                rds: "RDS blah blah"
            },
            bandIndex: function() { return self.config.bands.indexOf(self.state.station.band); }
        };

        self.config = {
            title: "Radio Box",
            bands: ["AM", "FM"],
            freqStep: [100, 0.1],
        };
        
        self.app = {
            connected: false,
            websocket: null
        };

        self.togglePower = function() {
            console.log("togglePower:", self.state.power);
        };

        self.toggleBand = function() {
            if (!self.app.connected)
                return console.log("offline :-(");
            var newBand = self.config.bands[(self.state.bandIndex() + 1) % self.config.bands.length];
            self.state.station.band = newBand;
            console.log("togglePower:", newBand);
        };

        self.moveStation = function(x) {
            if (!self.app.connected)
                return console.log("offline :-(");
            console.log("moveStation:", x);
        };
        
        self.moveStep = function(x) {
            if (!self.app.connected)
                return console.log("offline :-(");
            self.state.station.freq += x * self.config.freqStep[self.state.bandIndex()];
            console.log("moveStep:", x);
        };

        self.toggleStereo = function() { 
            console.log("toggleStereo:", self.state.stereo);
        };

        self.toggleBass = function() {
            console.log("toggleBass:", self.state.bassBoost);
        };

        self.changeVolume = function() {
            console.log("changeVolume:", self.state.volume);
        };

        var connect = function() {
            var socket = new WebSocket("ws://" + (location.host || NODE_IP)); 
            socket.onopen = function() {
                console.log('WS conn');
                self.app.connected = true;
                $scope.$apply();
            };

            socket.onclose = function(event) {
                console.log("WS conn closed", event);
                self.app.connected = false;
                //if (event.wasClean)
                $scope.$apply();
                connect();
            };

            socket.onmessage = function(event) {
                console.log("WS data", event);
            };

            socket.onerror = function(error) {
                console.log("WS error", error);
            };

            self.app.websocket = socket;
        };

        (function() {
            connect();
        })();

    }]);
})(jQuery);
