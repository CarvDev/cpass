#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int caractere_eh_valido(unsigned char caractere_inicial) {
    if (caractere_inicial <= 32 || caractere_inicial == 127) return 0;
    else return 1; 
}

int main(int argc, char *argv []) {
    FILE *fptr;

    if (argc < 2) {
        fprintf(stderr, "Argumentos insuficientes\n");
        return 1;
    } else if (argc > 2) {
        fprintf(stderr, "Argumentos demais\n");
        return 1;
    }

    int tam;
    if(sscanf(argv[1], "%d", &tam) != 1) {
        fprintf(stderr, "Erro: não é um número\n");
        return 1;
    } else if (tam <= 0) {
        fprintf(stderr, "Erro: o número informado deve ser maior que 0\n");
        return 1;
    }

    unsigned char *senha = (unsigned char *) malloc((tam +1) * sizeof(unsigned char));

    if (senha == NULL) {
        perror("Falha no malloc");
        return 1;
    }

    if ((fptr = fopen("/dev/urandom", "rb")) == NULL) {
        perror("Erro ao abrir o arquivo");
        free(senha);
        return 1;
    }

    int i = 0;
    while (i < tam) {
        unsigned char temp;
        fread(&temp, sizeof(temp), 1, fptr);
        temp = toascii(temp);

        if (caractere_eh_valido(temp)) {
            senha[i] = temp;
            i++;
        }
    };
     
    senha[tam] = '\0';

    printf("%s\n", senha);

    free(senha);
    fclose(fptr); 
    return 0;
}