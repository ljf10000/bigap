#!/bin/js

var mod = this,
    intf = require('product/helper/interface');

const name = 'fw/helper/ipt',
	MASK = {
		wan: 15
	},
	deft = {
		val: 0,
		mask: 0
	},
	cache = {
		val: 0,
		mask: 0
	},
	marks = {},
	PICK = {
		ALL: 0,
		MASK: 1,
		YES: 2,
		NO: 3
	};

mod.$name = function() { return name; };
mod.$debugger = new $Debugger(name);

(function () {
	var idx;

	for (idx=0; idx<intf.WAN.COUNT; idx++) {
		marks['wan'+idx] = {
			val: 1+idx,
			mask: MASK.wan
		};
	}
}());

function entry (name) {
	return marks[name] || deft;
}

function merge (obj) {
	return obj.val + '/' + obj.mask;
}

function pick (name, what) {
	var entry = entry(name);

	switch (what) {
		case PICK.MASK:
			return entry(name).mask;
		case PICK.YES:
			return entry(name).val;
		case PICK.NO:
			return 0;
		case PICK.ALL:
		default:
			return merge(obj);
	}
}

function genby (clean, argv) {
	var idx, count = argv.length;
	var name, what;

	if (0==count || 0!=(count%2)) {
		return deft.val + '/' + deft.mask;
	}

	if (clean) {
		cache.val = 0;
		cache.mask = 0;
	}

	for (idx=0; idx<count; idx+=2) {
		name = argv[idx];
		what = argv[idx+1];

		cache.val  |= pick(name, what);
		cache.mask |= pick(name, PICK.MASK);
	}

	return merge(cache);
}

mod.PICK = PICK;

mod.marks = {
	pick: pick,
	gen: function () {
		var argv = Array.prototype.slice.call(arguments);

		return genby(true, argv);
	},
	append: function () {
		var argv = Array.prototype.slice.call(arguments);

		return genby(false, argv);
	}
};

function rule_combin (obj, action, separator) {
	var count = arguments.length;
	var i;

	rule_append(obj, action, arguments[2]);

	for (i=3; i<count; i++) {
		if (arguments[i]) {
			obj.rule += separator + arguments[i];
		}
	}

	return obj;
}

function rule_append (obj) {
	var count = arguments.length;
	var i;

	for (i=1; i<count; i++) {
		obj.rule += ' ' + arguments[i];
	}

	return obj;
}

function rule_mark_mask (obj, action, a, b) {
	return rule_combin(obj, action, '/', a, b);
}

function rule_from_to (obj, action, from, to) {
	return rule_combin(obj, action, '-', from, to);
}

function rule_match_sets (obj, action, sets, dira, dirb) {
	return rule_combin(obj, action + ' ' + sets, '-', dira, dirb);
}

function chain_action (obj, action) {
	return rule_append(obj, action, obj.chain);
}

function tables (obj, table) {
	obj.table = table;
	obj.rule = 'iptables -t ' + table;
	obj.chain = 'filter';

	return obj;
}

function chains (obj, chain) {
	obj.chain = chain;

	return obj;
}

function jump (obj, target) {
	return rule_append(obj, '-j', target);
}

var pt = {
	mangle: function () {
		return tables(this, 'mangle');
	},
	nat: function () {
		return tables(this, 'nat');
	},
	filter: function () {
		return tables(this, 'filter');
	},
	table: function (table) {
		return tables(this, table);
	},
	prerouting: function () {
		return chains(this, 'PREROUTING');
	},
	postrouting: function () {
		return chains(this, 'POSTROUTING');
	},
	forward: function () {
		return chains(this, 'FORWARD');
	},
	chain: function (chain) {
		return chains(this, chain);
	},
	create: function () {
		return chain_action(this, 'N');
	},
	append: function () {
		return chain_action(this, 'A');
	},
	flush: function () {
		return chain_action(this, 'F');
	},
	zero: function () {
		return chain_action(this, 'Z');
	},
	remove: function () {
		return chain_action(this, 'X');
	},
	policy: function (target) {
		chain_action(this, 'P');

		return rule_append(this, target);
	},
	not: function () {
		return rule_append(this, '!');
	},
	comment: function (comments) {
		return rule_append(this, '--comment', comments);
	},
	jump: function (target) {
		return jump(this, target);
	},
	pktype: function (type) {
		// unicast|broadcast|multicast
		return rule_append(this, '--pkt-type', type);
	},
	iif: function (ifname) {
		return rule_append(this, '-i', ifname);
	},
	oif: function (ifname) {
		return rule_append(this, '-o', ifname);
	},
	iphy: function (ifname) {
		return rule_append(this, '--physdev-in', ifname);
	},
	ophy: function (ifname) {
		return rule_append(this, '--physdev-out', ifname);
	},
	isiphy: function () {
		return rule_append(this, '--physdev-is-in');
	},
	isophy: function () {
		return rule_append(this, '--physdev-is-out');
	},
	isbphy: function () {
		return rule_append(this, '--physdev-is-bridged');
	},
	protocol: function (protocol) {
		return rule_append(this, '-p', protocol);
	},
	icmp: function (type, code) {
		return rule_mark_mask(this, '--icmp-type', type, code);
	},
	icmp6: function (type, code) {
		return rule_mark_mask(this, '--icmp6-type', type, code);
	},
	mac: function (mac) {
		return rule_append(this, '--mac-source', mac);
	},
	src_range: function (from, to) {
		return rule_from_to(this, '--src-range', from, to);
	},
	dst_range: function (from, to) {
		return rule_from_to(this, '--dst-range', from, to);
	},
	sport: function (port) {
		return rule_append(this, '--sport', port);
	},
	dport: function (port) {
		return rule_append(this, '--dport', port);
	},
	to_port: function (port) {
		return rule_append(this, '--to-port', port);
	},
	saddr: function (port) {
		return rule_append(this, '-s', addr);
	},
	daddr: function (addr) {
		return rule_append(this, '-d', addr);
	},
	ipv4: function() {
		return rule_append(this, '-4');
	},
	ipv6: function() {
		return rule_append(this, '-6');
	},
	mark: function(mark, mask) {
		return rule_mark_mask(this, '--mark', mark, mask);
	},
	mask: function (mask) {
		return rule_append(this, '--mask', mask);
	},
	nfmask: function (nfmask) {
		return rule_append(this, '--nfmask', nfmask);
	},
	ctmask: function (ctmask) {
		return rule_append(this, '--ctmask', ctmask);
	},
	set_xmark: function (mark, mask) {
		return rule_mark_mask(this, '--set-xmark', mark, mask);
	},
	save_mark: function () {
		return rule_append(this, '--save-mark');
	},
	restore_mark: function () {
		return rule_append(this, '--restore-mark');
	},
	and_mark: function (mark) {
		return rule_append(this, '--and-mark');
	},
	or_mark: function (mark) {
		return rule_append(this, '--or-mark');
	},
	xor_mark: function (mark) {
		return rule_append(this, '--xor-mark');
	},
	set_mark: function (mark, mask) {
		return rule_mark_mask(this, '--xor-xmark', mark, mask);
	},
	goto: function (chain) {
		return rule_append(this, '-g', chain);
	},
	uid_owner: function (from, to) {
		return rule_from_to(this, '--uid-owner', from, to);
	},
	gid_owner: function (from, to) {
		return rule_from_to(this, '--gid-owner', from, to);
	},
	match: function (match) {
		return rule_append(this, '-m', match);
	},
	match_set: function (sets, src, dst) {
		return rule_match_sets(this, '--match-set', sets, src, dst);
	},
	match_state: function (state) {
		var count = arguments.length;
		var i;

		rule_append(this, '--state', state);

		for (i=1; i<count; i++) {
			this.rule += ',' + arguments[i];
		}

		return this;
	},
	match_u32: function (offset, mask, value) {
		this.rule += ' "' + offset + '&' + mask + '=' + value + '"';

		return this;
	},
	match_mode: function (mode) {
		return rule_append(this, '--mode', mode);
	},
	match_every: function (number) {
		return rule_append(this, '--every', number);
	},
	match_packet: function (number) {
		return rule_append(this, '--packet', number);
	},
	commit: function () {
		__my__.shell(this.rule);
	}
};

mod.object = function(type) {
	return Object.setPrototypeOf({}, pt);
};
