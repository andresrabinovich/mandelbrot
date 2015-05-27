#include <stdio.h>
#include <math.h>
#include "SDL/SDL.h"
#include "SDL/SDL_gfxPrimitives.h"
#include "SDL/SDL_thread.h"

#define WIDTH 1024
#define HEIGHT 768
#define BPP 4
#define DEPTH 32

int mouse_x, mouse_y;
//SDL_SpinLock gDataLock = 0;

//int threadFunction( void* data ) { 
//	//Print incoming data 
//	printf( "Running thread with value = %d\n", (int)data ); 
//	return 0;
//}

typedef struct{
	long double real;
	long double imaginario;
}complejo;

double mod_cuadrado(complejo z){
	return (z.real*z.real + z.imaginario*z.imaginario);
}

complejo sumar_complejos(complejo a, complejo b){
	complejo c = {a.real+b.real, a.imaginario+b.imaginario}; 
	return c;
}

complejo multiplicar_complejos(complejo a, complejo b){
	complejo c = {(a.real*b.real-a.imaginario*b.imaginario), (a.real*b.imaginario+b.real*a.imaginario)};
	return c;
}

int comparar_complejos(complejo a, complejo b){
	if(a.real != b.real || a.imaginario != b.imaginario){
		return 0;
	}else{
		return 1;
	}
}

void imprimir_complejo(complejo a){
	printf("%Lf+%Lfi\n", a.real, a.imaginario);
}

void generar_mapa_de_colores(SDL_Surface *screen, Uint32 colores[], int rango){  
	Uint8 r, g, b;
	int i;
	double frecuencia = (double)15/rango;
	frecuencia = 0.3;
	for(i = 0; i < (rango-1); i++){
		r = (Uint8)round(sin(frecuencia*i + 0) * 127) + 128;
		g = (Uint8)round(sin(frecuencia*i + 2) * 127) + 128;
		b = (Uint8)round(sin(frecuencia*i + 4) * 127) + 128;
		colores[i] = SDL_MapRGB(screen->format, r, g, b);
	}
	colores[rango] = SDL_MapRGB(screen->format, 0, 0, 0);
}

int sucesion(complejo c, double r_max, int control){
	int paso = 0;
	complejo z_n = c;
	complejo z_n_mas_uno = {0, 0};
	double r_max_cuadrado = r_max*r_max;
	while(mod_cuadrado(z_n_mas_uno) < r_max_cuadrado && paso < control && !comparar_complejos(z_n_mas_uno, z_n)){
		z_n = z_n_mas_uno;
		z_n_mas_uno = sumar_complejos(multiplicar_complejos(z_n, z_n), c);
		//printf("%d\t%f\n", paso, mod(z_n_mas_uno));
		//imprimir_complejo(c);
		paso++;
	}
	if(mod_cuadrado(z_n_mas_uno) < r_max_cuadrado) paso = control;
	return paso;
}

void mandelbrot(long double x_minimo, long double x_maximo, long double y_minimo, long double y_maximo, int n[], int tolerancia, long double paso_parte_real, long double paso_parte_imaginaria){
	complejo z;
	int x, y;
	long int punto = 0;

	z.real = x_minimo;
	z.imaginario = y_maximo;
	for(y = HEIGHT; y > 0; y--){
		for(x = 0; x < WIDTH; x++){
			n[punto] = sucesion(z, 2, tolerancia);		
			punto++;
			z.real = z.real + paso_parte_real;
		
		}
		z.real = x_minimo;
		z.imaginario = z.imaginario - paso_parte_imaginaria;
	}
}

void set_pixel(SDL_Surface *screen, int x, int y, Uint32 colour){
    Uint32 *pixmem32;

    pixmem32 = (Uint32*) screen->pixels + y + x;
    *pixmem32 = colour;
}


void draw_screen(SDL_Surface* screen, int n[], Uint32 colores[]){ 
	int x, y, ytimesw;

	if(SDL_MUSTLOCK(screen)){
		if(SDL_LockSurface(screen) < 0) return;
	}

	long int punto=0;
	for(y = 0; y < HEIGHT; y++){
		ytimesw = y*screen->pitch/BPP;
		for( x = 0; x < WIDTH; x++){
			set_pixel(screen, x, ytimesw, colores[n[punto]]);
			punto++;
		}
	}

	rectangleRGBA(screen, mouse_x-25, mouse_y-25, mouse_x+25, mouse_y+25, 255, 255, 0, 255);

	if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

	SDL_Flip(screen); 
}

int main(void){
	int puntos = WIDTH*HEIGHT;
	int tolerancia = 200;
	Uint32 colores[tolerancia];
	int n[puntos];
	
	//Paso inicial	
	long double x_minimo = -2, x_maximo = 2, y_minimo = -2, y_maximo = 2;
	long double paso_parte_real = (x_maximo - x_minimo)/(WIDTH-1);
	long double paso_parte_imaginaria = (y_maximo - y_minimo)/(HEIGHT-1);
	
	//Habilitamos la pantalla
	SDL_Surface *screen;
	SDL_Event event;

	int keypress = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;

	if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE))){
		SDL_Quit();
		return 1;
	}

	//generamos el mapa de colores
	generar_mapa_de_colores(screen, colores, tolerancia);

	//Calculamos el mandelbrot
	mandelbrot(x_minimo, x_maximo, y_minimo, y_maximo, n, tolerancia, paso_parte_real, paso_parte_imaginaria);

	/*
	FILE *f;
	f = fopen("corridas/corrida.txt", "w");
	for(punto = 0; punto < puntos; punto++){
		fprintf(f, "%Lf\t%Lf\t%d\n", z[punto].real, z[punto].imaginario, n[punto]);
	}
	fclose(f);
	*/
	//Run the thread 
	//int data = 101; 
	//SDL_Thread* threadID = SDL_CreateThread( threadFunction, (void*)data ); 
	
	//Dibujamos
	while(!keypress){
		draw_screen(screen, n, colores);
		while(SDL_PollEvent(&event)){      
			switch (event.type){
				case SDL_QUIT:
					keypress = 1;
					break;
				case SDL_KEYDOWN:
					keypress = 1;
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState(&mouse_x,&mouse_y);
					break;
				case SDL_MOUSEBUTTONUP:
					//Calculamos el mandelbrot con las coordenadas nuevas
					if (event.button.button == SDL_BUTTON_LEFT){
						x_maximo = x_minimo+paso_parte_real*(mouse_x+25);
						x_minimo = x_minimo+paso_parte_real*(mouse_x-25);
						y_maximo = y_minimo+paso_parte_imaginaria*(HEIGHT-mouse_y+25);
						y_minimo = y_minimo+paso_parte_imaginaria*(HEIGHT-mouse_y-25);
					}
					if (event.button.button == SDL_BUTTON_RIGHT){
						x_maximo = 2;
						x_minimo = -2;
						y_minimo = -2;
						y_maximo = 2;
					}
					paso_parte_real = (x_maximo - x_minimo)/(WIDTH-1);
					paso_parte_imaginaria = (y_maximo - y_minimo)/(HEIGHT-1);
					mandelbrot(x_minimo, x_maximo, y_minimo, y_maximo, n, tolerancia, paso_parte_real, paso_parte_imaginaria);
					break;
			}
		}
	}
	//Remove thread
	//SDL_WaitThread( threadID, NULL );
	SDL_Quit();

	return 0;
}


