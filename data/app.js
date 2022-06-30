(function ($) {
    var app = angular.module("default-app", []); 
    app.controller("radio-controller", ['$q', '$http', '$scope', function($q, $http, $scope) {

        var NODE_IP = "192.168.0.36";

        var self = $scope;
        
        self.config = {
            title: "Radio Box",
            bands: [{
                id : 2,
                name: "FM",
                mlt: 100,
                step: 10,
                precision: 1,
                unit: "MHz"
            }],
            host: location.host == "localhost:8080" ? NODE_IP : location.host
        };

        self.capabilities = {
            volume: true,
            bass: true
        };
        
        self.state = {  
            power: true,
            bassBoost: true,
            stereo: false,
            volume: {
                value: 0,
                stored: 0
            },
            station: {
                freq: 0,
                title: "---",
                rds: "---"
            },
            band: self.config.bands[0]
        };

        
        self.app = {
            connected: false,
            websocket: null
        };

        self.togglePower = function() {
            if (!self.app.connected)
                return console.log("offline :-(");
            console.log("togglePower:", self.state.power);
            self.app.websocket.send(JSON.stringify({
                "name": "power", 
                "value": self.state.power
            }));
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
            if (!self.app.connected)
                return console.log("offline :-(");
            console.log("moveStation:", x);
            self.app.websocket.send(JSON.stringify(
                {
                    "name": x == 1 ? "seek_up" : "seek_down", 
                    "value": null
                }
            ));
        };
        
        self.moveStep = function(x) {
            if (!self.app.connected || !self.state.power)
                return console.log("offline :-(");
            self.state.station.freq += x * self.state.band.step;
            var k = Math.pow(10, self.state.band.precision);
            self.state.station.freq = Math.round(self.state.station.freq / k) * k;
            console.log("moveStep:", self.state.station.freq);
            self.app.websocket.send(JSON.stringify({
                "name": "freq", 
                "value": self.state.station.freq
            }));
        };

        self.toggleStereo = function() { 
            console.log("toggleStereo:", self.state.stereo);
            if (!self.app.connected)
                return console.log("offline :-(");
            self.app.websocket.send(JSON.stringify(
                {
                    "name": "mono", 
                    "value": !self.state.stereo
                }
            ));
        };

        self.toggleBass = function() {
            console.log("toggleBass:", self.state.bassBoost);
            if (!self.app.connected)
                return console.log("offline :-(");
            self.app.websocket.send(JSON.stringify(
                {
                    "name": "bass_boost", 
                    "value": self.state.bassBoost
                }
            ));
        };

        self.changeVolume = function(value) {
            value = typeof(value) == "undefined" ? self.state.volume.value : value;
            console.log("changeVolume:", value);
            if (!self.app.connected)
                return console.log("offline :-(");
            self.app.websocket.send(JSON.stringify(
                {
                    "name": "volume", 
                    "value": value
                }
            ));
        };

        var connect = function(callback) {
            var socket = new WebSocket("ws://" + self.config.host + ":81"); 
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
                switch (data.type) {
                    case "state":
                        self.state.stereo = !data.mono;
                        if (self.capabilities.volume) 
                            self.state.volume.value = data.volume;
                        self.state.station.freq = data.freq;
                        self.state.bassBoost = data.bass;
                        var matchingBands = self.config.bands.filter(function(b) { return b.id == data.band; });
                        self.state.band = matchingBands.length > 0 ? matchingBands.pop() : self.config.bands[0];
                        break;
                    case "capabilities":
                        self.capabilities = data;
                        break;
                }
                
                $scope.$apply(); 
                //console.log("WS data", data, self.state);
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
