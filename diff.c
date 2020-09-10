#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct{

    int N; //vektorok elemeinek a száma
    double* y;
    double* dy;
    double* y_n;
    double M, gamma, dt, t, T, v0, x0, r, gyok;

}valt;

typedef void (*akarmi)(valt*);

void mallocvektor(valt* m){
    m->y = malloc(m->N * sizeof(double));
    m->y_n = malloc(m->N * sizeof(double));
    m->dy = malloc(m->N * sizeof(double));

    if(m->y == 0 || m->dy == 0 || m->y_n == 0){
        printf(":(\n");
        exit(-1);
    }
}

void printvektor(valt* m){
    for(int i = 0; i < m->N; i++){
        printf("y_n: %lf, y: %lf\n", m->y_n[i], m->y[i]);
    }
    printf("\n");
}

double tavolsag(valt* m){
     return m->r = sqrt((pow(m->y[0], 2) + pow(m->y[2], 2)));
}


void diff(valt* m){ //y = x, vx, y, vy

    m->dy[0] = m->y[1];
    m->dy[1] = - m->y[0] / (pow(tavolsag(m), 3));
    m->dy[2] = m->y[3];
    m->dy[3] = - m->y[2] / (pow(tavolsag(m), 3));
}

void integrator(valt* m, FILE* f, akarmi a){
    for(m->t = 0; m->t < m->T; m->t += m->dt){
        a(m);
        for(int i = 0; i < m->N; i++){
            m->y_n[i] = m->y[i] + m->dt * m->dy[i];
            m->y[i] = m->y_n[i];
        }
        fprintf(f, "%lf\t %lf\t %lf\t %lf\t %lf\n", m->y_n[0], m->y_n[1], m->y_n[2], m->y_n[3], m->t);
    }
}

int main(int argc, char* argv[])
{
    valt o;

    o.N = atoi(argv[1]);

    mallocvektor(&o);

    o.y[0] = atof(argv[2]); //x0
    o.y[1] = atof(argv[3]); // v x0
    o.y[2] = atof(argv[4]); //y 0
    o.y[3] = atof(argv[5]); // v y0
    o.dt = atof(argv[6]); // lepesköz
    o.M = atof(argv[7]); //vonzó tömeg
    o.t = atof(argv[8]); //melyik idõpillanattol indulunk
    o.T = atof(argv[9]); //melyik pillanatig megyünk

    FILE* f = fopen(argv[10], "w");

    if(f == 0){
        printf("nem sikerült megnyitni a fajlt!\n");
        exit(-1);
    }

    integrator(&o, f, &diff);

    fclose(f);

    free(o.y);
    free(o.dy);
    free(o.y_n);

    return 0;
}

