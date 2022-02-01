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
    movieData * currentMovie;  //Para iterar por las peliculas animadas
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

//Recibe un string que contiene los generos separados por ,
//Devuelve un vector donde en cada posicion almacena un genero y deja su dimension en un parametro de salida
/*static char ** genreVec(char * s, unsigned int * dim){
    char * line;
    char ** vec = malloc(sizeof(char *) * BLOCK);
    if(vec == NULL || errno == ENOMEM){
        perror("Memory ERROR in genreVec");
        return NULL;
    }

    unsigned int i = 0;
    line = strtok(s, GDIV);

    while(line != NULL){
        if(i % BLOCK == 0){
            vec= realloc(vec, sizeof(char *) * (BLOCK + i));
        }
        if(vec == NULL || errno == ENOMEM){
            perror("Memory ERROR in genreVec");
            return NULL;
        }

        vec[i] = malloc(strlen(line) + 1);
        strcpy(vec[i], line);
        i++;

        line = strtok(NULL, GDIV);
    }

    vec = realloc(vec, sizeof(char *) * i);
    if(vec == NULL || errno == ENOMEM){
        perror("Memory ERROR in genreVec");
        return NULL;
    }
    *dim = i;
    return vec;
}

*/

/*static TGenre * addGenre(TGenre * first, char * name, char * type){
    int c;
    if(first == NULL || (c = strcmp(first->name, name)) > 0){
        TGenre * aux = malloc(sizeof(TGenre));
        if(aux == NULL || errno == ENOMEM){
            perror("Memory ERROR in addGenre");
            return NULL;
        }
        aux->name = malloc(strlen(name) + 1);
        if(aux->name == NULL || errno == ENOMEM){
            perror("Memory ERROR in addGenre");
            return NULL;
        }
        strcpy(aux->name, name);

        if (strcmp("movie", type) == 0) {
            aux->sizeM = 1;         //porque se que estan llamando a la funcion para agregar una pelicula
        }

        aux->tail = first;
        return aux;
    }

    if(c < 0)
        first->tail = addGenre(first->tail, name, type);
    else
        first->sizeM += 1;

    return first;
}
*/
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

int addMovieSeries(moviesADT m, char * genre, unsigned int year, char * type, char * title, int votes, double rating) {
    addYear(m, year); //Si no esta el año, agrega el año
    TYear *currentY = searchYearRec(m->firstYear, year); //Busca el año y devuelve un puntero al nodo en el que inserto
    if (currentY == NULL)
        return 2;   //Si en el año pasan \N no agrega

    if (strcmp("movie", type) == 0) { //Si  pasan una pelicula
        currentY->totalM++;
        if (currentY->mostVotedMovie->votes < votes) { //Actualizo la MAS VOTADA. TODO: HACER LAS 100 MAS VOTADAS!
            currentY->mostVotedMovie->title = realloc(currentY->mostVotedMovie->title, strlen(title) + 1);

            if (currentY->mostVotedMovie->title == NULL || errno == ENOMEM) {
                return 2;
            }
            strcpy(currentY->mostVotedMovie->title, title);
            currentY->mostVotedMovie->rating = rating;
            currentY->mostVotedMovie->votes = votes;
        }

        //unsigned int i = 0, dim = 0;

        /*char **vec = genreVec(genre, &dim);
        //Recorremos el vector que almacena los distintos generos para una pelicula y agregamos la misma en cada uno
        while (i < dim) {
            currentY->firstGenre = addGenre(currentY->firstGenre,
                                            vec[i], type); //Busca el genero y retorna el nodo si esta; sino lo agrega y lo retorna
            i++;
        }*/

        if (isValidAnimatedMovie(genre, votes) == 1){
            m->bestAnimatedMoviesAllTime=addAnimatedMovie(m->bestAnimatedMoviesAllTime,genre,year,type,title,votes,rating);
        }

        /*for (unsigned int k = 0; k < dim; k++) {
            free(vec[k]);
        }
        free(vec);*/
    } else if (strcmp("tvSeries", type) == 0 || strcmp("tvMiniSeries", type) == 0) {
        currentY->totalS++;

        /*unsigned int i = 0, dim = 0;
        char **vec = genreVec(genre, &dim);
        //Recorremos el vector que almacena los distintos generos para una pelicula y agregamos la misma en cada uno
        while (i < dim) {
            currentY->firstGenre = addGenre(currentY->firstGenre,vec[i], type); //Busca el genero y retorna el nodo si esta; sino lo agrega y lo retorna
            i++;
        }
        for (unsigned int k = 0; k < dim; k++) {
            free(vec[k]);
        }
        free(vec);
    */
    } else if (strcmp("short", type) == 0) {
        currentY->totalSh++;

      /*  unsigned int i = 0, dim = 0;
        char **vec = genreVec(genre, &dim);
        //Recorremos el vector que almacena los distintos generos para una pelicula y agregamos la misma en cada uno
        while (i < dim) {
            currentY->firstGenre = addGenre(currentY->firstGenre,
                                            vec[i], type); //Busca el genero y retorna el nodo si esta; sino lo agrega y lo retorna
            i++;
        }
        for (unsigned int k = 0; k < dim; k++) {
            free(vec[k]);
        }
        free(vec);*/
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

unsigned int nextYear(moviesADT m, unsigned int * movies, unsigned int * series, unsigned int * shorts) {

    *movies = m->currentYear->totalM;
    *series = m->currentYear->totalS;
    *shorts = m->currentYear->totalSh;

    unsigned int aux = m->currentYear->year;
    m->currentYear = m->currentYear->tail;
    return aux;
}

void toBeginMovie(moviesADT m){
    m->currentMovie=m->bestAnimatedMoviesAllTime;
}

unsigned int hasNextMovie(moviesADT m){
    return m->currentMovie!=NULL;
}


char ** nextMovie2(moviesADT m, unsigned int * year, int * votes, double *rating){
    if(!hasNextMovie(m))
        return 0;

    *votes=m->currentMovie->votes;
    *rating=m->currentMovie->rating;
    *year = m->currentMovie->year;

    char ** aux = malloc(2 * sizeof(char *));

    aux[0] = malloc(strlen(m->currentMovie->title)+1);
    strcpy(aux[0], m->currentMovie->title);

    aux[1] = malloc(strlen(m->currentMovie->genres)+1);
    strcpy(aux[1], m->currentMovie->genres);

    m->currentMovie=m->currentMovie->nextMovie;
    return aux;

}
