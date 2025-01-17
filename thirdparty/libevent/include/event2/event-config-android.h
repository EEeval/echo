/* event2/event-config.h
*
* This file was generated by autoconf when libevent was built, and post-
* processed by Libevent so that its macros would have a uniform prefix.
*
* DO NOT EDIT THIS FILE.
*
* Do not rely on macros in this file existing in later versions.
*/

#ifndef EVENT__CONFIG_ANDROID_H_
#define EVENT__CONFIG_ANDROID_H_
/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if libevent should build without support for a debug mode */
/* #undef EVENT__DISABLE_DEBUG_MODE */

/* Define if libevent should not allow replacing the mm functions */
/* #undef EVENT__DISABLE_MM_REPLACEMENT */

/* Define if libevent should not be compiled with thread support */
/* #undef EVENT__DISABLE_THREAD_SUPPORT */

/* Define to 1 if you have the `arc4random' function. */
#define EVENT__HAVE_ARC4RANDOM 1

/* Define to 1 if you have the `arc4random_buf' function. */
#define EVENT__HAVE_ARC4RANDOM_BUF 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define EVENT__HAVE_ARPA_INET_H 1

/* Define to 1 if you have the `clock_gettime' function. */
#define EVENT__HAVE_CLOCK_GETTIME 1

/* Define to 1 if you have the declaration of `CTL_KERN', and to 0 if you
   don't. */
/* #undef EVENT__HAVE_DECL_CTL_KERN */

/* Define to 1 if you have the declaration of `KERN_ARND', and to 0 if you
   don't. */
/* #undef EVENT__HAVE_DECL_KERN_ARND */

/* Define to 1 if you have the declaration of `KERN_RANDOM', and to 0 if you
   don't. */
/* #undef EVENT__HAVE_DECL_KERN_RANDOM */

/* Define to 1 if you have the declaration of `RANDOM_UUID', and to 0 if you
   don't. */
/* #undef EVENT__HAVE_DECL_RANDOM_UUID */

/* Define if /dev/poll is available */
/* #undef EVENT__HAVE_DEVPOLL */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define EVENT__HAVE_DLFCN_H 1

/* Define if your system supports the epoll system calls */
#define EVENT__HAVE_EPOLL 1

/* Define to 1 if you have the `epoll_ctl' function. */
#define EVENT__HAVE_EPOLL_CTL 1

/* Define to 1 if you have the `eventfd' function. */
#define EVENT__HAVEEVENT_FD 1

/* Define if your system supports event ports */
/* #undef EVENT__HAVEEVENT__PORTS */

/* Define to 1 if you have the `fcntl' function. */
#define EVENT__HAVE_FCNTL 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define EVENT__HAVE_FCNTL_H 1

/* Define to 1 if the system has the type `fd_mask'. */
/* #undef EVENT__HAVE_FD_MASK */

/* Do we have getaddrinfo()? */
#define EVENT__HAVE_GETADDRINFO 1

/* Define to 1 if you have the `getegid' function. */
#define EVENT__HAVE_GETEGID 1

/* Define to 1 if you have the `geteuid' function. */
#define EVENT__HAVE_GETEUID 1

/* Define this if you have any gethostbyname_r() */
/* #undef EVENT__HAVE_GETHOSTBYNAME_R */

/* Define this if gethostbyname_r takes 3 arguments */
/* #undef EVENT__HAVE_GETHOSTBYNAME_R_3_ARG */

/* Define this if gethostbyname_r takes 5 arguments */
/* #undef EVENT__HAVE_GETHOSTBYNAME_R_5_ARG */

/* Define this if gethostbyname_r takes 6 arguments */
/* #undef EVENT__HAVE_GETHOSTBYNAME_R_6_ARG */

/* Define to 1 if you have the `getnameinfo' function. */
#define EVENT__HAVE_GETNAMEINFO 1

/* Define to 1 if you have the `getprotobynumber' function. */
#define EVENT__HAVE_GETPROTOBYNUMBER 1

/* Define to 1 if you have the `getservbyname' function. */
/* #undef EVENT__HAVE_GETSERVBYNAME */

/* Define to 1 if you have the `gettimeofday' function. */
#define EVENT__HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the `inet_aton' function. */
#define EVENT__HAVE_INET_ATON 1

/* Define to 1 if you have the `inet_ntop' function. */
#define EVENT__HAVE_INET_NTOP 1

/* Define to 1 if you have the `inet_pton' function. */
#define EVENT__HAVE_INET_PTON 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define EVENT__HAVE_INTTYPES_H 1

/* Define to 1 if you have the `issetugid' function. */
/* #undef EVENT__HAVE_ISSETUGID */

/* Define to 1 if you have the `kqueue' function. */
/* #undef EVENT__HAVE_KQUEUE */

/* Define if the system has zlib */
#define EVENT__HAVE_LIBZ 1

/* Define to 1 if you have the <memory.h> header file. */
#define EVENT__HAVE_MEMORY_H 1

/* Define to 1 if you have the `mmap' function. */
#define EVENT__HAVE_MMAP 1

/* Define to 1 if you have the <netdb.h> header file. */
#define EVENT__HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in6.h> header file. */
#define EVENT__HAVE_NETINET_IN6_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define EVENT__HAVE_NETINET_IN_H 1

/* Define if the system has openssl */
/* #undef EVENT__HAVE_OPENSSL */

/* Define to 1 if you have the <openssl/bio.h> header file. */
/* #undef EVENT__HAVE_OPENSSL_BIO_H */

/* Define to 1 if you have the `pipe' function. */
#define EVENT__HAVE_PIPE 1

/* Define to 1 if you have the `poll' function. */
#define EVENT__HAVE_POLL 1

/* Define to 1 if you have the <poll.h> header file. */
#define EVENT__HAVE_POLL_H 1

/* Define to 1 if you have the `port_create' function. */
/* #undef EVENT__HAVE_PORT_CREATE */

/* Define to 1 if you have the <port.h> header file. */
/* #undef EVENT__HAVE_PORT_H */

/* Define if you have POSIX threads libraries and header files. */
/* #undef EVENT__HAVE_PTHREAD */

/* Define if we have pthreads on this system */
#define EVENT__HAVE_PTHREADS 1

/* Define to 1 if you have the `putenv' function. */
#define EVENT__HAVE_PUTENV 1

/* Define to 1 if the system has the type `sa_family_t'. */
#define EVENT__HAVE_SA_FAMILY_T 1

/* Define to 1 if you have the `select' function. */
#define EVENT__HAVE_SELECT 1

/* Define to 1 if you have the `sendfile' function. */
#define EVENT__HAVE_SENDFILE 1

/* Define to 1 if you have the `setenv' function. */
#define EVENT__HAVE_SETENV 1

/* Define if F_SETFD is defined in <fcntl.h> */
#define EVENT__HAVE_SETFD 1

/* Define to 1 if you have the `sigaction' function. */
#define EVENT__HAVE_SIGACTION 1

/* Define to 1 if you have the `signal' function. */
#define EVENT__HAVE_SIGNAL 1

/* Define to 1 if you have the `splice' function. */
#define EVENT__HAVE_SPLICE 1

/* Define to 1 if you have the <stdarg.h> header file. */
#define EVENT__HAVE_STDARG_H 1

/* Define to 1 if you have the <stddef.h> header file. */
#define EVENT__HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define EVENT__HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define EVENT__HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define EVENT__HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define EVENT__HAVE_STRING_H 1

/* Define to 1 if you have the `strlcpy' function. */
#define EVENT__HAVE_STRLCPY 1

/* Define to 1 if you have the `strsep' function. */
#define EVENT__HAVE_STRSEP 1

/* Define to 1 if you have the `strtok_r' function. */
#define EVENT__HAVE_STRTOK_R 1

/* Define to 1 if you have the `strtoll' function. */
#define EVENT__HAVE_STRTOLL 1

/* Define to 1 if the system has the type `struct addrinfo'. */
#define EVENT__HAVE_STRUCT_ADDRINFO 1

/* Define to 1 if the system has the type `struct in6_addr'. */
#define EVENT__HAVE_STRUCT_IN6_ADDR 1

/* Define to 1 if `s6_addr16' is a member of `struct in6_addr'. */
#define EVENT__HAVE_STRUCT_IN6_ADDR_S6_ADDR16 1

/* Define to 1 if `s6_addr32' is a member of `struct in6_addr'. */
#define EVENT__HAVE_STRUCT_IN6_ADDR_S6_ADDR32 1

/* Define to 1 if the system has the type `struct sockaddr_in6'. */
#define EVENT__HAVE_STRUCT_SOCKADDR_IN6 1

/* Define to 1 if `sin6_len' is a member of `struct sockaddr_in6'. */
/* #undef EVENT__HAVE_STRUCT_SOCKADDR_IN6_SIN6_LEN */

/* Define to 1 if `sin_len' is a member of `struct sockaddr_in'. */
/* #undef EVENT__HAVE_STRUCT_SOCKADDR_IN_SIN_LEN */

/* Define to 1 if the system has the type `struct sockaddr_storage'. */
#define EVENT__HAVE_STRUCT_SOCKADDR_STORAGE 1

/* Define to 1 if `ss_family' is a member of `struct sockaddr_storage'. */
#define EVENT__HAVE_STRUCT_SOCKADDR_STORAGE_SS_FAMILY 1

/* Define to 1 if `__ss_family' is a member of `struct sockaddr_storage'. */
/* #undef EVENT__HAVE_STRUCT_SOCKADDR_STORAGE___SS_FAMILY */

/* Define to 1 if you have the `sysctl' function. */
/* #undef EVENT__HAVE_SYSCTL */

/* Define to 1 if you have the <sys/devpoll.h> header file. */
/* #undef EVENT__HAVE_SYS_DEVPOLL_H */

/* Define to 1 if you have the <sys/epoll.h> header file. */
#define EVENT__HAVE_SYS_EPOLL_H 1

/* Define to 1 if you have the <sys/eventfd.h> header file. */
#define EVENT__HAVE_SYSEVENT_FD_H 1

/* Define to 1 if you have the <sys/event.h> header file. */
/* #undef EVENT__HAVE_SYSEVENT__H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define EVENT__HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#define EVENT__HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define EVENT__HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/queue.h> header file. */
#define EVENT__HAVE_SYS_QUEUE_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define EVENT__HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/sendfile.h> header file. */
#define EVENT__HAVE_SYS_SENDFILE_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define EVENT__HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define EVENT__HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/sysctl.h> header file. */
/* #undef EVENT__HAVE_SYS_SYSCTL_H */

/* Define to 1 if you have the <sys/time.h> header file. */
#define EVENT__HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define EVENT__HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/uio.h> header file. */
#define EVENT__HAVE_SYS_UIO_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define EVENT__HAVE_SYS_WAIT_H 1

/* Define if TAILQ_FOREACH is defined in <sys/queue.h> */
#define EVENT__HAVE_TAILQFOREACH 1

/* Define if timeradd is defined in <sys/time.h> */
#define EVENT__HAVE_TIMERADD 1

/* Define if timerclear is defined in <sys/time.h> */
#define EVENT__HAVE_TIMERCLEAR 1

/* Define if timercmp is defined in <sys/time.h> */
#define EVENT__HAVE_TIMERCMP 1

/* Define if timerisset is defined in <sys/time.h> */
#define EVENT__HAVE_TIMERISSET 1

/* Define to 1 if the system has the type `uint16_t'. */
#define EVENT__HAVE_UINT16_T 1

/* Define to 1 if the system has the type `uint32_t'. */
#define EVENT__HAVE_UINT32_T 1

/* Define to 1 if the system has the type `uint64_t'. */
#define EVENT__HAVE_UINT64_T 1

/* Define to 1 if the system has the type `uint8_t'. */
#define EVENT__HAVE_UINT8_T 1

/* Define to 1 if the system has the type `uintptr_t'. */
#define EVENT__HAVE_UINTPTR_T 1

/* Define to 1 if you have the `umask' function. */
#define EVENT__HAVE_UMASK 1

/* Define to 1 if you have the <unistd.h> header file. */
#define EVENT__HAVE_UNISTD_H 1

/* Define to 1 if you have the `unsetenv' function. */
#define EVENT__HAVE_UNSETENV 1

/* Define to 1 if you have the `vasprintf' function. */
#define EVENT__HAVE_VASPRINTF 1

/* Define if kqueue works correctly with pipes */
/* #undef EVENT__HAVE_WORKING_KQUEUE */

/* Define to 1 if you have the <zlib.h> header file. */
#define EVENT__HAVE_ZLIB_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define EVENT__LT_OBJDIR ".libs/"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef EVENT__NO_MINUS_C_MINUS_O */

/* Numeric representation of the version */
#define EVENT__NUMERIC_VERSION 0x02001600

/* Name of package */
#define EVENT__PACKAGE "libevent"

/* Define to the address where bug reports for this package should be sent. */
#define EVENT__PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define EVENT__PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define EVENT__PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define EVENT__PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define EVENT__PACKAGE_URL ""

/* Define to the version of this package. */
#define EVENT__PACKAGE_VERSION ""

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef EVENT__PTHREAD_CREATE_JOINABLE */

/* The size of `int', as computed by sizeof. */
#define EVENT__SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define EVENT__SIZEOF_LONG 4

/* The size of `long long', as computed by sizeof. */
#define EVENT__SIZEOF_LONG_LONG 8

/* The size of `off_t', as computed by sizeof. */
#define EVENT__SIZEOF_OFF_T 4

/* The size of `pthread_t', as computed by sizeof. */
#define EVENT__SIZEOF_PTHREAD_T 4

/* The size of `short', as computed by sizeof. */
#define EVENT__SIZEOF_SHORT 2

/* The size of `size_t', as computed by sizeof. */
#define EVENT__SIZEOF_SIZE_T 4

/* The size of `void *', as computed by sizeof. */
#define EVENT__SIZEOF_VOID_P 4

/* Define to 1 if you have the ANSI C header files. */
#define EVENT__STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define EVENT__TIME_WITH_SYS_TIME 1

/* Version number of package */
#define EVENT__VERSION "2.0.22-stable"

/* Define to appropriate substitue if compiler doesnt have __func__ */
/* #undef EVENT____func__ */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef EVENT__const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef EVENT____cplusplus
/* #undef EVENT__inline */
#endif

/* Define to `int' if <sys/types.h> does not define. */
/* #undef EVENT__pid_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef EVENT__size_t */

/* Define to unsigned int if you dont have it */
/* #undef EVENT__socklen_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef EVENT__ssize_t */

#endif /* event2/event-config.h */
