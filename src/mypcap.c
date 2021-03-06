/*
 * mypcap.c
 *
 *  Created on: Apr 9, 2012
 *      Author: seven
 */

#include "mypcap.h"

void *th_work(void *arg)
{
	pk *pack, *unused;
	for(;;)
	{
		//pack = (pk *)arg;
		pack = NULL;
		pthread_mutex_lock(&buf_lock);
		if (buff.pos > 0)
			pack = buff.buf[--buff.pos];
		pthread_mutex_unlock(&buf_lock);
		if (pack != NULL)
		{
			in_addr_t mk = pack->mk, nt = pack->nt;
			for (pack = pack->next; pack; free(unused))
			{
				pthread_mutex_lock(&f_lock);
				got_packet(pack->packet, mk, nt);
				pthread_mutex_unlock(&f_lock);
				unused = pack;
				pack = pack->next;
			}
			free(pack);
		}else
			usleep(10000);
			//	free(arg);
	}
}

void thread_clean(void *handle)
{
	if (handle)
		pcap_close(handle);
}

void *th_eth(void *arg)
// handle thread for every interface
{

	ifeth *eth = (ifeth *)arg;
	char ip[16], mask[16], net[16];
	pk *phead, *pbody;
	pcap_t *handle;
	struct pcap_pkthdr *packhdr;
	struct bpf_program filter;
	char filter_app[45];	// strlen("net 111.111.111.111 mask 255.255.255.255") + 1;
	char errbuf[PCAP_ERRBUF_SIZE];
	int ret;


	if ((inet_ntop(AF_INET, &eth->ad, ip, sizeof(ip)) == NULL) ||
	(inet_ntop(AF_INET, &eth->mk, mask, sizeof(mask)) == NULL) ||
	(inet_ntop(AF_INET, &eth->nt, net, sizeof(net)) == NULL)){
			syslog(LOG_ERR, "inet_ntop: %s", strerror(errno));
			pthread_exit((void *)1);
	}

	syslog(LOG_NOTICE, "dev: %s address: %s net: %s netmask: %s",
			eth->name,
			ip,
			net,
			mask
			);

	char tmp[4];
	memcpy(tmp, ip, 3);
	tmp[3] = '\0';

	if ((strcmp("10.", tmp) == 0) || (strcmp("192", tmp) == 0) || (strcmp("172", tmp) == 0)){
		syslog(LOG_NOTICE, "%s is internal IP, exit", ip);
		pthread_exit(NULL);
	}

	if ((handle = pcap_open_live(eth->name, 65535, 0/*0：close promisc, 1:open promisc*/, 0, errbuf)) == NULL){
		syslog(LOG_ERR, "Pcap_open_live err: %s", errbuf);
		pthread_exit((void *)1);
	}

	pthread_cleanup_push(thread_clean, handle);

	/*
	//"src net %s mask %s and not dst net %s mask %s"
	snprintf(filter_app, sizeof(filter_app), "net %s mask %s", net, mask);
	if (pcap_compile(handle, &filter, filter_app, 0, eth->mk) == -1){
		syslog(LOG_ERR, "Couldn't parse filter: %s", pcap_geterr(handle));
		pthread_exit((void *)1);
	}

	if (pcap_setfilter(handle, &filter) == -1){
		syslog(LOG_ERR, "Couldn't install filter: %s", pcap_geterr(handle));
		pthread_exit((void *)1);
	}
	*/
	int n;
	for (;;)
	{
		phead = calloc(1, sizeof(pk));
		phead->mk = eth->mk;
		phead->nt = eth->nt;
		for (n = 0; n < 10000; n++){
			pbody = calloc(1, sizeof(pk));
			if ((ret = pcap_next_ex(handle, &packhdr, (const u_char **)&pbody->packet)) < 0){
				syslog(LOG_ERR, "Can't get packet: %s", pcap_geterr(handle));
				free(pbody);
				pthread_exit((void *)1);
			}

			if (ret == 0){
				free(pbody);
				continue;
			}

			pbody->next = phead->next;
			phead->next = pbody;

		}

		pthread_mutex_lock(&buf_lock);
		if (buff.pos >= (BUF_SIZE - 1))
			syslog(LOG_ERR, "Pos too large, %i", buff.pos);
		else
			buff.buf[buff.pos++] = phead;
		pthread_mutex_unlock(&buf_lock);

		/*
		if ((perr = pthread_create(&tid, &attr, th_work, phead)) != 0){
			syslog(LOG_ERR, "Can't create work pthread: %s", strerror(perr));
			for (; phead; free(pbody))
			{
			    pbody = phead;
			    phead = phead->next;
			}
			pthread_exit((void *)1);
		}
		*/
	}

	pthread_cleanup_pop(0);
}


void th_sig(void *arg)
{
	for(;;)
	{
		int sig;
		if (sigwait(&mask, &sig) != 0){
			syslog(LOG_ERR, "wait signal error");
			exit(1);
		}

		switch (sig){
		case SIGTERM:
			syslog(LOG_NOTICE, "Recv signal term, proc will exit...");
			exit(0);

		case SIGINT:
			syslog(LOG_NOTICE, "Ctrl + C, proc will exit...");
			exit(0);

		case SIGALRM:
			pthread_mutex_lock(&f_lock);

			struct hashtable *h_ex_old, *h_ex_in_old, *h_in_old, *h_in_in_old;
			keys *ex_keyhead_old, *in_keyhead_old;
			unsigned long in_bytes_count_old,  out_bytes_count_old;
			unsigned int in_packets_count_old, out_packets_count_old;

			h_ex_old = h_ex; h_ex_in_old = h_ex_in; h_in_old = h_in; h_in_in_old = h_in_in;
			ex_keyhead_old = ex_keyhead; in_keyhead_old = in_keyhead;

			in_bytes_count_old = in_bytes_count;
			in_packets_count_old = in_packets_count;
			out_bytes_count_old = out_bytes_count;
			out_packets_count_old = out_packets_count;

			in_bytes_count = 0; in_packets_count = 0;
			out_bytes_count = 0; out_packets_count = 0;

			h_ex = create_hashtable(16, hashfromkey, equalkeys);
			h_ex_in = create_hashtable(16, hashfromkey, equalkeys);
			h_in = create_hashtable(16, hashfromkey, equalkeys);
			h_in_in = create_hashtable(16, hashfromkey, equalkeys);

			if ((h_ex == NULL) || (h_ex_in == NULL) || (h_in == NULL) || (h_in_in == NULL)){
				exit(-1);
			}

			ex_keyhead = calloc(1, sizeof(keys));
			in_keyhead = calloc(1, sizeof(keys));

			if ((ex_keyhead == NULL) || (in_keyhead == NULL )){
				syslog(LOG_ERR, "Calloc : %s", strerror(errno));
				exit(1);
			}

			pthread_mutex_unlock(&f_lock);
			alarm(300);

			handler(h_ex_old, h_ex_in_old,  h_in_old, h_in_in_old,
					ex_keyhead_old, in_keyhead_old,
					in_bytes_count_old, in_packets_count_old,
					out_bytes_count_old, out_packets_count_old);

			hashtable_destroy(h_ex_old, 1); hashtable_destroy(h_ex_in_old, 1);
			hashtable_destroy(h_in_old, 1); hashtable_destroy(h_in_in_old, 1);

			syslog(LOG_NOTICE, "in_bytes:%lu in_packets:%u out_bytes:%lu out_packets:%u", in_bytes_count_old, in_packets_count_old, out_bytes_count_old, out_packets_count_old);

			keys *ukey;
			for (; ex_keyhead_old; free(ukey))
			{
				ukey = ex_keyhead_old;
				ex_keyhead_old = ex_keyhead_old->next;
			}

			for (; in_keyhead_old; free(ukey))
			{
				ukey = in_keyhead_old;
				in_keyhead_old = in_keyhead_old->next;
			}
			break;

		default:
			printf("Recv signum = %i\n", sig);
			break;
		}
	}

}
void myexit(void)
{
	pthread_mutex_destroy(&f_lock);
	pthread_attr_destroy(&attr);
}

void Daemon(void)
{
	pid_t mypid1, mypid2;
	u_short n = 0;

	openlog("sniffer",LOG_PID, LOG_LOCAL7);
	umask(0);
	if ((mypid1 = fork()) == -1){
		syslog(LOG_ERR, "fork: %s", strerror(errno));
		exit(1);
	}
	if (mypid1 > 0)
		exit(0);

	setsid();

	signal(SIGHUP, SIG_IGN);
	if ((mypid2 = fork()) == -1){
		syslog(LOG_ERR, "fork: %s", strerror(errno));
		exit(1);
	}

	if (mypid2 > 0)
		exit(0);

	chdir("/");

	for(; n < 1025; n++)
		close(n);

	open("/dev/null", O_RDWR);
	dup(0);
	dup(0);

}

int main(int argc, char *argv[])
{
//	Daemon();
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *devs, *alldevs;
	pcap_addr_t *i;
	struct sockaddr_in *ad, *mk;
	ifeth *eths, *body;
	pthread_t ehltid[24], sigtid, workid[10];
	u_short pindex = 0, ptail;
	int perr = 0, num;
	buff.pos = 0;

	pthread_mutex_init(&f_lock, NULL);
	pthread_mutex_init(&buf_lock, NULL);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	atexit(myexit);

	sigfillset(&mask);
	if ((perr = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0 ){
		syslog(LOG_ERR, "pthread_sigmask err :%s", strerror(perr));
		exit(1);
	}

	if ((perr = pthread_create(&sigtid, &attr, (void *)th_sig, NULL)) != 0){
		syslog(LOG_ERR, "pthread_create err: %s", strerror(perr));
		exit(1);
	}

	in_bytes_count = 0; in_packets_count = 0;
	out_bytes_count = 0; out_packets_count = 0;

	h_ex = create_hashtable(16, hashfromkey, equalkeys);
	h_ex_in = create_hashtable(16, hashfromkey, equalkeys);
	h_in = create_hashtable(16, hashfromkey, equalkeys);
	h_in_in = create_hashtable(16, hashfromkey, equalkeys);

	if ((h_ex == NULL) || (h_ex_in == NULL) || (h_in == NULL) || (h_in_in == NULL)){
		syslog(LOG_ERR, "create hashtable error");
		exit(-1);
	}

	ex_keyhead = calloc(1, sizeof(keys));
	in_keyhead = calloc(1, sizeof(keys));

	if ((ex_keyhead == NULL) || (in_keyhead == NULL )){
		syslog(LOG_ERR, "Calloc : %s", strerror(errno));
		exit(1);
	}

	if ((eths = alloca(sizeof(ifeth))) < 0){
		syslog(LOG_ERR, "Alloca : %s", strerror(errno));
		exit(1);
	}
	memset(eths, 0, sizeof(ifeth));

	if (pcap_findalldevs(&alldevs, errbuf) < 0){
		syslog(LOG_ERR, "pcap_findalldevs error: %s", errbuf);
		exit(1);
	}

	for (devs = alldevs; devs; devs = devs->next)
		for (i = devs->addresses; i; i = i->next)
		{
			ad = (struct sockaddr_in *)i->addr;
			mk = (struct sockaddr_in *)i->netmask;
			if (ad->sin_addr.s_addr <  0x1000001)	//0x01000001 == ip 1.0.0.1
				continue;

			if ((body = alloca(sizeof(ifeth))) < 0){
				syslog(LOG_ERR, "Alloca : %s", strerror(errno));
				exit(1);
			}

			body->name = devs->name;
			body->ad = ad->sin_addr.s_addr;
			body->mk = mk->sin_addr.s_addr;
			body->nt = body->ad & body->mk;
			body->next = eths->next;
			eths->next = body;
		}

	for (eths = eths->next; eths; eths = eths->next)
		if (strcmp(eths->name, "lo") && strcmp(eths->name, "virbr0") && strcmp(eths->name, "eth0")){
			perr = pthread_create(&ehltid[pindex++], NULL, th_eth, eths);
			if (perr != 0 ){
				syslog(LOG_ERR, "Create thread err: %s", strerror(perr));
				exit(1);
			}
			sleep(1);
		}

	for (num = 0; num < 10; num++){
		perr = pthread_create(&workid[num], NULL, th_work, NULL);
		if (perr != 0 ){
			syslog(LOG_ERR, "Create thread err: %s", strerror(perr));
			exit(1);
		}
	}

	alarm(300);

	for(ptail = 0; ptail < pindex; ptail++)
		if ((perr = pthread_join(ehltid[ptail], NULL)) != 0){
			syslog(LOG_ERR, "pthread_join err: %s", strerror(perr));
			continue;
		}

	closelog();
	return 0;
 }
