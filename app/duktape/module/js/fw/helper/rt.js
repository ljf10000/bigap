#!/bin/js

var mod = this;

const name = 'fw/helper/rt',
	rt_tables = '/etc/iproute2/rt_tables';

function table_create (table, number) {
	var rule = number + ' ' + table;

	__my__.appendfile(rt_tables, rule);
}

function route_add (table, ifname) {
	var cmd = 'ip route add default dev ' + ifname + ' table ' + table;

	__my__.shell(cmd);
}

mod.table_create    = table_create;
mod.route_add       = route_add;
