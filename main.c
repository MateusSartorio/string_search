#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void imprime_sufixos(suffix* s, int tam);
void le_query(suffix* s);
void query();
int binary_search(suffix* vet_suf, int lo, int hi, string* k);
int contem(string* s1, string* s2);

int main(int argc, char** argv) {
    FILE* f = fopen(argv[1], "r");

    if(f == NULL) {
        printf("Erro na abertura do arquivo!\n");
        exit(1);
    }

    int n = 0;
    fscanf(f, "%d\n", &n);

    string* str = le_entrada(f, n);

    suffix* s = gera_sufixos(str);
    organiza_sufixos(s, str->len - 1);
    // imprime_sufixos(s, str->len - 1);

    le_query(s);

    // int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 32, 43, 94, 102, 305, 987};
    // int i = binary_search(a, 0, 15, atoi(argv[2]));
    // i >= 0 ? printf("%d\n", a[i]) : printf("Elemento nao encontrado!\n");

    // char* s1 = "coo";
    // char* s2 = "cool";

    // contem(s1, s2, strlen(s1), strlen(s2)) ? printf("Contem!\n") : printf("Nao contem!\n");

    // string* s1 = (string*) malloc(sizeof(string));
    // s1->c = "A";
    // s1->len = 7;

    // string* s2 = (string*) malloc(sizeof(string));
    // s2->c = "B";
    // s2->len = 5;

    // int i = compara_de(s1, s2, 0, 0);
    // printf("%d\n", i);

    libera_sufixos(s);
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
    str->c[str->len++] = '\0';

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
    suffix*  vet_suf = (suffix*) malloc((str->len - 1)*sizeof(suffix));

    for(int i = 0; i < str->len - 1; i++) {
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

void imprime_sufixos(suffix* s, int tam) {
    for(int i = 0; i < tam; i++)
        printf("%s\n", s[i].s->c + s[i].index);
}

void le_query(suffix* s) {
    char* linha = NULL;
    int tam = 0;
    size_t size = 0;
    
    string* str = (string*) malloc(sizeof(string));
    while((tam = getline(&linha, &size, stdin)) > 1) {
        printf("%d\n", tam);
        str->c = linha;
        str->len = tam - 1;
        query(s, str);
    }

    free(linha);
}

void query(suffix* s, string* str) {

}

//retorna 1 se s1 contem s2 a partir da origem, e 0 caso contrario
int contem(string* s1, string* s2) {
    if(s2->len > s1->len)
        return 0;
    
    for(int i = 0; i < s2->len; i++)
        if(s1->c[i] != s2->c[i])
            return 0;
        
    return 1;
}

int binary_search(suffix* vet_suf, int lo, int hi, string* k) {
    int mid = lo + (hi-lo)/2;

    if((hi == lo) && (!contem(vet_suf[lo].s, k)))
        return -1;

    if(contem(vet_suf[mid].s, k))
        return mid;
    else if(compara_de(vet_suf[mid].s, k, vet_suf[mid].index, 0) > 0)
        return binary_search(vet_suf, lo, mid - 1, k);
    else
        return binary_search(vet_suf, mid + 1, hi, k);
}