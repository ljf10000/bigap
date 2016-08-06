/* vi: set sw=4 ts=4: */
/*
 * applets.h - a listing of all busybox applets.
 *
 * If you write a new applet, you need to add an entry to this list to make
 * busybox aware of it.
 *
 * It is CRUCIAL that this listing be kept in ascii order, otherwise the binary
 * search lookup contributed by Gaute B Strokkenes stops working. If you value
 * your kneecaps, you'll be sure to *make sure* that any changes made to this
 * file result in the listing remaining in ascii order. You have been warned.
 */

/*
name  - applet name as it is typed on command line
name2 - applet name, converted to C (ether-wake: name2 = ether_wake)
main  - corresponding <applet>_main to call (bzcat: main = bunzip2)
l     - location to install link to: [/usr]/[s]bin
s     - suid type:
        _BB_SUID_REQUIRE: will complain if busybox isn't suid
        and is run by non-root (applet_main() will not be called at all)
        _BB_SUID_DROP: will drop suid prior to applet_main()
        _BB_SUID_MAYBE: neither of the above
*/


#if defined(PROTOTYPES)
# define APPLET(name,l,s)                    int name##_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
# define APPLET_ODDNAME(name,main,l,s,name2) int main##_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
# define APPLET_NOEXEC(name,main,l,s,name2)  int main##_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
# define APPLET_NOFORK(name,main,l,s,name2)  int main##_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;

#elif defined(NAME_MAIN_CNAME)
# define APPLET(name,l,s)                    name name##_main name
# define APPLET_ODDNAME(name,main,l,s,name2) name main##_main name2
# define APPLET_NOEXEC(name,main,l,s,name2)  name main##_main name2
# define APPLET_NOFORK(name,main,l,s,name2)  name main##_main name2

#elif defined(MAKE_USAGE) && ENABLE_FEATURE_VERBOSE_USAGE
# define APPLET(name,l,s)                    name##_trivial_usage name##_full_usage "\0"
# define APPLET_ODDNAME(name,main,l,s,name2) name2##_trivial_usage name2##_full_usage "\0"
# define APPLET_NOEXEC(name,main,l,s,name2)  name2##_trivial_usage name2##_full_usage "\0"
# define APPLET_NOFORK(name,main,l,s,name2)  name2##_trivial_usage name2##_full_usage "\0"

#elif defined(MAKE_USAGE) && !ENABLE_FEATURE_VERBOSE_USAGE
# define APPLET(name,l,s)                    name##_trivial_usage "\0"
# define APPLET_ODDNAME(name,main,l,s,name2) name2##_trivial_usage "\0"
# define APPLET_NOEXEC(name,main,l,s,name2)  name2##_trivial_usage "\0"
# define APPLET_NOFORK(name,main,l,s,name2)  name2##_trivial_usage "\0"

#elif defined(MAKE_LINKS)
# define APPLET(name,l,c)                    LINK l name
# define APPLET_ODDNAME(name,main,l,s,name2) LINK l name
# define APPLET_NOEXEC(name,main,l,s,name2)  LINK l name
# define APPLET_NOFORK(name,main,l,s,name2)  LINK l name

#else
  static struct bb_applet applets[] = { /*    name, main, location, need_suid */
# define APPLET(name,l,s)                    { #name, #name, l, s },
# define APPLET_ODDNAME(name,main,l,s,name2) { #name, #main, l, s },
# define APPLET_NOEXEC(name,main,l,s,name2)  { #name, #main, l, s, 1 },
# define APPLET_NOFORK(name,main,l,s,name2)  { #name, #main, l, s, 1, 1 },
#endif

#if ENABLE_INSTALL_NO_USR
# define _BB_DIR_USR_BIN _BB_DIR_BIN
# define _BB_DIR_USR_SBIN _BB_DIR_SBIN
#endif

IF_TEST(APPLET_NOFORK([,  test, _BB_DIR_USR_BIN, _BB_SUID_DROP, test))
IF_TEST(APPLET_NOFORK([[, test, _BB_DIR_USR_BIN, _BB_SUID_DROP, test))
IF_ACPID(APPLET(acpid, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_ADDGROUP(APPLET(addgroup, _BB_DIR_BIN, _BB_SUID_DROP))
IF_ADDUSER(APPLET(adduser, _BB_DIR_BIN, _BB_SUID_DROP))
IF_ADJTIMEX(APPLET(adjtimex, _BB_DIR_SBIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_AK(APPLET(ak, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_AR(APPLET(ar, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_ARP(APPLET(arp, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_ARPING(APPLET(arping, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_ASH(APPLET(ash, _BB_DIR_BIN, _BB_SUID_DROP))
IF_AWK(APPLET_NOEXEC(awk, awk, _BB_DIR_USR_BIN, _BB_SUID_DROP, awk))
IF_BASENAME(APPLET_NOFORK(basename, basename, _BB_DIR_USR_BIN, _BB_SUID_DROP, basename))
IF_BBCONFIG(APPLET(bbconfig, _BB_DIR_BIN, _BB_SUID_DROP))
//IF_BBSH(APPLET(bbsh, _BB_DIR_BIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_BCK(APPLET(bck, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_BEEP(APPLET(beep, _BB_DIR_USR_BIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_BENV(APPLET(benv, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_BLKID(APPLET(blkid, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_BRCTL(APPLET(brctl, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_BUNZIP2(APPLET(bunzip2, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_BUNZIP2(APPLET_ODDNAME(bzcat, bunzip2, _BB_DIR_USR_BIN, _BB_SUID_DROP, bzcat))
IF_BZIP2(APPLET(bzip2, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CAL(APPLET(cal, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CAT(APPLET_NOFORK(cat, cat, _BB_DIR_BIN, _BB_SUID_DROP, cat))
IF_CATV(APPLET(catv, _BB_DIR_BIN, _BB_SUID_DROP))
IF_CHAT(APPLET(chat, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CHATTR(APPLET(chattr, _BB_DIR_BIN, _BB_SUID_DROP))
IF_CHCON(APPLET(chcon, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CHGRP(APPLET_NOEXEC(chgrp, chgrp, _BB_DIR_BIN, _BB_SUID_DROP, chgrp))
IF_CHMOD(APPLET_NOEXEC(chmod, chmod, _BB_DIR_BIN, _BB_SUID_DROP, chmod))
IF_CHOWN(APPLET_NOEXEC(chown, chown, _BB_DIR_BIN, _BB_SUID_DROP, chown))
IF_CHPASSWD(APPLET(chpasswd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_CHPST(APPLET(chpst, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CHROOT(APPLET(chroot, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_CHRT(APPLET(chrt, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CHVT(APPLET(chvt, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CKSUM(APPLET(cksum, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CLEAR(APPLET(clear, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CMP(APPLET(cmp, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_COMM(APPLET(comm, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CP(APPLET_NOEXEC(cp, cp, _BB_DIR_BIN, _BB_SUID_DROP, cp))
IF_CPIO(APPLET(cpio, _BB_DIR_BIN, _BB_SUID_DROP))
IF_CROND(APPLET(crond, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_CRONTAB(APPLET(crontab, _BB_DIR_USR_BIN, _BB_SUID_REQUIRE))
IF_CRYPTPW(APPLET(cryptpw, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_CTTYHACK(APPLET(cttyhack, _BB_DIR_BIN, _BB_SUID_DROP))
IF_CUT(APPLET_NOEXEC(cut, cut, _BB_DIR_USR_BIN, _BB_SUID_DROP, cut))
#if 0 /* BIGAP, not setup softlink date */
IF_DATE(APPLET(date, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_DC(APPLET(dc, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_DD(APPLET_NOEXEC(dd, dd, _BB_DIR_BIN, _BB_SUID_DROP, dd))
IF_DEALLOCVT(APPLET(deallocvt, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_DELGROUP(APPLET_ODDNAME(delgroup, deluser, _BB_DIR_BIN, _BB_SUID_DROP, delgroup))
IF_DELUSER(APPLET(deluser, _BB_DIR_BIN, _BB_SUID_DROP))
IF_DEPMOD(APPLET(depmod, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MODPROBE_SMALL(APPLET_ODDNAME(depmod, modprobe, _BB_DIR_SBIN, _BB_SUID_DROP, modprobe))
IF_DEVFSD(APPLET(devfsd, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_DEVMEM(APPLET(devmem, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_DF(APPLET(df, _BB_DIR_BIN, _BB_SUID_DROP))
IF_DHCPRELAY(APPLET(dhcprelay, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_DIFF(APPLET(diff, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_DIRNAME(APPLET_NOFORK(dirname, dirname, _BB_DIR_USR_BIN, _BB_SUID_DROP, dirname))
IF_DMESG(APPLET(dmesg, _BB_DIR_BIN, _BB_SUID_DROP))
IF_DNSD(APPLET(dnsd, _BB_DIR_USR_SBIN, _BB_SUID_REQUIRE))
IF_HOSTNAME(APPLET_ODDNAME(dnsdomainname, hostname, _BB_DIR_BIN, _BB_SUID_DROP, dnsdomainname))
IF_DOS2UNIX(APPLET(dos2unix, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_DPKG(APPLET(dpkg, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_DPKG_DEB(APPLET_ODDNAME(dpkg-deb, dpkg_deb, _BB_DIR_USR_BIN, _BB_SUID_DROP, dpkg_deb))
#if 0 /* BIGAP, not setup softlink date */
IF_DU(APPLET(du, _BB_DIR_USR_BIN, _BB_SUID_DROP))
#endif
IF_DUMPKMAP(APPLET(dumpkmap, _BB_DIR_BIN, _BB_SUID_DROP))
IF_DUMPLEASES(APPLET(dumpleases, _BB_DIR_USR_BIN, _BB_SUID_DROP))
//IF_E2FSCK(APPLET(e2fsck, _BB_DIR_SBIN, _BB_SUID_DROP))
//IF_E2LABEL(APPLET_ODDNAME(e2label, tune2fs, _BB_DIR_SBIN, _BB_SUID_DROP, e2label))
IF_ECHO(APPLET_NOFORK(echo, echo, _BB_DIR_BIN, _BB_SUID_DROP, echo))
IF_ED(APPLET(ed, _BB_DIR_BIN, _BB_SUID_DROP))
IF_FEATURE_GREP_EGREP_ALIAS(APPLET_ODDNAME(egrep, grep, _BB_DIR_BIN, _BB_SUID_DROP, egrep))
IF_EJECT(APPLET(eject, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_ENV(APPLET_NOEXEC(env, env, _BB_DIR_USR_BIN, _BB_SUID_DROP, env))
IF_ENVDIR(APPLET_ODDNAME(envdir, chpst, _BB_DIR_USR_BIN, _BB_SUID_DROP, envdir))
IF_ENVUIDGID(APPLET_ODDNAME(envuidgid, chpst, _BB_DIR_USR_BIN, _BB_SUID_DROP, envuidgid))
IF_ETHER_WAKE(APPLET_ODDNAME(ether-wake, ether_wake, _BB_DIR_USR_BIN, _BB_SUID_DROP, ether_wake))
IF_EXPAND(APPLET(expand, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_EXPR(APPLET(expr, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_FAKEIDENTD(APPLET(fakeidentd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_FALSE(APPLET_NOFORK(false, false, _BB_DIR_BIN, _BB_SUID_DROP, false))
IF_FBSET(APPLET(fbset, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_FBSPLASH(APPLET(fbsplash, _BB_DIR_SBIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_FCOOKIE(APPLET(fcookie, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_FDFLUSH(APPLET_ODDNAME(fdflush, freeramdisk, _BB_DIR_BIN, _BB_SUID_DROP, fdflush))
IF_FDFORMAT(APPLET(fdformat, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_FDISK(APPLET(fdisk, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_FEATURE_GREP_FGREP_ALIAS(APPLET_ODDNAME(fgrep, grep, _BB_DIR_BIN, _BB_SUID_DROP, fgrep))
IF_FIND(APPLET_NOEXEC(find, find, _BB_DIR_USR_BIN, _BB_SUID_DROP, find))
IF_FINDFS(APPLET(findfs, _BB_DIR_SBIN, _BB_SUID_MAYBE))
IF_FLASH_ERASEALL(APPLET(flash_eraseall, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_FLASH_LOCK(APPLET_ODDNAME(flash_lock, flash_lock_unlock, _BB_DIR_USR_SBIN, _BB_SUID_DROP, flash_lock))
IF_FLASH_UNLOCK(APPLET_ODDNAME(flash_unlock, flash_lock_unlock, _BB_DIR_USR_SBIN, _BB_SUID_DROP, flash_unlock))
IF_FLASHCP(APPLET(flashcp, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_FOLD(APPLET(fold, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_FREE(APPLET(free, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_FREERAMDISK(APPLET(freeramdisk, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_FSCK(APPLET(fsck, _BB_DIR_SBIN, _BB_SUID_DROP))
//IF_E2FSCK(APPLET_ODDNAME(fsck.ext2, e2fsck, _BB_DIR_SBIN, _BB_SUID_DROP, fsck_ext2))
//IF_E2FSCK(APPLET_ODDNAME(fsck.ext3, e2fsck, _BB_DIR_SBIN, _BB_SUID_DROP, fsck_ext3))
IF_FSCK_MINIX(APPLET_ODDNAME(fsck.minix, fsck_minix, _BB_DIR_SBIN, _BB_SUID_DROP, fsck_minix))
IF_FSYNC(APPLET_NOFORK(fsync, fsync, _BB_DIR_BIN, _BB_SUID_DROP, fsync))
IF_FTPD(APPLET(ftpd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_FTPGET(APPLET_ODDNAME(ftpget, ftpgetput, _BB_DIR_USR_BIN, _BB_SUID_DROP, ftpget))
IF_FTPPUT(APPLET_ODDNAME(ftpput, ftpgetput, _BB_DIR_USR_BIN, _BB_SUID_DROP, ftpput))
IF_FUSER(APPLET(fuser, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_GETENFORCE(APPLET(getenforce, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_GETOPT(APPLET(getopt, _BB_DIR_BIN, _BB_SUID_DROP))
IF_GETSEBOOL(APPLET(getsebool, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_GETTY(APPLET(getty, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_GREP(APPLET(grep, _BB_DIR_BIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_GUARD(APPLET(guard, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_GUNZIP(APPLET(gunzip, _BB_DIR_BIN, _BB_SUID_DROP))
IF_GZIP(APPLET(gzip, _BB_DIR_BIN, _BB_SUID_DROP))
IF_HALT(APPLET(halt, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_HD(APPLET_NOEXEC(hd, hexdump, _BB_DIR_USR_BIN, _BB_SUID_DROP, hd))
IF_HDPARM(APPLET(hdparm, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_HEAD(APPLET(head, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_HEXDUMP(APPLET_NOEXEC(hexdump, hexdump, _BB_DIR_USR_BIN, _BB_SUID_DROP, hexdump))
#if 1 /* BIGAP */
IF_HISI(APPLET(hisi, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_HOSTID(APPLET_NOFORK(hostid, hostid, _BB_DIR_USR_BIN, _BB_SUID_DROP, hostid))
IF_HOSTNAME(APPLET(hostname, _BB_DIR_BIN, _BB_SUID_DROP))
IF_HTTPD(APPLET(httpd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_HUSH(APPLET(hush, _BB_DIR_BIN, _BB_SUID_DROP))
IF_HWCLOCK(APPLET(hwclock, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_ID(APPLET(id, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_IFCONFIG(APPLET(ifconfig, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_IFUPDOWN(APPLET_ODDNAME(ifdown, ifupdown, _BB_DIR_SBIN, _BB_SUID_DROP, ifdown))
IF_IFENSLAVE(APPLET(ifenslave, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_IFPLUGD(APPLET(ifplugd, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_IFUPDOWN(APPLET_ODDNAME(ifup, ifupdown, _BB_DIR_SBIN, _BB_SUID_DROP, ifup))
IF_INETD(APPLET(inetd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_INIT(APPLET(init, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_INOTIFYD(APPLET(inotifyd, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_INSMOD(APPLET(insmod, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MODPROBE_SMALL(APPLET_ODDNAME(insmod, modprobe, _BB_DIR_SBIN, _BB_SUID_DROP, modprobe))
IF_INSTALL(APPLET(install, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_IONICE(APPLET(ionice, _BB_DIR_BIN, _BB_SUID_DROP))
#if 0 /* BIGAP, not setup softlink date */
#if ENABLE_FEATURE_IP_ADDRESS \
 || ENABLE_FEATURE_IP_ROUTE \
 || ENABLE_FEATURE_IP_LINK \
 || ENABLE_FEATURE_IP_TUNNEL \
 || ENABLE_FEATURE_IP_RULE
IF_IP(APPLET(ip, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
#endif
IF_IPADDR(APPLET(ipaddr, _BB_DIR_BIN, _BB_SUID_DROP))
IF_IPCALC(APPLET(ipcalc, _BB_DIR_BIN, _BB_SUID_DROP))
IF_IPCRM(APPLET(ipcrm, _BB_DIR_USR_BIN, _BB_SUID_REQUIRE))
IF_IPCS(APPLET(ipcs, _BB_DIR_USR_BIN, _BB_SUID_REQUIRE))
IF_IPLINK(APPLET(iplink, _BB_DIR_BIN, _BB_SUID_DROP))
IF_IPROUTE(APPLET(iproute, _BB_DIR_BIN, _BB_SUID_DROP))
IF_IPRULE(APPLET(iprule, _BB_DIR_BIN, _BB_SUID_DROP))
IF_IPTUNNEL(APPLET(iptunnel, _BB_DIR_BIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_JLOGD(APPLET(jlogd, _BB_DIR_BIN, _BB_SUID_DROP))
IF_JLOGGER(APPLET(jlogger, _BB_DIR_BIN, _BB_SUID_DROP))
IF_JS(APPLET(js, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_KBD_MODE(APPLET(kbd_mode, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_KILL(APPLET(kill, _BB_DIR_BIN, _BB_SUID_DROP))
IF_KILLALL(APPLET_ODDNAME(killall, kill, _BB_DIR_USR_BIN, _BB_SUID_DROP, killall))
IF_KILLALL5(APPLET_ODDNAME(killall5, kill, _BB_DIR_USR_BIN, _BB_SUID_DROP, killall5))
IF_KLOGD(APPLET(klogd, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_LASH(APPLET(lash, _BB_DIR_BIN, _BB_SUID_DROP))
IF_LAST(APPLET(last, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_LENGTH(APPLET_NOFORK(length, length, _BB_DIR_USR_BIN, _BB_SUID_DROP, length))
IF_LESS(APPLET(less, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_SETARCH(APPLET_ODDNAME(linux32, setarch, _BB_DIR_BIN, _BB_SUID_DROP, linux32))
IF_SETARCH(APPLET_ODDNAME(linux64, setarch, _BB_DIR_BIN, _BB_SUID_DROP, linux64))
IF_FEATURE_INITRD(APPLET_ODDNAME(linuxrc, init, _BB_DIR_ROOT, _BB_SUID_DROP, linuxrc))
IF_LN(APPLET_NOEXEC(ln, ln, _BB_DIR_BIN, _BB_SUID_DROP, ln))
IF_LOAD_POLICY(APPLET(load_policy, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_LOADFONT(APPLET(loadfont, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_LOADKMAP(APPLET(loadkmap, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_LOGGER(APPLET(logger, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_LOGIN(APPLET(login, _BB_DIR_BIN, _BB_SUID_REQUIRE))
IF_LOGNAME(APPLET_NOFORK(logname, logname, _BB_DIR_USR_BIN, _BB_SUID_DROP, logname))
IF_LOGREAD(APPLET(logread, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_LOSETUP(APPLET(losetup, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_LPD(APPLET(lpd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_LPQ(APPLET_ODDNAME(lpq, lpqr, _BB_DIR_USR_BIN, _BB_SUID_DROP, lpq))
IF_LPR(APPLET_ODDNAME(lpr, lpqr, _BB_DIR_USR_BIN, _BB_SUID_DROP, lpr))
IF_LS(APPLET_NOEXEC(ls, ls, _BB_DIR_BIN, _BB_SUID_DROP, ls))
IF_LSATTR(APPLET(lsattr, _BB_DIR_BIN, _BB_SUID_DROP))
IF_LSMOD(APPLET(lsmod, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MODPROBE_SMALL(APPLET_ODDNAME(lsmod, modprobe, _BB_DIR_SBIN, _BB_SUID_DROP, modprobe))
IF_LSPCI(APPLET(lspci, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_LSUSB(APPLET(lsusb, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_UNLZMA(APPLET_ODDNAME(lzmacat, unlzma, _BB_DIR_USR_BIN, _BB_SUID_DROP, lzmacat))
IF_LZOP(APPLET(lzop, _BB_DIR_BIN, _BB_SUID_DROP))
IF_LZOP(APPLET_ODDNAME(lzopcat, lzop, _BB_DIR_USR_BIN, _BB_SUID_DROP, lzopcat))
IF_MAKEDEVS(APPLET(makedevs, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MAKEMIME(APPLET(makemime, _BB_DIR_BIN, _BB_SUID_DROP))
IF_MAN(APPLET(man, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MATCHPATHCON(APPLET(matchpathcon, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_MD5SUM(APPLET_ODDNAME(md5sum, md5_sha1_sum, _BB_DIR_USR_BIN, _BB_SUID_DROP, md5sum))
IF_MDEV(APPLET(mdev, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MESG(APPLET(mesg, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_MICROCOM(APPLET(microcom, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_MKDIR(APPLET_NOFORK(mkdir, mkdir, _BB_DIR_BIN, _BB_SUID_DROP, mkdir))
IF_MKFS_VFAT(APPLET_ODDNAME(mkdosfs, mkfs_vfat, _BB_DIR_SBIN, _BB_SUID_DROP, mkfs_vfat))
IF_MKFS_EXT2(APPLET_ODDNAME(mke2fs, mkfs_ext2, _BB_DIR_SBIN, _BB_SUID_DROP, mkfs_ext2))
IF_MKFIFO(APPLET(mkfifo, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_MKFS_EXT2(APPLET_ODDNAME(mkfs.ext2, mkfs_ext2, _BB_DIR_SBIN, _BB_SUID_DROP, mkfs_ext2))
//IF_MKE2FS(APPLET_ODDNAME(mkfs.ext3, mke2fs, _BB_DIR_SBIN, _BB_SUID_DROP, mkfs_ext3))
IF_MKFS_MINIX(APPLET_ODDNAME(mkfs.minix, mkfs_minix, _BB_DIR_SBIN, _BB_SUID_DROP, mkfs_minix))
IF_MKFS_REISER(APPLET_ODDNAME(mkfs.reiser, mkfs_reiser, _BB_DIR_SBIN, _BB_SUID_DROP, mkfs_reiser))
IF_MKFS_VFAT(APPLET_ODDNAME(mkfs.vfat, mkfs_vfat, _BB_DIR_SBIN, _BB_SUID_DROP, mkfs_vfat))
IF_MKNOD(APPLET(mknod, _BB_DIR_BIN, _BB_SUID_DROP))
IF_CRYPTPW(APPLET_ODDNAME(mkpasswd, cryptpw, _BB_DIR_USR_BIN, _BB_SUID_DROP, mkpasswd))
IF_MKSWAP(APPLET(mkswap, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MKTEMP(APPLET(mktemp, _BB_DIR_BIN, _BB_SUID_DROP))
IF_MODPROBE(APPLET(modprobe, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MODPROBE_SMALL(APPLET(modprobe, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MORE(APPLET(more, _BB_DIR_BIN, _BB_SUID_DROP))
IF_MOUNT(APPLET(mount, _BB_DIR_BIN, IF_DESKTOP(_BB_SUID_MAYBE) IF_NOT_DESKTOP(_BB_SUID_DROP)))
IF_MOUNTPOINT(APPLET(mountpoint, _BB_DIR_BIN, _BB_SUID_DROP))
IF_MSH(APPLET(msh, _BB_DIR_BIN, _BB_SUID_DROP))
IF_MT(APPLET(mt, _BB_DIR_BIN, _BB_SUID_DROP))
IF_MV(APPLET(mv, _BB_DIR_BIN, _BB_SUID_DROP))
IF_NAMEIF(APPLET(nameif, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_NC(APPLET(nc, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_NETSTAT(APPLET(netstat, _BB_DIR_BIN, _BB_SUID_DROP))
IF_NICE(APPLET(nice, _BB_DIR_BIN, _BB_SUID_DROP))
IF_NMETER(APPLET(nmeter, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_NOHUP(APPLET(nohup, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_NSLOOKUP(APPLET(nslookup, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_NTPD(APPLET(ntpd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_OD(APPLET(od, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_OPENVT(APPLET(openvt, _BB_DIR_USR_BIN, _BB_SUID_DROP))
//IF_PARSE(APPLET(parse, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_PASSWD(APPLET(passwd, _BB_DIR_USR_BIN, _BB_SUID_REQUIRE))
IF_PATCH(APPLET(patch, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_PGREP(APPLET(pgrep, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_PIDOF(APPLET(pidof, _BB_DIR_BIN, _BB_SUID_DROP))
IF_PING(APPLET(ping, _BB_DIR_BIN, _BB_SUID_MAYBE))
IF_PING6(APPLET(ping6, _BB_DIR_BIN, _BB_SUID_MAYBE))
IF_PIPE_PROGRESS(APPLET(pipe_progress, _BB_DIR_BIN, _BB_SUID_DROP))
IF_PIVOT_ROOT(APPLET(pivot_root, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_PKILL(APPLET_ODDNAME(pkill, pgrep, _BB_DIR_USR_BIN, _BB_SUID_DROP, pkill))
IF_POPMAILDIR(APPLET(popmaildir, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_HALT(APPLET_ODDNAME(poweroff, halt, _BB_DIR_SBIN, _BB_SUID_DROP, poweroff))
#if 1 /* BIGAP */
IF_PPP_DIAL(APPLET(ppp_dial, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
#endif
IF_PRINTENV(APPLET(printenv, _BB_DIR_BIN, _BB_SUID_DROP))
IF_PRINTF(APPLET_NOFORK(printf, printf, _BB_DIR_USR_BIN, _BB_SUID_DROP, printf))
IF_PS(APPLET(ps, _BB_DIR_BIN, _BB_SUID_DROP))
IF_PSCAN(APPLET(pscan, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_PWD(APPLET_NOFORK(pwd, pwd, _BB_DIR_BIN, _BB_SUID_DROP, pwd))
IF_RAIDAUTORUN(APPLET(raidautorun, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_RDATE(APPLET(rdate, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_RDEV(APPLET(rdev, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_READAHEAD(APPLET(readahead, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_READLINK(APPLET(readlink, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_READPROFILE(APPLET(readprofile, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_REALPATH(APPLET(realpath, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_HALT(APPLET_ODDNAME(reboot, halt, _BB_DIR_SBIN, _BB_SUID_DROP, reboot))
IF_REFORMIME(APPLET(reformime, _BB_DIR_BIN, _BB_SUID_DROP))
IF_RENICE(APPLET(renice, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_RESET(APPLET(reset, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_RESIZE(APPLET(resize, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_RESTORECON(APPLET_ODDNAME(restorecon, setfiles, _BB_DIR_SBIN, _BB_SUID_DROP, restorecon))
IF_RM(APPLET_NOFORK(rm, rm, _BB_DIR_BIN, _BB_SUID_DROP, rm))
IF_RMDIR(APPLET_NOFORK(rmdir, rmdir, _BB_DIR_BIN, _BB_SUID_DROP, rmdir))
IF_RMMOD(APPLET(rmmod, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MODPROBE_SMALL(APPLET_ODDNAME(rmmod, modprobe, _BB_DIR_SBIN, _BB_SUID_DROP, modprobe))
IF_ROUTE(APPLET(route, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_RPM(APPLET(rpm, _BB_DIR_BIN, _BB_SUID_DROP))
IF_RPM2CPIO(APPLET(rpm2cpio, _BB_DIR_USR_BIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_RT(APPLET(rt, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_RTCWAKE(APPLET(rtcwake, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_RUN_PARTS(APPLET_ODDNAME(run-parts, run_parts, _BB_DIR_BIN, _BB_SUID_DROP, run_parts))
IF_RUNCON(APPLET(runcon, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_RUNLEVEL(APPLET(runlevel, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_RUNSV(APPLET(runsv, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_RUNSVDIR(APPLET(runsvdir, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_RX(APPLET(rx, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_SCRIPT(APPLET(script, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_SCRIPTREPLAY(APPLET(scriptreplay, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SED(APPLET(sed, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SELINUXENABLED(APPLET(selinuxenabled, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_SENDMAIL(APPLET(sendmail, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_SEQ(APPLET_NOFORK(seq, seq, _BB_DIR_USR_BIN, _BB_SUID_DROP, seq))
IF_SESTATUS(APPLET(sestatus, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_SETARCH(APPLET(setarch, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SETCONSOLE(APPLET(setconsole, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_SETENFORCE(APPLET(setenforce, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_SETFILES(APPLET(setfiles, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_SETFONT(APPLET(setfont, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_SETKEYCODES(APPLET(setkeycodes, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_SETLOGCONS(APPLET(setlogcons, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_SETSEBOOL(APPLET(setsebool, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_SETSID(APPLET(setsid, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_SETUIDGID(APPLET_ODDNAME(setuidgid, chpst, _BB_DIR_USR_BIN, _BB_SUID_DROP, setuidgid))
IF_FEATURE_SH_IS_ASH(APPLET_ODDNAME(sh, ash, _BB_DIR_BIN, _BB_SUID_DROP, sh))
IF_FEATURE_SH_IS_HUSH(APPLET_ODDNAME(sh, hush, _BB_DIR_BIN, _BB_SUID_DROP, sh))
IF_SHA1SUM(APPLET_ODDNAME(sha1sum, md5_sha1_sum, _BB_DIR_USR_BIN, _BB_SUID_DROP, sha1sum))
IF_SHA256SUM(APPLET_ODDNAME(sha256sum, md5_sha1_sum, _BB_DIR_USR_BIN, _BB_SUID_DROP, sha256sum))
IF_SHA512SUM(APPLET_ODDNAME(sha512sum, md5_sha1_sum, _BB_DIR_USR_BIN, _BB_SUID_DROP, sha512sum))
IF_SHOWKEY(APPLET(showkey, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_SLATTACH(APPLET(slattach, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_SLEEP(APPLET_NOFORK(sleep, sleep, _BB_DIR_BIN, _BB_SUID_DROP, sleep))
#if 1 /* BIGAP */
IF_SMC(APPLET(smc, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SMD(APPLET(smd, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_SOFTLIMIT(APPLET_ODDNAME(softlimit, chpst, _BB_DIR_USR_BIN, _BB_SUID_DROP, softlimit))
IF_SORT(APPLET_NOEXEC(sort, sort, _BB_DIR_USR_BIN, _BB_SUID_DROP, sort))
IF_SPLIT(APPLET(split, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_START_STOP_DAEMON(APPLET_ODDNAME(start-stop-daemon, start_stop_daemon, _BB_DIR_SBIN, _BB_SUID_DROP, start_stop_daemon))
IF_STAT(APPLET(stat, _BB_DIR_BIN, _BB_SUID_DROP))
IF_STRINGS(APPLET(strings, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_STTY(APPLET(stty, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SU(APPLET(su, _BB_DIR_BIN, _BB_SUID_REQUIRE))
IF_SULOGIN(APPLET(sulogin, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_SUM(APPLET(sum, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_SV(APPLET(sv, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_SVLOGD(APPLET(svlogd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_SWAPONOFF(APPLET_ODDNAME(swapoff, swap_on_off, _BB_DIR_SBIN, _BB_SUID_DROP,swapoff))
IF_SWAPONOFF(APPLET_ODDNAME(swapon, swap_on_off, _BB_DIR_SBIN, _BB_SUID_DROP, swapon))
IF_SWITCH_ROOT(APPLET(switch_root, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_SYNC(APPLET_NOFORK(sync, sync, _BB_DIR_BIN, _BB_SUID_DROP, sync))
#if 1 /* BIGAP */
IF_SYS(APPLET(sys, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_BB_SYSCTL(APPLET(sysctl, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_SYSLOGD(APPLET(syslogd, _BB_DIR_SBIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_SYSMOUNT(APPLET(sysmount, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SYSREPAIR(APPLET(sysrepair, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SYSRESET(APPLET(sysreset, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SYSSTARTUP(APPLET(sysstartup, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SYSUMOUNT(APPLET(sysumount, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SYSUPGRADE(APPLET(sysupgrade, _BB_DIR_BIN, _BB_SUID_DROP))
IF_SYSUSBUPGRADE(APPLET(sysusbupgrade, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_TAC(APPLET_NOEXEC(tac, tac, _BB_DIR_USR_BIN, _BB_SUID_DROP, tac))
IF_TAIL(APPLET(tail, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_TAR(APPLET(tar, _BB_DIR_BIN, _BB_SUID_DROP))
IF_TASKSET(APPLET(taskset, _BB_DIR_USR_BIN, _BB_SUID_DROP))
/* IF_TC(APPLET(tc, _BB_DIR_SBIN, _BB_SUID_DROP)) */
IF_TCPSVD(APPLET_ODDNAME(tcpsvd, tcpudpsvd, _BB_DIR_USR_BIN, _BB_SUID_DROP, tcpsvd))
IF_TEE(APPLET(tee, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_TELNET(APPLET(telnet, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_TELNETD(APPLET(telnetd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_TEST(APPLET_NOFORK(test, test, _BB_DIR_USR_BIN, _BB_SUID_DROP, test))
#if ENABLE_FEATURE_TFTP_GET || ENABLE_FEATURE_TFTP_PUT
IF_TFTP(APPLET(tftp, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_TFTPD(APPLET(tftpd, _BB_DIR_USR_BIN, _BB_SUID_DROP))
#endif
IF_TIME(APPLET(time, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_TIMEOUT(APPLET(timeout, _BB_DIR_USR_BIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_TMC(APPLET(tmc, _BB_DIR_BIN, _BB_SUID_DROP))
IF_TMD(APPLET(tmd, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_TOP(APPLET(top, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_TOUCH(APPLET_NOFORK(touch, touch, _BB_DIR_BIN, _BB_SUID_DROP, touch))
IF_TR(APPLET(tr, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_TRACEROUTE(APPLET(traceroute, _BB_DIR_USR_BIN, _BB_SUID_MAYBE))
IF_TRACEROUTE6(APPLET(traceroute6, _BB_DIR_USR_BIN, _BB_SUID_MAYBE))
IF_TRUE(APPLET_NOFORK(true, true, _BB_DIR_BIN, _BB_SUID_DROP, true))
IF_TTY(APPLET(tty, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_TTYSIZE(APPLET(ttysize, _BB_DIR_USR_BIN, _BB_SUID_DROP))
#if 1 /* BIGAP */
IF_TTYUSB(APPLET(ttyUSB, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
#endif
IF_TUNCTL(APPLET(tunctl, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_MKFS_EXT2(APPLET(tune2fs, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_UDHCPC(APPLET(udhcpc, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_UDHCPD(APPLET(udhcpd, _BB_DIR_USR_SBIN, _BB_SUID_DROP))
IF_UDPSVD(APPLET_ODDNAME(udpsvd, tcpudpsvd, _BB_DIR_USR_BIN, _BB_SUID_DROP, udpsvd))
#if 1 /* BIGAP */
IF_UMC(APPLET(umc, _BB_DIR_BIN, _BB_SUID_DROP))
IF_UMD(APPLET(umd, _BB_DIR_BIN, _BB_SUID_DROP))
#endif
IF_UMOUNT(APPLET(umount, _BB_DIR_BIN, _BB_SUID_DROP))
IF_UNAME(APPLET(uname, _BB_DIR_BIN, _BB_SUID_DROP))
IF_UNCOMPRESS(APPLET(uncompress, _BB_DIR_BIN, _BB_SUID_DROP))
IF_UNEXPAND(APPLET_ODDNAME(unexpand, expand, _BB_DIR_USR_BIN, _BB_SUID_DROP, unexpand))
IF_UNIQ(APPLET(uniq, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_UNIX2DOS(APPLET_ODDNAME(unix2dos, dos2unix, _BB_DIR_USR_BIN, _BB_SUID_DROP, unix2dos))
IF_UNLZMA(APPLET(unlzma, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_LZOP(APPLET_ODDNAME(unlzop, lzop, _BB_DIR_USR_BIN, _BB_SUID_DROP, unlzop))
IF_UNZIP(APPLET(unzip, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_UPTIME(APPLET(uptime, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_USLEEP(APPLET_NOFORK(usleep, usleep, _BB_DIR_BIN, _BB_SUID_DROP, usleep))
IF_UUDECODE(APPLET(uudecode, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_UUENCODE(APPLET(uuencode, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_VCONFIG(APPLET(vconfig, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_VI(APPLET(vi, _BB_DIR_BIN, _BB_SUID_DROP))
IF_VLOCK(APPLET(vlock, _BB_DIR_USR_BIN, _BB_SUID_REQUIRE))
IF_VOLNAME(APPLET(volname, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_WALL(APPLET(wall, _BB_DIR_USR_BIN, _BB_SUID_REQUIRE))
IF_WATCH(APPLET(watch, _BB_DIR_BIN, _BB_SUID_DROP))
IF_WATCHDOG(APPLET(watchdog, _BB_DIR_SBIN, _BB_SUID_DROP))
IF_WC(APPLET(wc, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_WGET(APPLET(wget, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_WHICH(APPLET(which, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_WHO(APPLET(who, _BB_DIR_USR_BIN, _BB_SUID_DROP))
IF_WHOAMI(APPLET_NOFORK(whoami, whoami, _BB_DIR_USR_BIN, _BB_SUID_DROP, whoami))
IF_XARGS(APPLET_NOEXEC(xargs, xargs, _BB_DIR_USR_BIN, _BB_SUID_DROP, xargs))
IF_YES(APPLET_NOFORK(yes, yes, _BB_DIR_USR_BIN, _BB_SUID_DROP, yes))
IF_GUNZIP(APPLET_ODDNAME(zcat, gunzip, _BB_DIR_BIN, _BB_SUID_DROP, zcat))
IF_ZCIP(APPLET(zcip, _BB_DIR_SBIN, _BB_SUID_DROP))

#if !defined(PROTOTYPES) && !defined(NAME_MAIN_CNAME) && !defined(MAKE_USAGE)
};
#endif

#undef APPLET
#undef APPLET_ODDNAME
#undef APPLET_NOEXEC
#undef APPLET_NOFORK
