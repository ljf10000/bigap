#ifndef __IF_H_d2aee259b0af4f9f935ac914816d9ec9__
#define __IF_H_d2aee259b0af4f9f935ac914816d9ec9__
#ifdef __APP__
/******************************************************************************/
#define OS_IFREQ_ZERO { .ifr_ifrn = { .ifrn_name = __empty }}

extern int
__intf_ioctl(int cmd, char *action, struct ifreq *ifr);

extern int
intf_get_flag(char *ifname, int *flag);

extern int
intf_set_flag(char *ifname, int flag);

/*
* <0: error
*  0: link down
* >0: link up
*/
extern int
intf_get_link_updown(char *ifname);

/*
* <0: error
*  0: link down
* >0: link up
*/
extern int
intf_get_admin_updown(char *ifname);

extern int
intf_set_admin_updown(char *ifname, bool up);

/*
* <0: error
*  0: not promisc
* >0: is  promisc
*/
extern int
intf_get_promisc(char *ifname);

extern int
intf_set_promisc(char *ifname);

extern int
intf_get_mtu(char *ifname, int *mtu);

extern int
intf_set_mtu(char *ifname, int mtu);

extern int
intf_get_mac(char *ifname, byte mac[]);

extern int
intf_set_mac(char *ifname, byte mac[]);

extern int
intf_get_ip(char *ifname, uint32 *ip);

extern int
intf_get_netmask(char *ifname, uint32 *mask);

extern int
intf_foreach(mv_t (*foreach)(char *ifname));

extern int
intf_foreachEx(bool skip_loopback, mv_t (*foreach)(char *ifname));
/******************************************************************************/
#endif
#endif /* __IF_H_d2aee259b0af4f9f935ac914816d9ec9__ */
