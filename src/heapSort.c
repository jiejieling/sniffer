/*
 * heapSort.c
 *
 *  Created on: Apr 26, 2012
 *      Author: seven
 */

#include "mypcap.h"


void exchange(keys *a,keys *b)
{
	keys tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

int search(keys key)
{
	value *v;
	v = hashtable_search(ptable, key.key);
	return v->bytes;
}
int left(int i)
{
    return i << 1;
}

int rigth(int i)
{
    return (i << 1) + 1;
}

void max_heapify(keys a[], int i)
{
    int l = left(i);
    int r = rigth(i);
    int largest;

    if (l <= heapsize && search(a[l]) > search(a[i]))
        largest = l;
    else
        largest = i;

    if (r <= heapsize && search(a[r]) > search(a[largest]))
        largest = r;

    if (largest != i)
    {
        exchange(&a[i], &a[largest]);
        max_heapify(a, largest);
    }
}

void build_max_heap(keys a[], int n)
{
    int i;
 //   heapsize = n;
    for (i = n / 2; i >= 1; i--)
        max_heapify(a, i);
}

keys* heapsort(struct hashtable *table, keys *key, int flag)
{
	heapsize = key->n;
	int num;
	if (flag == 0)
		num = 1024;
	else
		num = 20;

    keys *rethead1, *rethead2, *retbody;
	if (key->n < num)
        num = key->n;

	keys a[heapsize + 1];
	ptable = table;
//	value *v;

    if((rethead1 = calloc(1, sizeof(keys))) == NULL)
        return NULL;

    rethead2 = rethead1;

    int i = 0;
	for (; key; key = key->next)
		a[i++] = *key;

	build_max_heap(a, heapsize);

	for (; heapsize > (a[0].n - num);)
    {
        if (heapsize != 1)
            exchange(&a[1], &a[heapsize]);
        /*
        v = hashtable_search(ptable, a[heapsize].key);
        printf("heapsize: %i n: %i %s --> %s bytes: %i, packets: %i\n", heapsize, a[heapsize].n, v->sip, v->dip, v->bytes, v->packets);
        */
        if ((retbody = calloc(1, sizeof(keys))) == NULL){
            for(; rethead1; free(retbody))
            {
                retbody = rethead1;
                rethead1 = rethead1->head;
                return NULL;
            }
        }

        *retbody = a[heapsize];
        retbody->next = NULL;
        rethead2->next = retbody;
        rethead2 = rethead2->next;
        --heapsize;
        max_heapify(a, 1);
    }

	return rethead1;

}


