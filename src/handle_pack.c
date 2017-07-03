/*
 * handble_pack.c
 *
 *  Created on: Apr 9, 2012
 *      Author: seven
 */

#include "mypcap.h"

void handler(struct hashtable *h_ex, struct hashtable *h_ex_in,
		 	 struct hashtable *h_in, struct hashtable *h_in_in,
		 	 keys *ex_keyhead, keys *in_keyhead,
		 	 u_long in_bytes_count,  u_int in_packets_count, u_long out_bytes_count, u_int out_packets_count)
{
	struct hashtable *h_ex_old = h_ex, *h_ex_in_old = h_ex_in;
	struct hashtable *h_in_old = h_in, *h_in_in_old = h_in_in;
	keys *ex_keyhead_old = ex_keyhead, *in_keyhead_old = in_keyhead;
	keys *ex_ret, *ex_rethead, *in_ret, *in_rethead;
	value *v;
	xvalue vs[1025];
	int sock;
	int i;
	struct sockaddr_in svraddr;
//	char sip[INET_ADDRSTRLEN], dip[INET_ADDRSTRLEN];

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		syslog(LOG_ERR, "Socket: %s", strerror(errno));
		exit(1);
	}

	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(PORT);
	if(inet_pton(AF_INET, IP, &svraddr.sin_addr) < 0){
		syslog(LOG_ERR, "inet_pton: %s", strerror(errno));
		exit(1);
	}

	if (connect(sock, (const struct sockaddr *)&svraddr, sizeof(svraddr)) < 0){
		syslog(LOG_ERR, "connect: %s", strerror(errno));
		close(sock);
		return;
	}

	memset(&vs[0], 0, sizeof(xvalue));
	vs[0].v.other = 0;
	vs[0].fbytes = out_bytes_count;
	vs[0].fpacket = out_packets_count;
	if (ex_keyhead_old->next){
		if (!ex_keyhead_old->next->next){
			v = hashtable_search(h_ex_old, ex_keyhead_old->next->key);
			vs[1].v = *v;

			if ((v = hashtable_search(h_ex_in_old, ex_keyhead_old->next->key)) == NULL){
				vs[1].fbytes = 0;
				vs[1].fpacket = 0;
			}else{
				vs[1].fbytes = v->bytes;
				vs[1].fpacket = v->packets;
			}

			write(sock, vs, sizeof(xvalue) * 2);
			/*
			if ((inet_ntop(AF_INET, &vs[1].v.sip, sip, INET_ADDRSTRLEN) == NULL) || (inet_ntop(AF_INET, &vs[1].v.dip, dip, INET_ADDRSTRLEN) == NULL)){
				syslog(LOG_ERR, "inet_ntop: %s", strerror(errno));
				return;
			}
			printf("Ex_in   %s --> %s, bytes: %i, packets: %i\n", sip, dip, vs[1].v.bytes, vs[1].v.packets);
			*/
		}else{
			if((ex_ret = heapsort(h_ex_old, ex_keyhead_old, 1)) == NULL)
				syslog(LOG_ERR, "heapSort error");
			else{
				ex_rethead = ex_ret;
				i = 1;
				for(ex_ret = ex_ret->next; ex_ret; ex_ret = ex_ret->next)
				{
					v = hashtable_search(h_ex_old, ex_ret->key);
					vs[i].v = *v;

					if ((v = hashtable_search(h_ex_in_old, ex_ret->key)) == NULL){
						vs[i].fbytes = 0;
						vs[i].fpacket = 0;
					}else{
						vs[i].fbytes = v->bytes;
						vs[i].fpacket = v->packets;
					}

					/*
					if ((inet_ntop(AF_INET, &vs[i].v.sip, sip, INET_ADDRSTRLEN) == NULL) || (inet_ntop(AF_INET, &vs[i].v.dip, dip, INET_ADDRSTRLEN) == NULL)){
						syslog(LOG_ERR, "inet_ntop: %s", strerror(errno));
						return;
					}
					printf("Ex_in  %s --> %s, bytes: %i, packets: %i\n", sip, dip, vs[i].v.bytes, vs[i].v.packets);
					*/
					i++;
				}
				write(sock, vs, sizeof(xvalue) * i);
				for(; ex_rethead; free(ex_ret))
				{
                    ex_ret = ex_rethead;
                    ex_rethead = ex_rethead->next;
				}


			}
		}
	}else
		write(sock, vs, sizeof(xvalue));
	close(sock);

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		syslog(LOG_ERR, "Socket: %s", strerror(errno));
		exit(1);
	}
	if (connect(sock, (const struct sockaddr *)&svraddr, sizeof(svraddr)) < 0){
		syslog(LOG_ERR, "connect: %s", strerror(errno));
		close(sock);
		return;
	}
	memset(&vs[0], 0, sizeof(xvalue));
	vs[0].v.other = 1;
	vs[0].fbytes = in_bytes_count;
	vs[0].fpacket = in_packets_count;
	if (in_keyhead_old->next){
		if (!in_keyhead_old->next->next){
			v = hashtable_search(h_in_old, in_keyhead_old->next->key);
			vs[1].v = *v;

			if ((v = hashtable_search(h_in_in_old, in_keyhead_old->next->key)) == NULL){
				vs[1].fbytes = 0;
				vs[1].fpacket = 0;
			}else{
				vs[1].fbytes = v->bytes;
				vs[1].fpacket = v->packets;
			}

			write(sock, vs, sizeof(xvalue) * 2);
			/*
			if ((inet_ntop(AF_INET, &v->sip, sip, INET_ADDRSTRLEN) == NULL) || (inet_ntop(AF_INET, &v->dip, dip, INET_ADDRSTRLEN) == NULL)){
				syslog(LOG_ERR, "inet_ntop: %s", strerror(errno));
				return;
			}
			printf("In   %s --> %s, bytes: %i, packets: %i\n", sip, dip, v->bytes, v->packets);
			*/
		}else{
			if((in_ret = heapsort(h_in_old, in_keyhead_old, 0)) == NULL)
				syslog(LOG_ERR, "heapSort error");
			else{
				in_rethead = in_ret;
				i = 1;
				for(in_ret = in_ret->next; in_ret ; in_ret  = in_ret->next)
				{
					v = hashtable_search(h_in_old, in_ret ->key);
					vs[i].v = *v;

					if ((v = hashtable_search(h_in_in_old, in_ret->key)) == NULL){
						vs[i].fbytes = 0;
						vs[i].fpacket = 0;
					}else{
						vs[i].fbytes = v->bytes;
						vs[i].fpacket = v->packets;
					}
					i++;
					/*
					if ((inet_ntop(AF_INET, &v->sip, sip, INET_ADDRSTRLEN) == NULL) || (inet_ntop(AF_INET, &v->dip, dip, INET_ADDRSTRLEN) == NULL)){
						syslog(LOG_ERR, "inet_ntop: %s", strerror(errno));
						return;
					}
					printf("In  %s --> %s, bytes: %i, packets: %i\n", sip, dip, v->bytes, v->packets);
					*/
				}
				write(sock, vs, sizeof(xvalue) * i);
				for(; in_rethead; free(in_ret))
				{
                    in_ret = in_rethead;
                    in_rethead = in_rethead->next;
				}

			}
		}
	}else
		write(sock, vs, sizeof(xvalue));

	close(sock);


}
