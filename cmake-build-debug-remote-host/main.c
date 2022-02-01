#include "moviesADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define DIV ";\n\r\t"
#define MAX 500
#define MAX_ANIMATED_MOVIES 500


enum  dataset {ID = 0, TYPE, TITLE, STARTY, ENDY, GENRE, RATING, VOTES, DUR};

void readData(moviesADT m, FILE * data){
    char token[MAX];
    char * line;
    //Hay que saltear la primera linea
    fgets(token, MAX, data);
    int i, year, votes;
    char * id;
    char * title;
    double rating;
    char * type;
    char * genre;
    while (fgets(token, MAX, data) != NULL){
        line = strtok(token, DIV);
        for (i = 0; i < DUR; i++){
            switch (i){
                case ID: id = line;
                    break;
                case TYPE: type = line;
                    break;
                case TITLE: title = line;
                    break;
                case STARTY: year = atoi(line);
                    break;
                case GENRE: genre = line;
                    break;
                case RATING: rating = atof(line);
                    break;
                case VOTES: votes = atoi(line);
                    break;
                default: break;
            }
            line = strtok(NULL, DIV);
        }

        int added = addMovieSeries(m, genre, year, type, title, votes, rating);

        //Se chequea que se haya agregado correctamente
        /*if (added == 0) {
            fprintf(stderr, "Error adding data\n");
            return;
        }*/
        //else{
        //DEBUG
        //  printf("%s\n", title);
        //  DEBUG
        //}

    }
}

void solQ1(moviesADT m, FILE * f1){
    toBeginYear(m);

    fprintf(f1, "year;films;series;shorts\n");
    //printf("year;films;series;shorts\n");
    unsigned int year, films, series, shorts;

    while(hasNextYear(m)){
        year = nextYear(m, &films, &series, &shorts);
        fprintf(f1, "%d;%d;%d;%d\n", year, films, series, shorts);
        //printf("%d;%d;%d;%d\n", year, films, series, shorts);
    }
}

void solQ2(moviesADT m, FILE * f2){
        toBeginMovie(m);

        printf("year;film;votes;rating;genres\n");
        //fprintf(f2, "year;film;votes;rating;genres\n");
        char ** vec;


        double rating;
        int votes;
        unsigned int year;
        int i;


        for (i=0;hasNextMovie(m) && i<MAX_ANIMATED_MOVIES;i++){
            vec = nextMovie2(m, &year, &votes, &rating);

            if(vec!=0) {
                //fprintf(f2, "%d;%s;%d;%f;%s\n", year, vec[0], votes, rating, vec[1]);
                printf("%d;%s;%d;%f;%s\n", year, vec[0], votes, rating, vec[1]);
            }
        }
    }

int main(int argc, char * argv[]){

    //Se verifica la cantidad de argumentos
    if (argc != 2){
        fprintf(stderr, "Include ONE file only\n");
        return 1;
    }

    //Se genera el archivo
    FILE * movieSeries = fopen(argv[1], "r");

    //Se abren los archivos csv
    //FILE * f1 = fopen("./query1.csv", "w");
    FILE * f2 = fopen("./query2.csv", "w");
    // FILE * f3 = fopen("./query3.csv", "w");
    // FILE * f4 = fopen("./query4.csv", "w");
    // FILE * f5 = fopen("./query5.csv", "w");

    //Se comprueba que el archivo exista y se puede abrir
    if(movieSeries == NULL){
        fprintf(stderr, "File not found or cannot access file\n");
        return 1;
    }

    //Se crea un nuevo TAD vacio
    moviesADT movieList = newMoviesADT();

    //Se verifica que haya memoria disponible
    if (movieList == NULL){
        fprintf(stderr, "Not enough memory\n");
        return 1;
    }

    //Con los datos del archivo .csv completamos la lista
    readData(movieList, movieSeries);

    //Una vez leido el archivo, ejecutamos los queries
    //solQ1(movieList, f1);
    solQ2(movieList, f2);
    //solQ3(movieList, f3);

    //Se libera la memoria utilizada
    //freeMoviesADT(movieList);

    //fclose(f1);
    fclose(f2);
    //fclose(f3);

    printf("Queries created successfully!\n");

    return 0;
}