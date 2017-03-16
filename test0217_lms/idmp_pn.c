#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/idmp.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <netinet/ucmp.h>
#include "idmp_pn.h"
const char *idmp_ntop(const void *id, char *dst, int len)
{
	__u16 tmp[8];
	char str[8][6];
	char buf[50];
	char buf2[50];
	char *pstart, *pend, *p0, *p1;
	struct in_id *src;
	int i, l;
	src = (struct in_id *)id;
	bzero(&tmp, sizeof(tmp));
	memset(dst, '\0', len);
	memset(buf, '\0', 50);
	memset(buf2, '\0', 50);
	memset(str, '\0', 8 * 6);
	
	for (i=0; i<8; i++) {
		tmp[i] = ntohs(src->s_id16[i]);
		sprintf(str[i], "%d", tmp[i]);
		if (strcmp(str[i], "0") == 0)
			strcat(buf, "O");
		else
			strcat(buf, str[i]);
		if (i<7)
			strcat(buf, "-");
	}
	pstart = buf;
	pend = strchr(buf, '\0');
	p0 = strchr(buf, 'O');
	if (p0 && p0 == pstart ) {
		p1 = pstart;
	} else if (p0 && p0 != pstart ) {
		strncat(buf2, pstart, p0-pstart-1);
		strcat(buf2, "~");
		p1 = p0;
	} else if (!p0) {
		strcat(buf2, pstart);
		p1 = NULL;
	}
	while (p1 && p1 != pend) {
		if (*p1 != 'O' && *p1 != '-') {
			strcat(buf2, p1);
			break;
		}
		p1++;
	}
	pstart = buf2;
	pend = strchr(buf2, '\0');
	for (p1 = pstart; p1 != pend; p1++) {
		if (*p1 == 'O') 
			*p1 = '0';
	}
	
	l = pend - pstart;
	if (len < l+1)
		return NULL;
	
	strcat(dst, buf2);
	
	return dst;
}

int idmp_pton(const char *src, struct in_id *dst)
{
	char before[50];
	char behind[50];
	char tmp[8][6];
	__u16 tmp2[8];
	char *pend, *pwave, *pband;
	int i, j;
	
	memset(before, '\0', 50);
	memset(behind, '\0', 50);
	memset(tmp, '\0', 8 * 6);
	bzero(tmp2, sizeof(tmp2));
	bzero(dst, sizeof(*dst));
	//pstart = (unsigned char *)src;
	const char *pstart=src;
	pend = strchr(src, '\0');
	// check id validity 
	
	for (i=0; i<pend-pstart; i++) {
		if (src[i] < '0' || src[i] > '9') {
			if (src[i] != '-' && src[i] != '~')
				return 0;
		}
	}
	if ((pwave = strchr(src, '~')) != NULL) {
	// id length is fixed to 128 bits 
		if (strchr(src, '~') != strrchr(src, '~'))
			return 0;
	
		if (pwave != pstart) {
			memcpy(before, src, pwave-pstart);
			memcpy(behind, pwave+1, pend-pwave-1);
		} else if (pwave == pstart) {
			memcpy(behind, pwave+1, pend-pwave-1);
		}
	} else {
	// id length is variable 
	
		memcpy(behind, src, pend-pstart);
	}
	if (before[0] != '\0') {
		pstart = before;
		pend = strchr(before, '\0');
		i = 0;
		while ((pband = strchr(pstart, '-')) != NULL) {
			memcpy(tmp[i], pstart, pband-pstart);
			i++;
			pstart = pband+1;
		}
		memcpy(tmp[i], pstart, pend-pstart);
	}
	if (behind[0] != '\0') {
		pstart = behind;
		pend = strchr(behind, '\0');
		i = 7;
		while((pband = strrchr(pstart, '-')) != NULL) {
			memcpy(tmp[i], pband+1, pend-pband-1);
			i--;
			pend = pband;
			*pband = '\0';
		}
		memcpy(tmp[i], pstart, pend-pstart);
	}
	for (i=0; i<8; i++) {
		if (tmp[i][0] == '\0')
			tmp[i][0]='0';
		
		for (j=0; tmp[i][j] != '\0'; j++) 
			tmp2[i] = 10 * tmp2[i] + (tmp[i][j] - '0');
		
		if (tmp2[i] > 65535)
			return 0;
	}
	for (i=0; i<8; i++)
		dst->s_id16[i] = htons(tmp2[i]);
	return 1;
}

