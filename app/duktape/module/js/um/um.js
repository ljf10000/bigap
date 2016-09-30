#!/bin/js

var mod  = this,
    rt   = require('um/helper/rt'),
	ipt  = require('um/helper/ipt');

const name = 'um/um';

mod.prepare = function() {
	rt.prepare();
	ipt.prepare();
};

mod.clean = function() {
	ipt.clean();
};

mod.init = function() {

};

mod.fini = function() {

};

mod.bind = function(user) {

};

mod.unbind = function(user) {

};

mod.auth = function(user) {

};

mod.deauth = function(user) {

};

mod.proto_change = function() {

};

/* eof */
