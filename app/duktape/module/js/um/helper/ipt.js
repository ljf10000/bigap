#!/bin/js

var mod  = this,
    intf = require('product/helper/interface'),
    protocol = require('product/helper/protocol'),
   	ipt  = require('fw/helper/ipt'),
   	mwan = require('config/mwan');

const name = 'um/helper/ipt',
	SIP_OFFSET = 12;

function mangle () {
	var i, rule = ipt.object(), wan, mark, mask;

	rule.mangle().prerouting().append()
		.match('state').match_state('ESTABLISHED', 'RELATED')
		.jump('CONNMARK').restore_mark()
		.commit();

	for (i=0; i<intf.WAN.COUNT; i++) {
		wan = 'wan' + i;

		rule.mangle().chain(wan).create().commit();

		mark = ipt.marks.pick(wan, ipt.PICK.YES);
		rule.mangle().chain(wan).append()
			.jump('MARK').or_mark(mark)
			.commit();

		mask = ipt.marks.pick(wan, ipt.PICK.MASK);
		rule.mangle().chain(wan).append()
			.jump('CONNMARK').save_mark().mask(mask)
			.commit();

		rule.mangle().prerouting().append()
			.iif('lan0')
			.match('state').match_state('NEW');
		if (mwan.is_lb_user()) {
			rule.match('u32')
				.match_u32(SIP_OFFSET, 1, i);
		} else {
			rule.match('statistic')
				.match_mode('nth')
				.match_every(intf.WAN.COUNT)
				.match_packet(i)
		}
		rule.commit();
	}
}

function mangle_clean () {
	var i, rule, wan;

	if (mwan.is_lb()) {
		rule = ipt.object();

		rule.mangle().prerouting().flush().commit();
		for (i=0; i<intf.WAN.COUNT; i++) {
			wan = 'wan' + i;

			rule.mangle().chain(wan).flush().commit();
		}
	}
}

function nat_mwan_select(rule, wan) {
	var mm;

	if (mwan.is_lb()) {
		mm = ipt.marks.pick(wan, ipt.PICK.ALL);

		rule.match('mark').mark(mm);
	}

	return rule;
}

function nat() {
	var i, rule = ipt.object(), wan, proxy, redirect;

	rule.nat().chain('nginx').create().commit();

	for (i=0; i<intf.WAN.COUNT; i++) {
		wan = 'wan' + i;

		rule.nat().chain('nginx').append()
			.protocol('tcp')
			.match('set').match_set('white', 'src');
		nat_mwan_select(rule, wan)
			.jump('REDIRECT').to_port(protocol.nginx.proxy(i))
			.commit();

		rule.nat().chain('nginx').append()
			.protocol('tcp')
			.match('set').not().match_set('white', 'src');
		nat_mwan_select(rule, wan)
			.jump('REDIRECT').to_port(protocol.nginx.redirect(i))
			.commit();
	}

	rule.nat().prerouting().append()
		.iif('lan0')
		.protocol('tcp').dport(80)
		.match('state').match_state('NEW')
		.match('set').not().match_set('host', 'dst')
		.jump('nginx')
		.commit();

	for (i=0; i<intf.WAN.COUNT; i++) {
		wan = 'wan' + i;

		rule.nat().postrouting().append()
			.oif(wan)
			.jump(MASQUERADE)
			.commit();
	}
}

function nat_clean () {
	var rule = ipt.object();

	rule.nat().prerouting().flush().commit();
	rule.nat().postrouting().flush().commit();
	rule.nat().chain('nginx').flush().commit();
}

function filter () {
	var rule = ipt.object();

	rule.filter().forward().flush().commit();
	rule.filter().forward().policy('DROP').commit();
	rule.filter().forward().append()
		.protocol('udp').dport(53)
		.match('state').match_state('NEW')
		.jump('ACCEPT')
		.commit();
	rule.filter().forward().append()
		.iif('lan0')
		.match('set').match_set('white', 'src')
		.jump('ACCEPT')
		.commit();
	rule.filter().forward().append()
		.oif('lan0')
		.match('set').match_set('white', 'dst')
		.jump('ACCEPT')
		.commit();
}

function filter_clean () {
	var rule = ipt.object();

	rule.filter().forward().flush().commit();
	rule.filter().forward().policy('ACCEPT').commit();
}

function prepare () {
	mangle();
	nat();
	filter();
}

function clean () {
	filter_clean();
	nat_clean();
	mangle_clean();
}

mod.prepare = prepare;
mod.clean = clean;
