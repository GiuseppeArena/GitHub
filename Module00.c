#include <rtl.h> 		
#include <time.h>		// penso che equivalga a <rtl_time.h>, libreria per 
#include <pthread.h> 	
#include <rtl_fifo.h>

/*
LIBRERIA <thread.h>
	attributi: scheduling parameters e CPU in cui eseguire

*/

#define SHARING_FIFO 0 	// fifo per controllare il programma
#define FIFO_BUFFER 10 // number of bytes
#define PERIOD1	500000000
#define PERIOD2	500000000


pthread_t ptask1;
pthread_t ptask2;
pthread_t task3;


void * ptask1_thread(void *t){
	pthread_make_periodic_np(pthread_self(), gethrtime(), PERIOD1);

	static int j=0;
	int j1=0;
	static char msg = 'a';
	
	while(1){
		pthread_wait_np();
		/* some code */
		j++;
		rtl_printf("Running task1, %d, %d\n", j, j1);
		msg = char(j);
		rtl_printf("written message is %c\n", msg);
		if(rtf_put(SHARING_FIFO, &msg, 1)<0){
			rtl_printf("error in writing fifo\n");
		}
	}

	return 0;
}

void * ptask2_thread(void *t){
	pthread_make_periodic_np(pthread_self(), gethrtime(), PERIOD2);

	static int i=0;
	static char msg = 'b';
	
	while(1){
		pthread_wait_np();
		/* some code */
		i++;
		rtl_printf("Running task2, %d\n", i);
		msg = rtf_get(SHARING_FIFO, &msg, 1);
		rtl_printf("%c\n", msg);
	}

	return 0;
}

void * task3_thread(void *t){
	pthread_make_periodic_np(pthread_self(), gethrtime(), 0);

	/* some code*/
	
	return 0;
}

/* Interrupt code */


int init_module(void){

	if (rtf_create(SHARING_FIFO, FIFO_BUFFER)<0)
	{
		rtl_printf("Could not install fifo %c\n", CONTROL_FIFO);
		return -ENODEV;
	}

	/* pthread creation parameter */
	/* ptask1 parameter */
	pthread_attr_t attr1;
	struct sched_param p1;
	p1.sched_priority = 1;
	pthread_attr_init(&attr1);
	pthread_attr_setcpu_np(&attr1,0);
	//pthread_setschedparam(pthread_self(),SCHED_FIFO, &p);
	pthread_attr_setschedparam(&attr1, &p1);
	//pthread_setfp_np(pthread_self(), 1);

	/* ptask2 parameter */
	pthread_attr_t attr2;
	struct sched_param p2;
	p2.sched_priority = 1;
	pthread_attr_init(&attr2);
	pthread_attr_setcpu_np(&attr2,0);
	//pthread_setschedparam(pthread_self(),SCHED_FIFO, &p);
	pthread_attr_setschedparam(&attr2, &p2);
	//pthread_setfp_np(pthread_self(), 1);

	if (pthread_create(&ptask1, &attr1, ptask1_thread, 0))
	{
		rtl_printf("Init ptask1 thread failed\n");
		return -EAGAIN;
	}
	
	if (pthread_create(&ptask2, &attr,2 ptask2_thread, 0))
	{
		rtl_printf("Init ptask2 thread failed\n");
		return -EAGAIN;
	}

	rtl_printf("Init module succesfull\n");
	return 0;
}

void cleanup_module(void){
	// pthread_delete_np(thread)
	pthread_cancel(ptask1);		// cancella thread
	pthread_join(ptask1,NULL);	// dealloca memoria

	pthread_cancel(ptask2);		// cancella thread
	pthread_join(ptask2,NULL);	// dealloca memoria

	rtf_destroy(SHARING_FIFO);

	rtl_printf("Clean up module succesfull");
	return 0;
}
