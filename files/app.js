(function ($) {
    var app = angular.module("default-app", []); 
    app.controller("radio-controller", ['$q', '$http', '$scope', function($q, $http, $scope) {

        var self = $scope;
        
        self.state = {  
            title: "Radio Box",       
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
            bands: ["AM", "FM"],
            freqStep: [100, 0.1],

        };
        
        self.togglePower = function() {
            console.log("togglePower:", self.state.power);
        };

        self.toggleBand = function() {
            var newBand = self.config.bands[(self.state.bandIndex() + 1) % self.config.bands.length];
            self.state.station.band = newBand;
            console.log("togglePower:", newBand);
        };

        self.moveStation = function(x) {
            console.log("moveStation:", x);
        };
        
        self.moveStep = function(x) {
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

        (function() {
            console.log("init");
        })();

    }]);
})(jQuery);
