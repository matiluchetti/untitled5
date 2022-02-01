#include "moviesADT.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define MAX_GENRES 32
#define MIN_VOTES_ANIMATED 20000
#define GDIV ","

//Contiene los datos de la pelicula mas votada
typedef struct dataNode{
    char * title;
    char * genres;
    double rating;
    int votes;
    unsigned int year;
    struct dataNode * nextMovie;
}movieData;

//Estructura del nodo para la lista de años
typedef struct yearNode{
    unsigned int year;
    unsigned int totalM;      //Cantidad total de peliculas en ese año
    unsigned int totalS;      //Cantidad total de series en ese año
    unsigned int totalSh;       //Cantidad total de cortos en ese año.
    movieData * mostVotedMovie;      //  Puntero a la primera pelicula mas votada del año.
    struct yearNode * tail;
}TYear;



typedef struct moviesCDT{
    TYear * firstYear;
    movieData * bestAnimatedMoviesAllTime;    //Puntero a la estructura con datos de las 500 peliculas animadas de mejor rating
    movieData * bestSeriesAllTime; //   Puntero a las 250 mejores series.
    movieData * worstSeriesAllTime; //  Puntero a las 50 peores series.
    TYear * currentYear;    //Para iterar por los años
    movieData * currentAnimatedMovie;  //Para iterar por las peliculas animadas
    movieData * currentTopMovie; // Para iterar por las mejores peliculas
}moviesCDT;


static int compareVotes(movieData * movie1, movieData * movie2){
    return movie1->votes - movie2->votes;
    //  Retorna positivo si movie1 tiene mas votos que movie 2.
    //  Retorna negativo si movie2 tiene mas votos que movie 1.
    //  Retorna 0 si tienen igual cantidad de votos.
}

static double compareRating(movieData * movie1, movieData * movie2){
    if (movie1->rating == movie2->rating){
        return movie1->votes - movie2->votes;
    }
    else{
        return movie1->rating - movie2->rating;
        //  Retorna positivo si m1 es mejor que m2.
        //  Retorna negativo si m2 es mejor que m1.
        //  Retorna compareVotes si son igual de buenas.
    }
}

moviesADT newMoviesADT(){
    moviesADT aux = calloc(1, sizeof(moviesCDT));
    if(aux == NULL || errno == ENOMEM){
        perror("Memory ERROR in newMoviesADT.");
        return NULL;
    }
    return aux;
}

static TYear * searchYearRec(TYear * first, unsigned int year){
    if(first == NULL || first->year < year)
        return NULL;
    if(first->year > year)
        return searchYearRec(first->tail, year);
    // Si no es ni mayor ni menor es igual
    return first;
}

static TYear * addYearRec(TYear * first, unsigned int year){
    if(first == NULL || first->year < year){
        //  Si tengo que agregar el año
        // Reservo espacio para el año
        TYear * aux = calloc(1, sizeof(TYear));
        if(aux == NULL || errno == ENOMEM){
            perror("Memory ERROR in addYearRec");
            return NULL;
        }

        aux->year = year;

        //  Reservo espacio para las pelis mas votadas

        aux->mostVotedMovie = calloc(1, sizeof(movieData));
        if(aux->mostVotedMovie == NULL || errno == ENOMEM){
            free(aux);
            perror("Memory ERROR in addYearRec");
            return NULL;
        }

        // Y lo meto en la lista.
        aux->tail = first;
        return aux;
    }

    // Si no tengo que añadir el año, paso al siguiente en la lista.
    if(first->year > year)
        first->tail = addYearRec(first->tail, year);

    return first;
}

static void addYear(moviesADT m, unsigned int year){
    if(year == 0) //El año podria ser 0 si leo un "\N"
        return;
    m->firstYear = addYearRec(m->firstYear, year);
}
static int isValidAnimatedMovie(char * genre, unsigned int votes){

    if (votes < MIN_VOTES_ANIMATED)
        return 0;

    char * line;
    char * resp = NULL;
    int c;
    unsigned long len=0;
    int flag = 0;
    line = strtok(genre,GDIV);
    for(int i = 0; i < MAX_GENRES && line != NULL; i++){
        c = strcmp("Animation", line);
        if (c != 0){
            len+= strlen(line)+1;      //llevo la cuenta de la longitud con la coma
            resp = realloc(resp, (len+1));
            resp = strcat(resp, line);     //concateno los generos
            resp = strcat(resp, GDIV);     //le agrego la coma
        }

        if (c<0 && flag==0){     //si ya lo paso y no esta animation salgo
            return 0;
        }
        else if (c==0)       //si lo encuentro prendo el flag
            flag=1;
        line = strtok(NULL,GDIV);

    }
    resp[len-1]=0;
    strcpy(genre,resp);       //lo paso sin la ultima coma al paremetro de salida
    return 1;
}

static movieData *addAnimatedMovie(movieData *m, char * genre, unsigned int year, char * type, char * title, int votes, double rating){
    if(m==NULL || m->rating<rating || (m->rating==rating && m->votes<votes)){
        movieData *aux= calloc(1, sizeof(movieData));
        if(aux == NULL || errno == ENOMEM){
            perror("Memory ERROR in addAnimatedMovie");
            return NULL;
        }
        aux->votes=votes;
        aux->rating=rating;
        aux->year=year;
        aux->title= malloc(strlen(title)+1);
        if(aux->title == NULL || errno == ENOMEM){
            perror("Memory ERROR in addAnimatedMovie");
            return NULL;
        }
        strcpy(aux->title,title);
        aux->genres= malloc(strlen(genre)+1);
        if(aux->genres == NULL || errno == ENOMEM){
            perror("Memory ERROR in addAnimatedMovie");
            return NULL;
        }
        strcpy(aux->genres,genre);
        aux->nextMovie=m;
        return aux;
    } else m->nextMovie= addAnimatedMovie(m->nextMovie,genre,year,type,title,votes,rating);
    return m;
}

movieData *addTopMovie(movieData *m, char * title, int votes, double rating, char * genre){
    if(m==NULL || m->votes<votes || (m->votes==votes && strcmp(m->title,title)<0)){
        movieData *aux= calloc(1,sizeof(movieData));
        if (aux == NULL || errno == ENOMEM){
            perror("Memory ERROR in addTopMovie");
            return NULL;
        }

        aux->votes=votes;
        aux->rating=rating;
        aux->title= malloc(strlen(title)+1);
        if(aux->title == NULL || errno == ENOMEM){
            perror("Memory ERROR in addTopMovie");
            return NULL;
        }
        strcpy(aux->title,title);

        aux->genres= malloc(strlen(genre)+1);
        if(aux->genres == NULL || errno == ENOMEM){
            perror("Memory ERROR in addTopMovie");
            return NULL;
        }
        strcpy(aux->genres,genre);

        aux->nextMovie=m;
        return aux;
    }
    else
        m->nextMovie= addTopMovie(m->nextMovie,title,votes,rating,genre);
    return m;
}

int addMovieSeries(moviesADT m, char * genre, unsigned int year, char * type, char * title, int votes, double rating) {
    addYear(m, year); //Si no esta el año, agrega el año
    TYear *currentY = searchYearRec(m->firstYear, year); //Busca el año y devuelve un puntero al nodo en el que inserto
    if (currentY == NULL)
        return 2;   //Si en el año pasan \N no agrega

    if (strcmp("movie", type) == 0) { //Si  pasan una pelicula
        currentY->totalM++;
        if(currentY->mostVotedMovie==NULL)
            currentY->mostVotedMovie=malloc(sizeof(movieData));
        currentY->mostVotedMovie=addTopMovie(currentY->mostVotedMovie,title,votes,rating,genre);
        if (isValidAnimatedMovie(genre, votes) == 1){
            m->bestAnimatedMoviesAllTime=addAnimatedMovie(m->bestAnimatedMoviesAllTime,genre,year,type,title,votes,rating);
        }
    }
    else if (strcmp("tvSeries", type) == 0 || strcmp("tvMiniSeries", type) == 0) {
        currentY->totalS++;



    }
    else if (strcmp("short", type) == 0) {
        currentY->totalSh++;
    }

    //else
    //   return 0;   //retorna 0 si NO pudo agregar

    return 1;       //agrego correctamente
}

void toBeginYear(moviesADT m){
    m->currentYear = m->firstYear;
}

unsigned int hasNextYear(moviesADT m){
    return m->currentYear != NULL;
}

unsigned int *nextYear(moviesADT m, unsigned int *year) {

    unsigned int *v=malloc(sizeof(unsigned int)*3);
    v[0] = m->currentYear->totalM;
    v[1] = m->currentYear->totalS;
    v[2] = m->currentYear->totalSh;

    *year = m->currentYear->year;
    m->currentYear = m->currentYear->tail;
    return v;
}

void toBeginMovie(moviesADT m){
    m->currentAnimatedMovie=m->bestAnimatedMoviesAllTime;
}

unsigned int hasNextMovie(moviesADT m){
    return m->currentAnimatedMovie != NULL;
}


char ** nextMovie2(moviesADT m, unsigned int * year, int * votes, double *rating){
    if(!hasNextMovie(m))
        return 0;

    *votes=m->currentAnimatedMovie->votes;
    *rating=m->currentAnimatedMovie->rating;
    *year = m->currentAnimatedMovie->year;

    char ** aux = malloc(2 * sizeof(char *));
    if (aux == NULL || errno == ENOMEM){
        perror("Memory ERROR in nextMovie2");
    }

    aux[0] = malloc(strlen(m->currentAnimatedMovie->title) + 1);
    if (aux[0] == NULL || errno == ENOMEM){
        perror("Memory ERROR in nextMovie2");
    }
    strcpy(aux[0], m->currentAnimatedMovie->title);

    aux[1] = malloc(strlen(m->currentAnimatedMovie->genres) + 1);
    if (aux[1] == NULL || errno == ENOMEM){
        perror("Memory ERROR in nextMovie2");
    }
    strcpy(aux[1], m->currentAnimatedMovie->genres);

    m->currentAnimatedMovie=m->currentAnimatedMovie->nextMovie;
    return aux;

}

void toBeginMostVotedMovie(moviesADT m){
    m->currentTopMovie = m->currentYear->mostVotedMovie;
}

unsigned int hasNextTopMovie(moviesADT m){
    return (m->currentTopMovie->title != NULL);
}

char ** nextTopMovie(moviesADT m, int * votes, double * rating){
    if (!hasNextTopMovie(m)){
        return NULL;
    }

    *votes = m->currentTopMovie->votes;
    *rating = m->currentTopMovie->rating;

    char ** aux = malloc(2 * sizeof(char *));
    if (aux == NULL || errno == ENOMEM){
        perror("Memory ERROR in nextMovie2");
    }

    aux[0] = malloc(strlen(m->currentTopMovie->title) + 1);
    if (aux[0] == NULL || errno == ENOMEM){
        perror("Memory ERROR in nextMovie2");
    }
    strcpy(aux[0], m->currentTopMovie->title);

    aux[1] = malloc(strlen(m->currentTopMovie->genres) + 1);
    if (aux[1] == NULL || errno == ENOMEM){
        perror("Memory ERROR in nextMovie2");
    }
    strcpy(aux[1], m->currentTopMovie->genres);
    m->currentTopMovie=m->currentTopMovie->nextMovie;

    return aux;
}