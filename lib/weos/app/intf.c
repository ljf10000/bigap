/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
int
__intf_ioctl(int cmd, char *action, struct ifreq *ifr)
{
    int fd = INVALID_FD, err = 0;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (false==is_good_fd(fd)) {
        err = -errno;
        
        debug_error("intf %s socket error:%d", ifr->ifr_name, err);
        
        goto error;
    }

    err = ioctl(fd, cmd, ifr);
    if (err<0) {
        err = -errno;
        
        debug_error("intf %s ioctl %d:%s error:%d", ifr->ifr_name, cmd, action, err);
        
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

    err = __intf_ioctl(SIOCGIFFLAGS, "get-flag", &ifr);
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

    return __intf_ioctl(SIOCSIFFLAGS, "set-flag", &ifr);
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

    err = __intf_ioctl(SIOCGIFMTU, "get-mtu", &ifr);
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
    
    return __intf_ioctl(SIOCSIFMTU, "set-mtu", &ifr);
}

int
intf_get_mac(char *ifname, byte mac[])
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);

    err = __intf_ioctl(SIOCGIFHWADDR, "get-mac", &ifr);
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

    return __intf_ioctl(SIOCSIFHWADDR, "set-mac", &ifr);
}

int
intf_get_ip(char *ifname, uint32 *ip)
{
    struct ifreq ifr = OS_IFREQ_ZERO;
    int err;

    os_strcpy(ifr.ifr_name, ifname);

    err = __intf_ioctl(SIOCGIFADDR, "get-ip", &ifr);
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

    err = __intf_ioctl(SIOCGIFNETMASK, "get-netmask", &ifr);
    if (err<0) {
        return err;
    }

    *mask = ((sockaddr_in_t *)&ifr.ifr_addr)->sin_addr.s_addr;
    
    return 0;
}
/******************************************************************************/
