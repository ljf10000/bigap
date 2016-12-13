/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
int
intf_ioctl(int cmd, char *action, char *name, void *user)
{
    int fd = INVALID_FD, err = 0;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (false==is_good_fd(fd)) {
        err = -errno;
        
        debug_error("intf %s socket error:%d", name, err);
        
        goto error;
    }

    err = ioctl(fd, cmd, user);
    if (err<0) {
        err = -errno;
        
        debug_error("intf %s ioctl %d:%s error:%d", name, cmd, action, err);
        
        goto error;
    }
    
error:
    os_close(fd);

    return err;
}

int
intf_get_flag(char *ifname, int *flag)
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);

    err = intf_ioctl(SIOCGIFFLAGS, "get-flag", ifr.ifr_name, &ifr);
    if (err<0) {
        return err;
    }
    
    *flag = ifr.ifr_flags;
    
    return 0;
}

int
intf_set_flag(char *ifname, int flag)
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);
    ifr.ifr_flags = flag;

    return intf_ioctl(SIOCSIFFLAGS, "set-flag", ifr.ifr_name, &ifr);
}

int
intf_get_link_updown(char *ifname)
{
    int flag = 0;
    int err = intf_get_flag(ifname, &flag);
    if (err<0) {
        return err;
    }

    return (IFF_UP & flag)?1:0;
}

int
intf_get_admin_updown(char *ifname)
{
    int flag = 0, err;

    err = intf_get_flag(ifname, &flag);
    if (err<0) {
        return err;
    }

    return (IFF_RUNNING & flag)?1:0;
}

int
intf_set_admin_updown(char *ifname, bool up)
{
    int flag = 0, err;

    err = intf_get_flag(ifname, &flag);
    if (err<0) {
        return err;
    }

    if (up) {
        flag |= IFF_UP;
    } else {
        flag &= ~IFF_UP;
    }
    
    return intf_set_flag(ifname, flag);
}

int
intf_get_promisc(char *ifname)
{
    int flag = 0, err;

    err = intf_get_flag(ifname, &flag);
    if (err<0) {
        return err;
    }
    
    return (IFF_PROMISC & flag)?1:0;
}

int
intf_set_promisc(char *ifname)
{
    int flag = 0, err;

    err = intf_get_flag(ifname, &flag);
    if (err<0) {
        return err;
    }
    
    return intf_set_flag(ifname, flag | IFF_PROMISC);
}

int
intf_get_mtu(char *ifname, int *mtu)
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);

    err = intf_ioctl(SIOCGIFMTU, "get-mtu", ifr.ifr_name, &ifr);
    if (err<0) {
        return err;
    }
    
    *mtu = ifr.ifr_mtu;

    return 0;
}

int
intf_set_mtu(char *ifname, int mtu)
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);
    ifr.ifr_mtu = mtu;
    
    return intf_ioctl(SIOCSIFMTU, "set-mtu", ifr.ifr_name, &ifr);
}

int
intf_get_mac(char *ifname, byte mac[])
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);

    err = intf_ioctl(SIOCGIFHWADDR, "get-mac", ifr.ifr_name, &ifr);
    if (err<0) {
        return err;
    }

    os_maccpy(mac, (byte *)ifr.ifr_hwaddr.sa_data);

    return 0;
}

int
intf_set_mac(char *ifname, byte mac[])
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);
    os_maccpy((byte *)ifr.ifr_hwaddr.sa_data, mac);

    return intf_ioctl(SIOCSIFHWADDR, "set-mac", ifr.ifr_name, &ifr);
}

int
intf_get_ip(char *ifname, uint32 *ip)
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);

    err = intf_ioctl(SIOCGIFADDR, "get-ip", ifr.ifr_name, &ifr);
    if (err<0) {
        return err;
    }

    *ip = ((sockaddr_in_t *)&ifr.ifr_addr)->sin_addr.s_addr;
    
    return 0;
}

int
intf_get_netmask(char *ifname, uint32 *mask)
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);

    err = intf_ioctl(SIOCGIFNETMASK, "get-netmask", ifr.ifr_name, &ifr);
    if (err<0) {
        return err;
    }

    *mask = ((sockaddr_in_t *)&ifr.ifr_addr)->sin_addr.s_addr;
    
    return 0;
}

int
intf_foreach(mv_t (*foreach)(char *ifname))
{
    struct ifconf conf;
    struct ifreq *ifr;
    char buff[BUFSIZ];
    int i, err, count;
    mv_u mv;
    
    conf.ifc_buf = buff;
    conf.ifc_len = sizeof(buff);

    err = intf_ioctl(SIOCGIFCONF, "get-conf", "global", &conf);
    if (err<0) {
        return err;
    }

    count = conf.ifc_len/sizeof(struct ifreq);
    
    for (i=0, ifr=conf.ifc_req; 
        i<count; 
        i++, ifr++) {
        if (foreach) {
            mv.v = (*foreach)(ifr->ifr_name);
            if (is_mv2_break(mv.v)) {
                return mv2_break(mv.v);
            }
        }
    }
    
    return 0;
}

int
intf_foreachEx(bool skip_loopback, mv_t (*foreach)(char *ifname))
{
    int flag;
    
    mv_t cb(char *name)
    {
        int ret = intf_get_flag(name, &flag);
        if (ret<0) {
            return mv2_go(ret);
        }
        else if (os_hasflag(flag, IFF_LOOPBACK)) {
            return mv2_ok;
        }
        else if (foreach) {
            return (*foreach)(name);
        }
        else {
            return mv2_ok;
        }
    }

    if (skip_loopback) {
        return intf_foreach(cb);
    } else {
        return intf_foreach(foreach);
    }
}
/******************************************************************************/
