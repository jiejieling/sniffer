/*
 * save_pack.c
 *
 *  Created on: Apr 9, 2012
 *      Author: seven
 */

#include "mypcap.h"

void got_packet(const u_char *packet, in_addr_t mk, in_addr_t nt)
{
	u_int16_t		*type;
	struct iphdr		*ip_hd;
	//u_int32_t	sip, dip;
	key *k1, *k2, *k3, *k4;
	value *v;
	unsigned short len;

	//ethernet size :14, arp size: 8, ip size: 20, tcp size : 20, udp size: 8
	type = (u_int16_t *)(packet + 12);
	if (*type == 0x0008) { //IP == 0x0800
		ip_hd = (struct iphdr *)(packet + 14);
		len = ntohs(ip_hd->tot_len);
		/*
		if ((inet_ntop(AF_INET, &ip_hd->saddr, sip, INET_ADDRSTRLEN) == NULL) || (inet_ntop(AF_INET, &ip_hd->daddr, dip, INET_ADDRSTRLEN) == NULL)){
			perror("inet_ntop");
			return;
		}


		printf("%s --> %s\n", sip, dip);
		printf("key: %x %x --> %x\n", nt, ip_hd->saddr & mk , ip_hd->daddr & mk);
		*/
		if ((ip_hd->saddr & mk) == nt && (ip_hd->daddr & mk) != nt){
			if ((k1 = calloc(1, sizeof(key))) == NULL){
				syslog(LOG_ERR, "Calloc : %s", strerror(errno));
				return;
			}
			k1->uip = ip_hd->saddr;

			if ((k2 = calloc(1, sizeof(key))) == NULL){
				syslog(LOG_ERR, "Calloc : %s", strerror(errno));
				return;
			}
			k2->uip = ip_hd->daddr;
			out_packets_count++;
			out_bytes_count += (len + 14);

		}else if (((ip_hd->daddr & mk) == nt) && ((ip_hd->saddr & mk) != nt)){
			if ((k1 = calloc(1, sizeof(key))) == NULL){
				syslog(LOG_ERR, "Calloc : %s", strerror(errno));
				return;
			}
			k1->uip = ip_hd->daddr;

			if ((k2 = calloc(1, sizeof(key))) == NULL){
				syslog(LOG_ERR, "Calloc : %s", strerror(errno));
				return;
			}
			k2->uip = ip_hd->saddr;
			in_packets_count++;
			in_bytes_count += (len + 14);

			v = hashtable_search(h_ex_in, k2);
			if (v == NULL){
				v = calloc(1, sizeof(value));
				if ((k3 = calloc(1, sizeof(key))) == NULL){
					syslog(LOG_ERR, "Calloc : %s", strerror(errno));
					return;
                 }
				memcpy(k3, k2, sizeof(key));				
			}
			v->packets++;
			v->bytes += (len + 14);
			if (v->packets < 2)
				hashtable_insert(h_ex_in, k3, v);

			v = hashtable_search(h_in_in, k1);
			if (v == NULL){
				v = calloc(1, sizeof(value));
				if ((k4 = calloc(1, sizeof(key))) == NULL){
					syslog(LOG_ERR, "Calloc : %s", strerror(errno));
					return;
				}
				memcpy(k4, k1, sizeof(key));
			}
			v->packets++;
			v->bytes += (len + 14);
			if (v->packets < 2)
				hashtable_insert(h_in_in, k4, v);
		}else
			return;


		v = hashtable_search(h_in, k1);
		if (v == NULL){
			v = calloc(1, sizeof(value));
			v->sip = k1->uip;
			keybody = calloc(1, sizeof(keys));
			if (keybody == NULL)
				return;
			keybody->key = k1;
			keybody->n = ++in_keyhead->n;
			keybody->next = in_keyhead->next;
			keybody->head = in_keyhead;
			if (keybody->next)
				keybody->next->head = keybody;
			in_keyhead->next = keybody;
		}else
			free(k1);
		switch (ip_hd->protocol){
			case IPPROTO_ICMP:
				v->icmp++;
				break;
			case IPPROTO_TCP:
				v->tcp++;
				break;
			case IPPROTO_UDP:
				v->udp++;
				break;
			default:
				v->other++;
				break;
		}

		v->packets++;
		v->bytes += (len + 14);
		if (v->packets < 2)
			hashtable_insert(h_in, k1, v);

		v = hashtable_search(h_ex, k2);
		if (v == NULL){
			v = calloc(1, sizeof(value));
			v->sip = k2->uip;
			keybody = calloc(1, sizeof(keys));
			if (keybody == NULL)
				return;
			keybody->key = k2;
			keybody->n = ++ex_keyhead->n;
			keybody->next = ex_keyhead->next;
			keybody->head = ex_keyhead;
			if (keybody->next)
				keybody->next->head = keybody;
			ex_keyhead->next = keybody;
		}else
			free(k2);
		switch (ip_hd->protocol){
			case IPPROTO_ICMP:
				v->icmp++;
				break;
			case IPPROTO_TCP:
				v->tcp++;
				break;
			case IPPROTO_UDP:
				v->udp++;
				break;
			default:
				v->other++;
				break;
		}
		v->packets++;
		v->bytes += (len + 14);
		if (v->packets < 2)
			hashtable_insert(h_ex, k2, v);
			
	}
		//printf("IP: %s ----> %s\ttype:%x len:%i\n", sip, dip, ip_hd->protocol, ntohs(ip_hd->tot_len));
		//protocol <netinet/in.h>
}
