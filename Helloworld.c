#include <rtl.h> 		
#include <time.h>		// penso che equivalga a <rtl_time.h>, libreria per 
#include <pthread.h> 	// libreria per generare thread


/*
LIBRERIA <thread.h>
	attributi: scheduling parameters e CPU in cui eseguire

*/


pthread_t pthread;

void * start_routine(void *arg){
	struct sched_param p;
	p.sched_priority = 1;
	pthread_setschedparam(pthread_self(),SCHED_FIFO, &p);	// SCHED_FIFO ??
	pthread_make_periodic_np(pthread_self(), gethrtime(),500000000); // (thread, timer_period, timer_expiration)
																	 // ma (pthread,clock_time, ??)  		
	while(1){
		pthread_wait_np();
		rtl_printf("I'm here; my arg is %x\n",(unsigned) arg);
	}
	return 0;
}

int init_module(void){
	/*

	*/
	return pthread_create(&thread,NULL,start_routine,0); //(*pthread,*attr,*routine,*arg)
}

void cleanup_module(void){
	pthread_cancel(thread);		// cancella thread
	pthread_join(thread,NULL)	// dealloca memoria
}


