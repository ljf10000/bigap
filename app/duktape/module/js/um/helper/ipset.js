#!/bin/js

var mod = this,
    intf = require('product/helper/interface'),
	ipset = require('fw/helper/ipset');

const name = 'um/helper/ipset',
	TABLES = ['host', 'white'],
	TYPE = 'bitmap:ip',
	RANGE = 0;

(function () {
	var count = TABLES.length;
	var i;

	for (i=0; i<count; i++) {
		TABLES[count + i] = intf.wanname(i);
	}
}());

