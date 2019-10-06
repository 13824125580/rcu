#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "list.h"
 
#define  NUM_THREADS     20

struct list_head_obj list_array[100];
void *rcu_list_process(void *args)
{
    struct list_head_obj *pos, *n;
    while(1)
    {
    	list_for_each_entry_safe(pos, n, &list_array[0].list, list)
    	{
		printf("%d. pos->val = %d.\n", (int)args, pos->val);
    	}
    }
    return NULL;
}
 
void *rcu_list_writer(void *args)
{
    while(1)
    {
	list_del_rcu(&list_array[88].list);
	printf("del and add\n");
	pthread_yield();
	list_add_rcu(&list_array[88].list, &list_array[87].list);
    }
    return NULL;
}
int main(void)
{
    unsigned int rc,t;
    pthread_t thread[NUM_THREADS];
    int i = 0;

    for(i = 0; i < 100; i ++)
    {
	INIT_LIST_HEAD(&list_array[i].list);
	list_array[i].val = i;
    }

    for(i = 1; i < 100; i ++)
    {
	list_add_tail(&list_array[i].list, &list_array[0].list)	;
    }

    for( t = 0; t < NUM_THREADS; t++)
    {
        printf("Creating thread %d\n", t);
        rc = pthread_create(&thread[t], NULL, rcu_list_process, (void*)t);
        if (rc)
        {
            printf("ERROR; return code is %d\n", rc);
            return EXIT_FAILURE;
        }
    }

    pthread_t writer;
    rc = pthread_create(&writer, NULL, rcu_list_writer, NULL);
    if (rc)
    {
        printf("ERROR; return code is %d\n", rc);
        return EXIT_FAILURE;
    }

    for( t = 0; t < NUM_THREADS; t++)
        pthread_join(thread[t], NULL);
    return EXIT_SUCCESS;
}
