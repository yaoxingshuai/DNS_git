#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/idmp.h>

#ifndef IDMP_PN_H_
#define IDMP_PN_H_

#define SA struct sockaddr

int idmp_pton(const char *src, struct in_id *dst);
const char *idmp_ntop(const void *id, char *dst, int len);
#endif
