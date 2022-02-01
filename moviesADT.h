#ifndef PROYECTOPI_MOVIESADT_H
#define PROYECTOPI_MOVIESADT_H
typedef struct moviesCDT * moviesADT;

//Crea un nuevo TAD vaciogit
moviesADT newMoviesADT();

//Permite iterar por todos los años en orden descendente
void toBeginYear(moviesADT m);

//Retorna 1 si queda un año por recorrer o 0 sino
unsigned int hasNextYear(moviesADT m);

//Retorna el siguiente año y deja en el parametro movies y series la cantidad de peliculas y series que hubo en ese año
unsigned int * nextYear(moviesADT m, unsigned int * year);

int addMovieSeries(moviesADT m, char * genre, unsigned int year, char * type, char * title, int votes, double rating);

void toBeginMovie(moviesADT m);

unsigned int hasNextMovie(moviesADT m);

char ** nextTopMovie(moviesADT m, int *votes, double *rating);

char ** nextMovie2(moviesADT m, unsigned int * year, int * votes, double *rating);

void toBeginMostVotedMovie(moviesADT m);

unsigned int hasNextTopMovie(moviesADT m);



#endif //PROYECTOPI_MOVIESADT_H