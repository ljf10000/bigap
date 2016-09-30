#!/bin/js

var mod = this,
    intf = require('product/helper/interface'),
	nginx = {
		proxy: [],
		redirect: [],
		bind: []
	};

const name = 'product/helper/protocol',
	NGINX = {
		OFFSET: 3,
		PROXY: 57610,
		REDIRECT: 57650
	};

function init () {
	var i;

	for (i=0; i<intf.WAN.COUNT; i++) {
		nginx.proxy[i] = NGINX.PROXY + NGINX.OFFSET;
		nginx.redirect[i] = NGINX.REDIRECT + NGINX.OFFSET;
		bind[i] = '192.168.0.' + (i + NGINX.OFFSET);
	}
}

mod.nginx = {
	proxy: function (idx) {
		return nginx.proxy[idx];
	},
	redirect: function (idx) {
		return nginx.redirect[idx];
	},
	bind: function (idx) {
		return nginx.bind[idx];
	}
};
