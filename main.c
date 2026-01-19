#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define TAM_BUFFER 512 * sizeof(unsigned char)

int caractere_eh_valido(unsigned char caractere) {
    if (caractere <= 32 || caractere == 127) return 0;
    else return 1; 
}

FILE *abrir_arquivo() {
    FILE *f = fopen("/dev/urandom", "rb");
    if (f == NULL) perror("Erro ao abrir");
    return f;
}

int main(int argc, char *argv []) {
    FILE *fptr;

    if (argc > 2) {
        fprintf(stderr, "Erro: Argumentos demais\n");
        return 1;
    }

    int tam;
    if (argc < 2) {
        tam = 16; // se não forem passados argumentos, assume 16 caracteres como tamanho padrão
    } else {
        if(sscanf(argv[1], "%d", &tam) != 1) {
            fprintf(stderr, "Erro: não é um número\n");
            return 1;
        } else if (tam <= 0) {
            fprintf(stderr, "Erro: o número informado deve ser maior que 0\n");
            return 1;
        }
    } 

    unsigned char *senha = (unsigned char *) malloc((tam +1) * sizeof(unsigned char));
    
    if (senha == NULL) {
        perror("Falha no malloc");
        return 1;
    }

    fptr = abrir_arquivo();
    if (fptr == NULL) {
        free(senha);
        return 1;
    }

    unsigned char buffer[TAM_BUFFER];
    
    int i = 0;
    int pos_buffer = 0;
    int buffer_restante = 0;
    while (i < tam) {
        if (buffer_restante == 0) {
            size_t lidos = fread(buffer, 1, TAM_BUFFER, fptr);
            if (lidos ==0) {
                fprintf(stderr, "Erro crítico: falha no /dev/urandom\n");
                free(senha);
                fclose(fptr);
                return 1;
            }

            buffer_restante = lidos;
            pos_buffer = 0;
        }
        
        unsigned char c = toascii(buffer[pos_buffer]);
        if (caractere_eh_valido(c)) {
            senha[i] = c;
            i++;
        }

        pos_buffer++;
        buffer_restante--;
    }
    
    senha[tam] = '\0';
    printf("%s\n", senha);

    free(senha);
    fclose(fptr); 
    return 0;
}   