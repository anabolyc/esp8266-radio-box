var app = {
    websocket: null,
    state: {},
    ui: {
        powerBtn: null,
        muteBtn: null,
        mixingBtn: null,
        enhanceBtn: null,
        volumeFR: null,
    },
    onmessage: function (event) {
        var time = 0;
        var json = JSON.parse(event.data);
        if (json) {
            if (json.type == 'state') {
                app.state = json;
                app.updateState();
            } else if (json.type == 'user-leave') {
                writeToScreen('Хтось пішов :( , нас тепер:' + json.count);
            }
        }
        console.log(json);
    },

    sendBoolean: function (name) {
        app.websocket.send(JSON.stringify({'name': name}));
    },

    sendValue: function (name, val) {
        app.websocket.send(JSON.stringify({'name': name, val: val}));
    },
    sendLcdValue: function (name, line, val) {
        app.websocket.send(JSON.stringify({'name': name, val: val, line: line}));
    },

    updateState: function () {
        app.ui.powerBtn.attr('class', app.state.power ? 'btn btn-success' : 'btn btn-default');
        app.ui.muteBtn.attr('class', app.state.mute ? 'btn btn-success' : 'btn btn-default');
        app.ui.mixingBtn.attr('class', app.state.mixing ? 'btn btn-success' : 'btn btn-default');
        app.ui.enhanceBtn.attr('class', app.state.enhance ? 'btn btn-success' : 'btn btn-default');
        app.ui.volumeFR.val(app.state.volumeFR);
        app.ui.volumeFL.val(app.state.volumeFL);
        app.ui.volumeRR.val(app.state.volumeRR);
        app.ui.volumeRL.val(app.state.volumeRL);
        app.ui.volumeCEN.val(app.state.volumeCEN);
        app.ui.volumeSW.val(app.state.volumeSW);
        app.ui.volumeALLCH.val(app.state.volumeALLCH);
        app.ui.frequency.val(app.state.frequency);
        jQuery('#freq').html(app.state.frequency);

        app.ui.source.removeClass('btn-success');
        app.ui.source.filter('[data-value="' + app.state.source + '"]').addClass('btn-success');

    },

    init: function () {

        if (location.host == '') {
            app.websocket = new WebSocket("ws://192.168.1.20/");
        } else {
            app.websocket = new WebSocket("ws://" + location.host + "/");
        }

        app.websocket.onmessage = app.onmessage;

        app.ui.powerBtn = jQuery('#power');
        app.ui.muteBtn = jQuery('#mute');
        app.ui.mixingBtn = jQuery('#mixing');
        app.ui.enhanceBtn = jQuery('#enhance');

        app.ui.volumeFR = jQuery('#volumeFR');
        app.ui.volumeFL = jQuery('#volumeFL');
        app.ui.volumeRR = jQuery('#volumeRR');
        app.ui.volumeRL = jQuery('#volumeRL');
        app.ui.volumeCEN = jQuery('#volumeCEN');
        app.ui.volumeSW = jQuery('#volumeSW');
        app.ui.volumeALLCH = jQuery('#volumeALLCH');
        app.ui.frequency = jQuery('#frequency');

        app.ui.lcdTextInput = jQuery('#lcd-text');
        app.ui.lcdTextSendBtn = jQuery('#lcd-text-send');

        app.ui.lcdTextSendBtn.click(function () {
            app.sendLcdValue('lcdText', 0, app.ui.lcdTextInput.val());
        });

        app.ui.source = jQuery('button.source');
        app.ui.tune = jQuery('button.tune');

        app.attachButtonHandler(app.ui.powerBtn);
        app.attachButtonHandler(app.ui.mixingBtn);
        app.attachButtonHandler(app.ui.muteBtn);
        app.attachButtonHandler(app.ui.enhanceBtn);

        app.attachRangeHandler(app.ui.frequency);
        app.attachRangeHandler(app.ui.volumeFR);
        app.attachRangeHandler(app.ui.volumeFL);
        app.attachRangeHandler(app.ui.volumeRR);
        app.attachRangeHandler(app.ui.volumeRL);
        app.attachRangeHandler(app.ui.volumeCEN);
        app.attachRangeHandler(app.ui.volumeSW);
        app.attachRangeHandler(app.ui.volumeALLCH);

        app.attachSingleChoiceHandler(app.ui.source);
        app.attachTuneHandler(app.ui.tune);


    },

    attachButtonHandler: function (btn) {
        btn.click(function () {
            app.sendBoolean(btn.attr('id'));
        });
    }
    ,
    attachRangeHandler: function (range) {
        range.change(function () {
            console.log(range.attr('id'), range.val());
            app.sendValue(range.attr('id'), range.val());
        });
    },
    attachSingleChoiceHandler: function (buttons) {
        buttons.click(function (e) {
            e.stopImmediatePropagation();
            console.log(e.target);
            app.sendValue(jQuery(e.target).attr('data-paramname'), jQuery(e.target).attr('data-value'));
        });
    },

    attachTuneHandler: function (buttons) {
        buttons.click(function (e) {
            e.stopImmediatePropagation();
            var frequency = parseFloat(jQuery(e.target).attr('data-value'))+app.state.frequency;
            console.log(frequency);
            app.sendValue('frequency', frequency);

        });
    }


};
window.addEventListener("load", function () {
    app.init();
}, false);