#!/bin/js

var mod  = this,
    intf = require('product/helper/interface'),
   	rt   = require('fw/helper/rt'),
   	mwan = require('config/mwan');

const name = 'um/helper/rt';

function table_create() {
	var i, table, number;

	for (i=0; i<intf.WAN.COUNT; i++) {
		table = intf.ifname('wan', i);
		number= i + intf.WAN.RTTABLE.NUMBER;

		rt.table_create(table, number);
	}
}

function route_add() {
	var i, table, ifname;

	for (i=0; i<intf.WAN.COUNT; i++) {
		table = ifname = intf.ifname('wan', i);

		rt.route_add(table, ifname);
	}
}

function setup() {
	route_add();
}

function proto_change() {
	if (mwan.is_lb()) {
		setup();
	}
}

function prepare() {
	table_create();
	proto_change();
}

mod.setup = setup;
mod.prepare = prepare;
mod.proto_change = proto_change;

