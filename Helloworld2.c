#include <rtl.h> 		
#include <time.h>		// penso che equivalga a <rtl_time.h>, libreria per 
#include <pthread.h> 	
#include <rtl_fifo.h>

/*
LIBRERIA <thread.h>
	attributi: scheduling parameters e CPU in cui eseguire

*/

#define CONTROL_FIFO 0 	// fifo per controllare il programma
#define EVENT_FIFO 1	// fifo per salvare i messaggi
#define FIFO_BUFFER 10 // number of bytes
#define PERIOD	500000000

#define START_THREAD 'a'
#define STOP_THREAD  'b'
#define INVALID_MESSAGE 'c' // 'b'
#define THREAD_EXECUTION 'd' // 'a'


void * thread_routine(void *t){
	while(1){
		pthread_wait_np();
		rt_event_msg(THREAD_EXECUTION);
	}
	return 0;
}

void start_thread(void){
	pthread_make_periodic_np(pthread_istance, gethrtime(), PERIOD); // (thread, resume time, period)  		
	return 0;
}

void stop_thread(void){
	pthread_make_periodic_np(pthread_istance, HRTIME_INFINITY, 0);
	return 0;
}

void rt_event_msg(unsigned char message){
	rtf_put(EVENT_FIFO, &message, 1);
	return 0;
}

int rt_control_message_handler(unsigned int fifo){
	unsigned char message;
	while(rtf_get(fifo, &message, 1) > 0){// rtf_get(fifo,start byte,bytes number)
		switch(message){
			case START_THREAD:
				start_thread();
				break;
			case STOP_THREAD:
				stop_thread();
				break;	
			default:
				rt_event_msg(INVALID_MESSAGE);
		}
	}
}



int init_module(void){

	if (rtf_create(CONTROL_FIFO, FIFO_BUFFER)<0)
	{
		rtl_printf("Could not install fifo %c\n", CONTROL_FIFO);
		return -ENODEV;
	}

	if (rtf_create(EVENT_FIFO, FIFO_BUFFER)<0)
	{
		rtl_printf("Could not install fifo %c\n", EVENT_FIFO);
		return -ENODEV;
	}

	if (rtf_create_handler(CONTROL_FIFO,rt_control_message_handler))
	{
		rtl_printf("Could not install fifo handler");
		return -EINVAL;
	}

	/* pthread creation parameter */
	pthread_attr_t attr;
	struct sched_param p;
	p.sched_priority = 1;
	pthread_attr_init(&attr);
	pthread_attr_setcpu_np(&attr,0);
	//pthread_setschedparam(pthread_self(),SCHED_FIFO, &p);
	pthread_attr_setschedparam(&attr, &p);
	pthread_setfp_np(pthread_self(), 1);
	if (pthread_create(&thread_istance, &attr, thread_routine, 0))
	{
		rtl_printf("Init thread failed\n");
		return -EAGAIN;
	}
	
	rtl_printf("Init module succesfull\n");
	return 0;
}

void cleanup_module(void){
	// pthread_delete_np(thread)
	pthread_cancel(thread);		// cancella thread
	pthread_join(thread,NULL);	// dealloca memoria

	rtf_destroy(CONTROL_FIFO);
	rtf_destroy(EVENT_FIFO);

	rtl_printf("Clean up module succesfull");
}
