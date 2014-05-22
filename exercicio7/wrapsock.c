/*
 *   A implementacao das funcoes wrapper abaixo foram retiradas da pagina web
 * http://www.cs.odu.edu/~cs779/stevens2nd/lib/wrapsock.c e foram alteradas
 * devido as necessidades particulares do Servidor de Eco.
 */

#include  "myNetworking.h"

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  int    n;

again:
  if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef  EPROTO
    if (errno == EPROTO || errno == ECONNABORTED)
#else
    if (errno == ECONNABORTED)
#endif
      goto again;
    else
      perror("accept error");
  }

  return(n);
}

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
  if (bind(fd, sa, salen) < 0)
    perror("bind error");
}

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
  if (connect(fd, sa, salen) < 0)
    perror("connect error");
}

void
Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  if (getpeername(fd, sa, salenptr) < 0)
    perror("getpeername error");
}

void
Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  if (getsockname(fd, sa, salenptr) < 0)
    perror("getsockname error");
}

void
Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
  if (getsockopt(fd, level, optname, optval, optlenptr) < 0)
    perror("getsockopt error");
}

int
Isfdtype(int fd, int fdtype)
{
  int    n;

  if ( (n = isfdtype(fd, fdtype)) < 0)
    perror("isfdtype error");
  return(n);
}

/* include Listen */
void
Listen(int fd, int backlog)
{
  char  *ptr;

    /*4can override 2nd argument with environment variable */
  if ( (ptr = getenv("LISTENQ")) != NULL)
    backlog = atoi(ptr);

  if (listen(fd, backlog) < 0)
    perror("listen error");
}
/* end Listen */

int
Poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
  int    n;

  if ( (n = poll(fdarray, nfds, timeout)) < 0)
    perror("poll error");

  return(n);
}

ssize_t
Recv(int fd, void *ptr, size_t nbytes, int flags)
{
  ssize_t    n;

  if ( (n = recv(fd, ptr, nbytes, flags)) < 0)
    perror("recv error");
  return(n);
}

ssize_t
Recvfrom(int fd, void *ptr, size_t nbytes, int flags,
     struct sockaddr *sa, socklen_t *salenptr)
{
  ssize_t    n;

  if ( (n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0)
    perror("recvfrom error");
  return(n);
}

ssize_t
Recvmsg(int fd, struct msghdr *msg, int flags)
{
  ssize_t    n;

  if ( (n = recvmsg(fd, msg, flags)) < 0)
    perror("recvmsg error");
  return(n);
}

int
Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
  int    n;

  if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
    perror("select error");
  return(n);    /* can return 0 on timeout */
}

void
Send(int fd, const void *ptr, size_t nbytes, int flags)
{
  if (send(fd, ptr, nbytes, flags) != nbytes)
    perror("send error");
}

void
Sendto(int fd, const void *ptr, size_t nbytes, int flags,
     const struct sockaddr *sa, socklen_t salen)
{
  if (sendto(fd, ptr, nbytes, flags, sa, salen) != nbytes)
    perror("sendto error");
}

void
Sendmsg(int fd, const struct msghdr *msg, int flags)
{
  int      i;
  ssize_t    nbytes;

  nbytes = 0;  /* must first figure out what return value should be */
  for (i = 0; i < msg->msg_iovlen; i++)
    nbytes += msg->msg_iov[i].iov_len;

  if (sendmsg(fd, msg, flags) != nbytes)
    perror("sendmsg error");
}

void
Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
  if (setsockopt(fd, level, optname, optval, optlen) < 0)
    perror("setsockopt error");
}

void
Shutdown(int fd, int how)
{
  if (shutdown(fd, how) < 0)
    perror("shutdown error");
}

int
Sockatmark(int fd)
{
  int    n;

  if ( (n = sockatmark(fd)) < 0)
    perror("sockatmark error");
  return(n);
}

/* include Socket */
int
Socket(int family, int type, int protocol)
{
  int    n;

  if ( (n = socket(family, type, protocol)) < 0)
    perror("socket error");
  return(n);
}
/* end Socket */

void
Socketpair(int family, int type, int protocol, int *fd)
{
  int    n;

  if ( (n = socketpair(family, type, protocol, fd)) < 0)
    perror("socketpair error");
}
