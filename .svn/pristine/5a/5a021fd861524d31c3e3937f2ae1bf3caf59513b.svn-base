/*
 * mypcap.h
 *
 *  Created on: Apr 9, 2012
 *      Author: seven
 */

#ifndef MYPCAP_H_
#define MYPCAP_H_

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include <errno.h>
#include <sys/syslog.h>
#include <fcntl.h>

#define PORT 4100
#define IP "115.182.1.230"
#define BUF_SIZE 1024

typedef struct ifeth{
	char	*name;
	in_addr_t ad;
	in_addr_t mk;
	in_addr_t nt;
	struct ifeth *next;
}ifeth;

typedef struct pack{
	u_char *packet;
	in_addr_t mk;
	in_addr_t nt;
	struct pack *next;
}pk;

typedef struct mystack {
	short pos;
	pk* buf[BUF_SIZE];
}mystack;

sigset_t mask;
pthread_mutex_t f_lock;
pthread_mutex_t buf_lock;
pthread_attr_t attr;
mystack buff;

typedef struct key{
	unsigned int uip;
}key;

typedef struct keys{
	key *key;
	long n;
	struct keys *head;
	struct keys *next;
}keys;

typedef struct value{
	u_int32_t	sip;
	unsigned int packets;
	unsigned int tcp;
	unsigned int udp;
	unsigned int icmp;
	unsigned int other;
	unsigned long bytes;
}value;

typedef struct {
	value v;
	unsigned int fpacket;
	unsigned long fbytes;
}xvalue;

struct hashtable *h_ex, *h_ex_in, *h_in, *h_in_in, *ptable;
keys *ex_keyhead, *in_keyhead, *keybody;
long int heapsize;
unsigned long in_bytes_count,  out_bytes_count;
unsigned int in_packets_count, out_packets_count;

// flag = 0  src net is localnet and dst net not is localnet,flag = 1 src net not is localnet and dst net is localnet
void got_packet(const u_char *packet, in_addr_t mk, in_addr_t nt);
void handler(struct hashtable *h_ex_old, struct hashtable *h_ex_in_old,
			 struct hashtable *h_in_old, struct hashtable *h_in_in_old,
			 keys *ex_keyhead_old, keys *in_keyhead_old,
			 u_long in_bytes_count,  u_int in_packets_count, u_long out_bytes_count, u_int out_packets_count);

keys *heapsort(struct hashtable*, keys*, int);
//keys *BubbleSort(struct hashtable *table, keys *key);;

#endif /* MYPCAP_H_ */

static unsigned int hashfromkey(void *ky)
{
	key *k = (key *)ky;
	return (((k->uip << 17) | (k->uip >> 15)) ^ k->uip) + ((k->uip >> 15) * 17);
}

static int equalkeys(void *k1, void *k2)
{
	return (0 == memcmp(k1,k2,sizeof(key)));
}
