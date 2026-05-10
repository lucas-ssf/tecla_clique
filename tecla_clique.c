#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h> //usleep

#define N 5
#define mover_mouse(x,y) XTestFakeMotionEvent(display, -1, x, y, CurrentTime)
#define simbolocodigo(x) XKeysymToKeycode(display, XStringToKeysym(#x))
#define _X 422
#define _Y 670

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


typedef struct{
	KeyCode codigo;
	int x, y;
	int pressionada;
	bool down;
	pthread_t t;
}tecla_t;

void* clique(void*arg){
	Display *display = XOpenDisplay(NULL);
	Window root = DefaultRootWindow(display);
	tecla_t*tecla = (tecla_t*)arg;

	char keys[32];

	while (1){
		XQueryKeymap(display, keys);
				tecla->pressionada = keys[tecla->codigo / 8] & (1 << (tecla->codigo % 8));
			
			if (tecla->pressionada && !tecla->down) {
			 //   pthread_mutex_lock(&mutex);
			    	mover_mouse(tecla->x,tecla->y);
			    	XTestFakeButtonEvent(display, 1, True, CurrentTime);
			 //   pthread_mutex_unlock(&mutex);
			    tecla->down = true;
			    XFlush(display);
			}
			
			if (!tecla->pressionada && tecla->down) {
			    XTestFakeButtonEvent(display, 1, False, CurrentTime);
			    tecla->down = false;
			    XFlush(display);
			}
		
		usleep(5e3);
	}
}


int main(){
	Display *display = XOpenDisplay(NULL);
	Window root = DefaultRootWindow(display);
	
	tecla_t teclas[] ={
		{.codigo=XKeysymToKeycode(display, XK_Up),    .x=_X+10, .y=_Y,          .down=false},
		{.codigo=XKeysymToKeycode(display, XK_Down),  .x=_X,    .y=_Y+85,       .down=false},
		{.codigo=XKeysymToKeycode(display, XK_Left),  .x=_X-40, .y=(2*_Y+85)/2, .down=false},
		{.codigo=XKeysymToKeycode(display, XK_Right), .x=_X+40, .y=(2*_Y+85)/2, .down=false},
		{.codigo=simbolocodigo(c),                    .x=1162,  .y=(2*_Y+85)/2, .down=false},
	};


	for(int i = 0; i < N; i++){
		pthread_create(&teclas[i].t,NULL,clique,&teclas[i]);
	}

	for(int i = 0; i < N; i++){
		pthread_join(teclas[i].t,NULL);
	}

	return 0;
}
