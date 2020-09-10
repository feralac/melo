#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct{
    int sor;
    int oszlop;
    double* m;
}matrix_t;

void mallocmatrix(matrix_t* mx){ //memoriat foglal a megadott paraméterek alapján
    mx->m = malloc(mx->sor * mx->oszlop * sizeof(double));
    if(mx->m == 0){
        printf("Baj van a memoriafoglalassal! :( \n");
        exit(-1);
    }
}

void loadmatrix(FILE* f, matrix_t mx){ //a megadott fajlbol beolvassa a matrixot
    for(int i = 0; i < mx.sor; i++){
        for(int j = 0; j < mx.oszlop; j++){
            fscanf(f, "%lf", &mx.m[i * mx.oszlop + j]);
        }
    }
}

void egysegmatrix(matrix_t* unit){ //legyartja az egysegmatrixot
    for(int i = 0; i < unit->sor; i++){
        for(int j = 0; j < unit->oszlop; j++){
            if(i == j){
                unit->m[i * unit->oszlop + j] = 1;
            }
            else{
                unit->m[i * unit->oszlop + j] = 0;
            }
        }
    }
}

void printmatrix(matrix_t* mx){ //kiprinteli a matrixot
    for(int i = 0; i < mx->sor; i++){
        for(int j = 0; j < mx->oszlop; j++){
            printf("%lf ", mx->m[i * mx->oszlop + j]);
        }
        printf("\n");
    }
    printf("------\n");
}

void gje_sorcsere(matrix_t* mx,int i1, int i2){ //a megadott sorok alapjan elvegzia a sorcserét
    double* csere = malloc(sizeof(double));
    for(int j = 0; j < mx->oszlop; j++){
        *csere = mx->m[i1 * mx->oszlop + j];
        mx->m[i1 * mx->oszlop + j] = mx->m[i2 * mx->oszlop + j];
        mx->m[i2 * mx->oszlop + j] = *csere;
    }
    free(csere);
}

double max_absz_sor(matrix_t* mx, int i){//adott sorban keresi meg a legnagyobb absz értekü elemet
    double max = 0;
    for(int j = 0; j < mx->oszlop; j++){
        if(max < fabs(mx->m[i * mx->oszlop + j])){
            max = fabs(mx->m[i * mx->oszlop + j]);
        }
    }
    return max;
}

void gje_norm_matrix(matrix_t* mx, matrix_t* unit){ //a legnagyobb absz erteku taggal leosztja minden sor minden elemet
    double max;
    for(int i = 0; i < mx->sor; i++){
        max = max_absz_sor(mx, i);
        for(int j = 0; j < mx->oszlop; j++){
            unit->m[i * unit->oszlop + j] /= max;
            mx->m[i * mx->oszlop + j] /= max;
        }
    }
}

void gje_diagonalrendezes(matrix_t* mx, matrix_t* unit, int j){ //pivotal
    double max = 0;
    int imax = 0;
    for(int i = j; i < mx->oszlop ; i++){
        if(max < fabs(mx->m[i * mx->oszlop + j])){
            max = fabs(mx->m[i * mx->oszlop + j]);
            imax = i;
        }
    }
    if(max == 0){ // a a pivot elem nulla akkor szingularis :(
        printf("A matrix szingularis! Nem tudom eliminalni! :( \n");
        exit(-1);
    }
    else if(max < pow(10, -7)){//numerikusan szingularis e a mx; az abrázolási tartomány gyöke a praktikus határ
        printf("A matrix numerikusan szingularis!\n");
        exit(-1);
    }
    gje_sorcsere(mx, j, imax);
    gje_sorcsere(unit, j, imax);
}

void diagonal_oszto(matrix_t* mx, matrix_t* unit, int i){ // "lenormalja a sorokat a pivotelem absz ertelevel"
    double diagelem = mx->m[i * mx->oszlop + i];
    for(int j = 0; j < mx->oszlop; j++){
            unit->m[i * unit->oszlop + j] /= diagelem;
            mx->m[i * mx->oszlop + j] /= diagelem;
    }
}

double gje_oszto_le(matrix_t* mx, int i1, int i2){
    double a;
        a = mx->m[i2 * mx->oszlop + i1]/mx->m[i1 * mx->oszlop + i1];
    return a;
}

//a negyedik paraméterbõl vonja ki a harmadikat 3 4
void gje_sorkivonas(matrix_t* mx, matrix_t* unit, int i1, int i2){
    double a = gje_oszto_le(mx, i1, i2); //a pivotelem alatt elemeket osztja a pivotelemmel, itt mar csak egyesek vannak a főatloban
    for(int j = 0; j < mx->oszlop; j++){
        mx->m[i2 * mx->oszlop + j] -=  mx->m[i1 * mx->oszlop + j] * a;
        unit->m[i2 * unit->oszlop + j] -= unit->m[i1 * unit->oszlop + j] *a;
    }
}
void gje_elim_le(matrix_t* mx, matrix_t* unit, int i){ //lefele eliminal
    for(int k = i + 1; k < mx->sor; k++){
        gje_sorkivonas(mx, unit, i, k);
    }
}

void gje_elim_fel(matrix_t* mx, matrix_t* unit, int i){ //felfele eliminal
    for(int k = i - 1 ; k >= 0; k--){
        gje_sorkivonas(mx, unit, i, k);
    }
}

void gje_start(matrix_t* mx, matrix_t* unit){ //elinditja az eliminaciot
    gje_norm_matrix(mx, unit);
    for(int j = 0; j < mx->oszlop; j++){
        gje_diagonalrendezes(mx, unit, j);
        diagonal_oszto(mx, unit, j);
        gje_elim_le(mx, unit, j);
    }
    for(int j = mx->oszlop -1; j >= 0; j--){
        gje_elim_fel(mx, unit, j);
    }
}

void productmx(matrix_t* mx1, matrix_t* mx2, matrix_t* mx3){ // matrixszorzo ciklus az ellenörzéshez
    if(mx1->oszlop != mx2->sor){
        printf("A matrixok nem osszeszorozhatoak!\n");
        exit(-1);
    }

    int pmmi = 0; //eredmenymatrix memoriajanak indexe "product matrix memory index"

    for(int i = 0; i < mx1->sor; i++){
        for(int j = 0; j < mx2->oszlop; j++){
            double ertek = 0;
            for(int k = 0; k < mx1->sor; k++){
                ertek += mx1->m[k + i * mx1->sor] * mx2->m[j + k * mx2->oszlop];
            }
            mx3->m[pmmi++] = ertek;
        }
        printf("\n");
    }
}

void egyenletbeolvaso(FILE* g, matrix_t egyenlet){//a megadott fajlbol beolvassa a vektor elemeit es mint oszlopvektort reprezentálja
    for(int i = 0; i < egyenlet.sor; i++){
        for(int j = 0; j < egyenlet.oszlop; j++){
            if(j == 0){
                fscanf(g, "%lf", &egyenlet.m[i * egyenlet.oszlop + j]);
            }
            else{
                egyenlet.m[i * egyenlet.oszlop + j] = 0;
            }
        }
    }
}

int main(int argc, char* argv[]){

    //3 vagy 4 bemenetet vár!
    //ha invertalni szeretnel add meg a matrixot tartalmazo fajlt és a matrix méreteit
    //ha egyenletrendszert szeretnél megoldani akkor pedig a vektor tartalmazo fajl nevét is add meg

    matrix_t mx; //letrehozzuk a strukturat

    mx.sor = atoi(argv[2]); //beallitjuk a struct elemeit
    mx.oszlop = atoi(argv[3]);

    mallocmatrix(&mx);//memoriat foglalunk

    FILE* f = fopen(argv[1],"r"); //megnyitjuk, betöltjük, majd bezárjuk a fajlt
    loadmatrix(f, mx);
    fclose(f);

    if(argc == 4){
        matrix_t ell; //itt ugyanaz mint fent csak pepitaban
        matrix_t unit;

        unit.sor = atoi(argv[2]);
        unit.oszlop = atoi(argv[3]);

        ell.sor = atoi(argv[2]);
        ell.oszlop = atoi(argv[3]);

        mallocmatrix(&ell);
        mallocmatrix(&unit);

        egysegmatrix(&unit); //legyartjuk az egysegmatrixot

        FILE* g = fopen(argv[1], "r");
        loadmatrix(g, ell);
        fclose(g);

        printf("az alapmatrix es az egysegmatrix:\n"); //elinditjuk az eliminaciot meg kiirjuk a dumat hogy feldolgozható legyen
        printf("\n");
        printmatrix(&mx);
        printmatrix(&unit);
        gje_start(&mx, &unit);
        printf("a kapott egysegmatrix es az eredeti inverze:\n");
        printf("\n");
        printmatrix(&mx);
        printmatrix(&unit);
        printf("az eredeti A matrix:\n");
        printmatrix(&ell);
        printf("az eredeti es az inverz szorzata:\n");
        productmx(&ell, &unit, &mx);
        printmatrix(&mx);

        free(unit.m);//felszabadítjuk a foglalt memóriát
        free(ell.m);
    }
    else if(argc == 5){
        matrix_t egyenlet; //semmi új, paraméterek beállitasa, mem foglalás etc
        egyenlet.sor = atoi(argv[2]);
        egyenlet.oszlop = atoi(argv[3]);
        mallocmatrix(&egyenlet);

        FILE* g = fopen(argv[4], "r");

        egyenletbeolvaso(g, egyenlet); // beolvassuk az egyenletet
        printmatrix(&egyenlet);

        gje_start(&mx, &egyenlet); //megoldjuk

        printf("Az egysegmatrix:\n"); //kiiratjuk etc
        printmatrix(&mx);
        printf("Es az egyenletrendszer megoldasa:\n");
        printmatrix(&egyenlet);

        free(egyenlet.m); //felszabadítunk és zárjuk a fajlt
        fclose(g);
    }

    else{
        printf("Nem adtal meg eleg parametert!\n");
    }

    free(mx.m); // végül felszabadítunk és hátradőlünk, vééééégre....

    return 0;
}
