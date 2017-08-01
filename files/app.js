(function ($) {
    var app = angular.module("default-app", []); 
    app.controller("radio-controller", ['$q', '$http', '$scope', function($q, $http, $scope) {

        var NODE_IP = "192.168.1.90";

        var self = $scope;
        
        self.config = {
            title: "Radio Box",
            bands: [{
                id : 2,
                name: "FM",
                mlt: 100,
                step: 10,
                unit: "MHz"
            }]
            //bands: ["FM", "AM", "KW"],
            //bandValues: [2, 3, 4],
        };

        self.state = {  
            power: true,
            bassBoost: true,
            stereo: false,
            volume: 0,
            station: {
                freq: 0,
                title: "---",
                rds: "---"
            },
            band: self.config.bands[0]
            /*
            bandIndex: function() { 
                return self.config.bands.indexOf(self.state.station.band); 
            } */
        };
        
        self.app = {
            connected: false,
            websocket: null
        };

        self.togglePower = function() {
            console.log("togglePower:", self.state.power);
            if (self.state.power) {
                self.state.volume = 5;
                self.changeVolume();    
            } else {
                self.state.volume = 0;
                self.changeVolume();
            }
        };

        self.toggleBand = function() {
            if (!self.app.connected || !self.state.power)
                return console.log("offline :-(");
            var bandIndex = (self.config.bands.indexOf(self.state.band) + 1) % self.config.bands.length;
            self.state.band = self.config.bands[bandIndex];
            console.log("toggleBand:", self.state.band);
            self.app.websocket.send(JSON.stringify(
                {
                    "name": "band", 
                    "value": self.state.band.id
                }
            ));
        };

        self.moveStation = function(x) {
            //if (!self.app.connected)
            //    return console.log("offline :-(");
            //console.log("moveStation:", x);
        };
        
        self.moveStep = function(x) {
            if (!self.app.connected || !self.state.power)
                return console.log("offline :-(");
            self.state.station.freq += x * self.state.band.step;
            console.log("moveStep:", self.state.station.freq);
            self.app.websocket.send(JSON.stringify(
                {
                    "name": "freq", 
                    "value": self.state.station.freq
                }
            ));
        };

        self.toggleStereo = function() { 
            console.log("toggleStereo:", self.state.stereo);
            if (!self.app.connected || !self.state.power)
                return console.log("offline :-(");
            self.app.websocket.send(JSON.stringify(
                {
                    "name": "mono", 
                    "value": !self.state.stereo
                }
            ));
        };

        self.toggleBass = function() {
            //console.log("toggleBass:", self.state.bassBoost);
        };

        self.changeVolume = function() {
            console.log("changeVolume:", self.state.volume);
            if (!self.app.connected || !self.state.power)
                return console.log("offline :-(");
            self.app.websocket.send(JSON.stringify(
                {
                    "name": "volume", 
                    "value": self.state.volume
                }
            ));
        };

        var connect = function(callback) {
            var socket = new WebSocket("ws://" + (location.host || NODE_IP)); 
            socket.onopen = function() {
                console.log('WS conn');
                self.app.connected = true;
                $scope.$apply();
                if (callback)
                    callback();
            };

            socket.onclose = function(event) {
                console.log("WS conn closed", event);
                self.app.connected = false;
                //if (event.wasClean)
                $scope.$apply();
                connect();
            };

            socket.onmessage = function(event) {
                var data = JSON.parse(event.data);
                if (data.type == "state") {
                    self.state.stereo = !data.mono;
                    self.state.volume = data.volume;
                    self.state.station.freq = data.freq;
                    var matchingBands = self.config.bands.filter(function(b) { return b.id == data.band; });
                    self.state.band = matchingBands.length > 0 ? matchingBands.pop() : self.config.bands[0];
                    $scope.$apply();
                }
                console.log("WS data", data, self.state);
            };

            socket.onerror = function(error) {
                console.log("WS error", error);
            };

            self.app.websocket = socket;
        };

        (function() {
            connect(function() {
                console.log("webSocket connected!");
            });
        })();

    }]);
})(jQuery);
