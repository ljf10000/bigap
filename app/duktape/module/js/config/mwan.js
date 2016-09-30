#!/bin/js

/*
* module: stream
*/
var mod = this;

const name = 'config/mwan',
	deft = {
		mode: 'lb',
		lb: 'user'
	},
	conf = (function () {
		var json;

		try {
			json = __my__.readtxt('/tmp/config/mwan.json');

			return JSON.parse(json);
		} catch(e) {
			return deft;
		}
	}());

mod.mode = function() {
	return conf.mode;
};

mod.lb = function() {
	return conf.lb;
};

mod.is_lb = function() {
	return 'lb'===conf.mode;
};

mod.is_ms = function() {
	return 'ms'===conf.mode;
};

mod.is_lb_user = function () {
	return mod.is_lb() && 'user'===conf.lb;
};

mod.is_lb_ct = function () {
	return mod.is_lb() && 'ct'===conf.lb;
};
/* eof */
