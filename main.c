#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(A, B) ( A < B ? A : B)
#define max(A, B) ( A > B ? A : B)

typedef struct string_t {
    char* c;
    int len;
} string;

typedef struct suffix_t {
    string* s;
    int index;
} suffix;

int compara_de(string* s, string* t, int i1, int i2);
static int compara_sufixos(const void* s1, const void* s2);
string* le_entrada(FILE* f, int n);
void libera_string(string* str);
void libera_vetor_sufixos(suffix* vet_suf);
suffix* gera_sufixos(string* str);
void libera_sufixos(suffix* s);
void organiza_sufixos(suffix* s, int tam);
void imprime_sufixos(suffix* vet_suf, int tam);
void le_query(suffix* vet_suf, int tam, int contexto);
void query(suffix* vet_suf, int tam, string* str, int contexto);
int contem(string* s1, string* s2, int i1, int i2);
int busca_binaria(suffix* vet_suf, int lo, int hi, string* k);
void imprime_string(string* str);
void imprime_string_intervalo(string* str, int i1, int i2);

int main(int argc, char** argv) {
    FILE* f = fopen(argv[1], "r");

    if(f == NULL) {
        printf("Erro na abertura do arquivo!\n");
        exit(1);
    }

    int contexto = atoi(argv[2]);

    int n = 0;
    fscanf(f, "%d\n", &n);

    string* str = le_entrada(f, n);
    // imprime_string(str);

    suffix* vet_suf = gera_sufixos(str);
    organiza_sufixos(vet_suf, str->len);
    // imprime_sufixos(vet_suf, str->len);

    le_query(vet_suf, str->len, contexto);

    libera_sufixos(vet_suf);
    libera_string(str);
    fclose(f);

    return 0;
}

int compara_de(string* s, string* t, int i1, int i2) {
    int min = (s->len - i1) < (t->len - i2) ? (s->len - i1) : (t->len - i2);

    for (int i = 0; i < min; i++) {
        if (s->c[i + i1] < t->c[i + i2])
            return -1;
        if (s->c[i + i1] > t->c[i + i2])
            return 1;
    }

    return (s->len - i1) - (t->len - i2);
}

static int compara_sufixos(const void* s1, const void* s2) {
    string* str1 = ((suffix*) s1)->s;
    string* str2 = ((suffix*) s2)->s;
    int i1 = ((suffix*) s1)->index;
    int i2 = ((suffix*) s2)->index;

    return compara_de(str1, str2, i1, i2);
}

string* le_entrada(FILE* f, int n) {
    string* str = (string*) malloc(sizeof(string));
    str->c = (char*) malloc((n+1)*sizeof(char));
    str->len = 0;

    char c_atual = '\0', c_ant = '\0';
    int i = 0, j = 0;
    for(i = 0; i < n; i++) {
        fscanf(f, "%c", &c_atual);

        if(c_atual == ' ' && (c_ant == ' ' || c_ant == '\n'))
            continue;
        else if(c_atual == '\n' && (c_ant == ' ' || c_ant == '\n'))
            continue;
        else if(c_atual == '\n')
            str->c[str->len++] = ' ';
        else
            str->c[str->len++] = c_atual;
        
        c_ant = c_atual;
    }
    str->c[str->len] = '\0';

    return str;
}

void libera_string(string* str) {
    free(str->c);
    free(str);
}

void libera_vetor_sufixos(suffix* vet_suf) {
    free(vet_suf);
}

suffix* gera_sufixos(string* str) {
    suffix*  vet_suf = (suffix*) malloc(str->len*sizeof(suffix));

    for(int i = 0; i < str->len; i++) {
        vet_suf[i].s = str;
        vet_suf[i].index = i;
    }
    
    return vet_suf;
}

void libera_sufixos(suffix* s) {
    free(s);
}

void organiza_sufixos(suffix* s, int tam) {
    qsort(s, tam, sizeof(suffix), compara_sufixos);
}

void imprime_sufixos(suffix* vet_suf, int tam) {
    for(int i = 0; i < tam; i++)
        printf("%d: %s\n", i, vet_suf[i].s->c + vet_suf[i].index);
}

void le_query(suffix* vet_suf, int tam, int contexto) {
    char* linha = NULL;
    int tam_lido = 0;
    size_t size = 0;
    
    string* str = (string*) malloc(sizeof(string));
    while((tam_lido = getline(&linha, &size, stdin)) > 1) {
        str->c = strtok(linha, "\n");
        str->len = tam_lido - 1;
        query(vet_suf, tam, str, contexto);
    }

    free(linha);
    free(str);
}

void query(suffix* vet_suf, int tam, string* str, int contexto) {
    int i = busca_binaria(vet_suf, 0, tam - 1, str);

    if(i >= 0) {
        printf("%s\n", vet_suf[i].s->c + vet_suf[i].index);
        i++;

        while((i < tam) && contem(vet_suf[0].s, str, vet_suf[i].index, 0)) {
            imprime_string_intervalo(vet_suf[0].s, max(vet_suf[i].index - contexto, 0), min(vet_suf[i].index + contexto, tam - 1));
            i++;
        }
    }
    else
        printf("Nao encontrado!\n");
}

//retorna 1 se s1 contem s2 a partir da origem, e 0 caso contrario
int contem(string* s1, string* s2, int i1, int i2) {
    if((s2->len - i2) > (s1->len - i1))
        return 0;
    
    for(int i = 0; i < (s2->len - i2); i++)
        if(s1->c[i + i1] != s2->c[i + i2])
            return 0;
        
    return 1;
}

int busca_binaria(suffix* vet_suf, int lo, int hi, string* k) {
    int mid = lo + (hi-lo)/2;

    // printf("================================================================\n");
    // printf("lo: %d, mid: %d, hi: %d\n", lo, mid, hi);
    // imprime_string(k);
    // imprime_string(vet_suf[hi].s);
    // int a = contem(vet_suf[hi].s, k,  vet_suf[hi].index, 0);
    // if(a)
    //     printf("Contem!\n");
    // else
    //     printf("Nao contem!\n");
    // printf("================================================================\n");

    if((hi <= lo) && (!contem(vet_suf[lo].s, k, vet_suf[lo].index, 0)))
        return -1;

    if(contem(vet_suf[lo].s, k,  vet_suf[lo].index, 0))
        return lo;

    if(contem(vet_suf[mid].s, k,  vet_suf[mid].index, 0)) {
        if(compara_de(vet_suf[mid].s, k, vet_suf[mid].index, 0) > 0)
            return busca_binaria(vet_suf, lo + 1, mid, k);
        else
            return mid;
    }        

    if(compara_de(vet_suf[mid].s, k, vet_suf[mid].index, 0) > 0)
        return busca_binaria(vet_suf, lo, mid - 1, k);
    else
        return busca_binaria(vet_suf, mid + 1, hi, k);    
}

void imprime_string(string* str) {
    printf("string: %s, tam: %d\n", str->c, str->len);
}

void imprime_string_intervalo(string* str, int i1, int i2) {
    if(i1 < 0) {
        printf("Indice menor que zero! - imprime_string_intervalo()\n");
        exit(1);
    }

    if(i2 >= str->len) {
        printf("Indice maior que o indice maximo - imprime_string_intervalo()\n");
        exit(1);
    }

    for(int i = i1; i < i2; i++)
        printf("%c", str->c[i]);
    printf("\n");
}