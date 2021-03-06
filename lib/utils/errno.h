#ifndef __ERRNO_H_6960dfed91a54253a91018ecc68f8048__
#define __ERRNO_H_6960dfed91a54253a91018ecc68f8048__
/******************************************************************************/
#ifdef __BOOT__
/* 
* linux/include/uapi/asm/generic/errno-base.h 
*/
#define	EPERM		 1	/* Operation not permitted */
#define	ENOENT		 2	/* No such file or directory */
#define	ESRCH		 3	/* No such process */
#define	EINTR		 4	/* Interrupted system call */
#define	EIO		     5	/* I/O error */
#define	ENXIO		 6	/* No such device or address */
#define	E2BIG		 7	/* Argument list too long */
#define	ENOEXEC		 8	/* Exec format error */
#define	EBADF		 9	/* Bad file number */
#define	ECHILD		10	/* No child processes */
#define	EAGAIN		11	/* Try again */
#define	ENOMEM		12	/* Out of memory */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
#define	ENOTBLK		15	/* Block device required */
#define	EBUSY		16	/* Device or resource busy */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Cross-device link */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory */
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* File table overflow */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Not a typewriter */
#define	ETXTBSY		26	/* Text file busy */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Illegal seek */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */
#define	EDOM		33	/* Math argument out of domain of func */
#define	ERANGE		34	/* Math result not representable */

/* 
* linux/include/uapi/asm/generic/errno.h 
*/
#define	EDEADLK		35	/* Resource deadlock would occur */
#define	ENAMETOOLONG	36	/* File name too long */
#define	ENOLCK		37	/* No record locks available */
#define	ENOSYS		38	/* Function not implemented */
#define	ENOTEMPTY	39	/* Directory not empty */
#define	ELOOP		40	/* Too many symbolic links encountered */
#define	EWOULDBLOCK	EAGAIN	/* Operation would block */
#define	ENOMSG		42	/* No message of desired type */
#define	EIDRM		43	/* Identifier removed */
#define	ECHRNG		44	/* Channel number out of range */
#define	EL2NSYNC	45	/* Level 2 not synchronized */
#define	EL3HLT		46	/* Level 3 halted */
#define	EL3RST		47	/* Level 3 reset */
#define	ELNRNG		48	/* Link number out of range */
#define	EUNATCH		49	/* Protocol driver not attached */
#define	ENOCSI		50	/* No CSI structure available */
#define	EL2HLT		51	/* Level 2 halted */
#define	EBADE		52	/* Invalid exchange */
#define	EBADR		53	/* Invalid request descriptor */
#define	EXFULL		54	/* Exchange full */
#define	ENOANO		55	/* No anode */
#define	EBADRQC		56	/* Invalid request code */
#define	EBADSLT		57	/* Invalid slot */

#define	EDEADLOCK	EDEADLK

#define	EBFONT		59	/* Bad font file format */
#define	ENOSTR		60	/* Device not a stream */
#define	ENODATA		61	/* No data available */
#define	ETIME		62	/* Timer expired */
#define	ENOSR		63	/* Out of streams resources */
#define	ENONET		64	/* Machine is not on the network */
#define	ENOPKG		65	/* Package not installed */
#define	EREMOTE		66	/* Object is remote */
#define	ENOLINK		67	/* Link has been severed */
#define	EADV		68	/* Advertise error */
#define	ESRMNT		69	/* Srmount error */
#define	ECOMM		70	/* Communication error on send */
#define	EPROTO		71	/* Protocol error */
#define	EMULTIHOP	72	/* Multihop attempted */
#define	EDOTDOT		73	/* RFS specific error */
#define	EBADMSG		74	/* Not a data message */
#define	EOVERFLOW	75	/* Value too large for defined data type */
#define	ENOTUNIQ	76	/* Name not unique on network */
#define	EBADFD		77	/* File descriptor in bad state */
#define	EREMCHG		78	/* Remote address changed */
#define	ELIBACC		79	/* Can not access a needed shared library */
#define	ELIBBAD		80	/* Accessing a corrupted shared library */
#define	ELIBSCN		81	/* .lib section in a.out corrupted */
#define	ELIBMAX		82	/* Attempting to link in too many shared libraries */
#define	ELIBEXEC	83	/* Cannot exec a shared library directly */
#define	EILSEQ		84	/* Illegal byte sequence */
#define	ERESTART	85	/* Interrupted system call should be restarted */
#define	ESTRPIPE	86	/* Streams pipe error */
#define	EUSERS		87	/* Too many users */
#define	ENOTSOCK	88	/* Socket operation on non-socket */
#define	EDESTADDRREQ	89	/* Destination address required */
#define	EMSGSIZE	90	/* Message too long */
#define	EPROTOTYPE	91	/* Protocol wrong type for socket */
#define	ENOPROTOOPT	92	/* Protocol not available */
#define	EPROTONOSUPPORT	93	/* Protocol not supported */
#define	ESOCKTNOSUPPORT	94	/* Socket type not supported */
#define	EOPNOTSUPP	95	/* Operation not supported on transport endpoint */
#define	EPFNOSUPPORT	96	/* Protocol family not supported */
#define	EAFNOSUPPORT	97	/* Address family not supported by protocol */
#define	EADDRINUSE	98	/* Address already in use */
#define	EADDRNOTAVAIL	99	/* Cannot assign requested address */
#define	ENETDOWN	100	/* Network is down */
#define	ENETUNREACH	101	/* Network is unreachable */
#define	ENETRESET	102	/* Network dropped connection because of reset */
#define	ECONNABORTED	103	/* Software caused connection abort */
#define	ECONNRESET	104	/* Connection reset by peer */
#define	ENOBUFS		105	/* No buffer space available */
#define	EISCONN		106	/* Transport endpoint is already connected */
#define	ENOTCONN	107	/* Transport endpoint is not connected */
#define	ESHUTDOWN	108	/* Cannot send after transport endpoint shutdown */
#define	ETOOMANYREFS	109	/* Too many references: cannot splice */
#define	ETIMEDOUT	110	/* Connection timed out */
#define	ECONNREFUSED	111	/* Connection refused */
#define	EHOSTDOWN	112	/* Host is down */
#define	EHOSTUNREACH	113	/* No route to host */
#define	EALREADY	114	/* Operation already in progress */
#define	EINPROGRESS	115	/* Operation now in progress */
#define	ESTALE		116	/* Stale NFS file handle */
#define	EUCLEAN		117	/* Structure needs cleaning */
#define	ENOTNAM		118	/* Not a XENIX named type file */
#define	ENAVAIL		119	/* No XENIX semaphores available */
#define	EISNAM		120	/* Is a named type file */
#define	EREMOTEIO	121	/* Remote I/O error */
#define	EDQUOT		122	/* Quota exceeded */

#define	ENOMEDIUM	123	/* No medium found */
#define	EMEDIUMTYPE	124	/* Wrong medium type */
#define	ECANCELED	125	/* Operation Canceled */
#define	ENOKEY		126	/* Required key not available */
#define	EKEYEXPIRED	127	/* Key has expired */
#define	EKEYREVOKED	128	/* Key has been revoked */
#define	EKEYREJECTED	129	/* Key was rejected by service */

/* for robust mutexes */
#define	EOWNERDEAD	130	/* Owner died */
#define	ENOTRECOVERABLE	131	/* State not recoverable */
#define ERFKILL		132	/* Operation not possible due to RF-kill */
#define EHWPOISON	133	/* Memory page has hardware error */

#define errno       EINVAL
#endif

#ifndef EHWPOISON
#define EHWPOISON   133
#endif

#define ERRNO(_x)   (140 + (_x))

enum {
    EINVAL0     = ERRNO(0), 
    EINVAL1     = ERRNO(1),
    EINVAL2     = ERRNO(2),
    EINVAL3     = ERRNO(3),
    EINVAL4     = ERRNO(4),
    EINVAL5     = ERRNO(5),
    EINVAL6     = ERRNO(6),
    EINVAL7     = ERRNO(7),
    EINVAL8     = ERRNO(8),
    EINVAL9     = ERRNO(9),
    
    ENOSUPPORT  = ERRNO(10),
    ENOINIT     = ERRNO(11),
    ENOEXIST    = ERRNO(12),
    ENOSPACE    = ERRNO(13),
    ENOEMPTY    = ERRNO(14),
    ENOINLIST   = ERRNO(15),
    ENOLOCK     = ERRNO(16),
    ENOAUTH     = ERRNO(17),
    ENOMATCH    = ERRNO(18),
    ENOMUST     = ERRNO(19),
    
    EBADIDX     = ERRNO(20),
    EBADREF     = ERRNO(21),
    EBADCRC     = ERRNO(22),
    EBADENV     = ERRNO(23),
    EBADMAC     = ERRNO(24),
    EBADREASON  = ERRNO(25),
    EBADJSON    = ERRNO(26),
    EBADFSM     = ERRNO(27),
    EBADHEX     = ERRNO(28),
    EBADSEQ     = ERRNO(29),    
    EBADPROTO   = ERRNO(30),
    EBADVERSION = ERRNO(31),
    EBADSLOT    = ERRNO(32),
    EBADCONF    = ERRNO(33),
    EBADBASEMAC = ERRNO(34),
    EBADIP      = ERRNO(35),
    EBADJOBJ    = ERRNO(36),
    EBADJTYPE   = ERRNO(37),
    EBADRULE    = ERRNO(38),
    EBADMD5     = ERRNO(39),
    
    ETOOBIG     = ERRNO(40),
    ETOOSMALL   = ERRNO(41),
    EUNREADABLE = ERRNO(42),
    EUNWRITABLE = ERRNO(43),
    EKEYNULL    = ERRNO(44),
    EKEYBAD     = ERRNO(45),
    ESYSTEM     = ERRNO(46),
    EFORMAT     = ERRNO(47),
    ELIMIT      = ERRNO(48),
    EINLIST     = ERRNO(49),
    ETIMEOUT    = ERRNO(50),
    EEMPTY      = ERRNO(51),
    EFULL       = ERRNO(52),
    EHELP       = ERRNO(53),
    ETRYEXCEED  = ERRNO(54),
    EHACKED     = ERRNO(55),
    EMOUNT      = ERRNO(56),
    EEXEC       = ERRNO(57),
    EREINIT     = ERRNO(58),
    ECURLFAIL   = ERRNO(59),
    EFAKESEVER  = ERRNO(60),
    ERSYNC      = ERRNO(61),
    EBUG        = ERRNO(62),
    EDNS        = ERRNO(63),
    ECONFIG     = ERRNO(64),
    EFILTER     = ERRNO(65),
    EFORK       = ERRNO(66),
    EDEAMON     = ERRNO(67),
    E2SMALL     = ERRNO(68),
    ECALLBACK   = ERRNO(69),
    ESCRIPT     = ERRNO(70),
    EBOUND      = ERRNO(71),
    EPROTOCOL   = ERRNO(72),
    EDAMAGED    = ERRNO(73),
    ENOREADY    = ERRNO(74),
    EPEERCLOSED = ERRNO(75),
    
    EDLLLOAD        = ERRNO(80),
    EDLLSYMLOAD     = ERRNO(81),
    EDLLPARAMSIZE   = ERRNO(82),
    EDLLPARAMCOUNT  = ERRNO(83),
    EDLLRESULTSIZE  = ERRNO(84),

    ENOPERM         = ERRNO(90),
    EBADFILE        = ERRNO(91),
    EBASE64         = ERRNO(92),
    EBADENUM        = ERRNO(93),
    EBADCB          = ERRNO(94),
    ENOCERT         = ERRNO(95),
    EBADKEY         = ERRNO(96),
    EBADSIZE        = ERRNO(97),
    EOBJ2JSON       = ERRNO(98),
};

static inline int
shell_error(int err)
{
    return (err<0)?-err:err;
}

static inline int
native_error(int err)
{
    return (err>0)?-err:err;
}
/******************************************************************************/
#endif /* __ERRNO_H_6960dfed91a54253a91018ecc68f8048__ */
