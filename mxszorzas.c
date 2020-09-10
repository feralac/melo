#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int sor;
    int oszlop;
    double* m;
    }mx_t;

void mallocmx(mx_t* mx){
    mx->m = malloc(mx->oszlop * mx->sor * sizeof(double));

    if(mx->m == 0){
        printf("baj van a memoriafoglalassal! :( \n");
        exit(-1);
    }
}

void productmx(mx_t* mx1, mx_t* mx2, mx_t* mx3){
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

void loadmx(FILE* f, mx_t mx){
    for(int i = 0; i < mx.sor; i++){
        for(int j = 0; j < mx.oszlop; j++){
            fscanf(f, "%lf", &mx.m[i * mx.oszlop + j]);
        }
    }
}

void printmx(mx_t mx){
    for(int i = 0; i < mx.sor; i++){
        for(int j = 0; j < mx.oszlop; j++){
            printf("%lf ", mx.m[i * mx.oszlop + j]);
        }
    printf("\n");
    }
}

int main(int argc, char* argv[]){
    mx_t mx1;
    mx_t mx2;
    mx_t mx3;

    if(argc < 6){
        printf("Nem adtal meg eleg argumentumot!\n");
    }

    FILE* f1 = fopen(argv[1], "r");
    mx1.sor = atoi(argv[2]);
    mx1.oszlop = atoi(argv[3]);

    FILE* f2 = fopen(argv[4], "r");
    mx2.sor = atoi(argv[5]);
    mx2.oszlop = atoi(argv[6]);

    mx3.sor = mx1.sor;
    mx3.oszlop = mx2.oszlop;

    mallocmx(&mx1);
    mallocmx(&mx2);
    mallocmx(&mx3);

    loadmx(f1, mx1);
    loadmx(f2, mx2);

    printf("az A matrix:\n");
    printmx(mx1);
    printf("\n");

    printf("a B matrix:\n");
    printmx(mx2);


    productmx(&mx1, &mx2, &mx3);

    printf("az AB matrix:\n");
    printmx(mx3);

    free(mx1.m);
    free(mx2.m);
    free(mx3.m);

    fclose(f1);
    fclose(f2);

    return 0;
}
