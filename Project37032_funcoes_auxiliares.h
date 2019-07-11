#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_MAX 999999999
#define minimo(a,b) a<b? a:b
#define SIZE_WORD 30

typedef struct MENSAGEM{
    int pos_msg;
    int sizemsg;
    int **dicc;
    int *size_each_char;
    int spacing;
    int *palavras_encontradas;
    int total_palavras;
    struct MENSAGEM *nseg;
}Mensagem;

typedef struct{
    int msg;
    int total;
}Rank;


//Read files
Mensagem* total_mensagens_ficheiro(FILE*);
Mensagem* makeNode();
Mensagem* insertLast(Mensagem*,Mensagem*);

char** ler_keywords (FILE*,int*);

//Read message
char* le_msg_ficheiro (FILE*,int,int);
//Funções auxiliares
Mensagem* encontra_msg_numero (Mensagem*,int);
int total_msgs (Mensagem *v);

//Build dictionary e funcoes auxiliares
int** constroi_dicc(char*,int,int**);

//Funcoes pesquisa
int* pesquisa(char*,int**,int*,int,char*,int*);
int* compara_pesquisas (int**,int*,int,int*,int*);
void organiza_finds (int**,int*,int);
int* insertSort(int*,int);
int existe_numero (int*,int,int);

//Função de organização de ranks
Rank* ordena_ranks(Mensagem*);

Mensagem* total_mensagens_ficheiro(FILE *f)
{
    Mensagem *v=NULL, *aux;
    unsigned char check;
    int tot=0, pos_cursor=0;

    while (fread(&check, 1, 1, f)){
        tot++;
        if (check == 10) {
            aux=makeNode();
            aux->pos_msg=pos_cursor;
            aux->sizemsg=tot;
            v=insertLast(v,aux);
            tot=0;
            pos_cursor=ftell(f);
        }
    }

    rewind(f);
    return v;
}

//Funções auxiliares para construção de lista de mensagens

Mensagem* makeNode()
{
    Mensagem*v=malloc(sizeof(Mensagem));
    v->pos_msg=0;
    v->sizemsg=0;
    v->size_each_char=NULL;
    v->dicc=NULL;
    v->nseg=NULL;
    v->palavras_encontradas=NULL;
    v->total_palavras=0;
    v->spacing=-1;
    return v;
}

Mensagem* insertLast(Mensagem *v,Mensagem *nv)
{
    Mensagem *aux=v;
    if (v==NULL){return nv;}
    while (aux->nseg!=NULL){
        aux=aux->nseg;
    }
    aux->nseg=nv;
    return v;
}

//Termino de funções auxiliares


//Função de leitura de mensagem começando numa determindada pos e com determinado tamanho (sizemsg)

char* le_msg_ficheiro (FILE *f, int pos, int sizemsg) //le uma mensagem
{
    char *msg=NULL;

    fseek(f,pos,SEEK_SET); //set cursor in position to read message

    msg=(char*)malloc(sizemsg*sizeof(char));

    fread(msg, sizemsg, 1, f);

    return msg;
}

// Funções auxiliares de procura em lista de mensagens

// Função de procura de msg pelo seu numero

Mensagem* encontra_msg_numero (Mensagem *v,int numero)
{
    int tot=0;
    while (v!=NULL){
        if (tot==numero){break;}
        tot++;
        v=v->nseg;
    }
    return v;
}

// Função de contagem do numero de mensagens

int total_msgs (Mensagem *v)
{
    int tot=0;
    while (v!=NULL){
        tot++;
        v=v->nseg;
    }
    return tot;
}

// Término de funções auxiliares

//Contrucao de dicionario
int** constroi_dicc(char* msg, int sizemsg, int **size_each_char)
{
    int **v=malloc(26*sizeof(int*)), *totv=calloc(26, sizeof(int)), i, c, aux=0;
    char ch;
    for (i=0; i<26; i++){
        ch=(char)(i+97);
        for (c=0; c<sizemsg; c++){
            if (msg[c]==ch){totv[i]++;}}
    }

    for (i=0; i<26; i++){
        v[i]=malloc(totv[i]*sizeof(int));
        ch=(char)(i+97);
        aux=0;
        for (c=0; c<sizemsg; c++){
            if (msg[c]==ch){v[i][aux]=c; aux++;}
        }
    }
    *size_each_char=totv;
    return v;
}

// Função de leitura de ficheiro de keywords

char** ler_keywords (FILE *f, int *tot_aux)
{
    int tot=0, i;
    char **v=NULL, temp[SIZE_WORD];

    //Ver total de palavaras
    while (fgets(temp,SIZE_WORD,f)){
        tot++;
    }

    //Alocar memória para as receber em vetor
    v=malloc(tot*sizeof(char*));
    for (i=0; i<tot; i++){
        v[i]=malloc(SIZE_WORD*sizeof(char));
    }

    //Copiar palavras de ficheiro para vetor
    rewind (f);
    tot=0;
    while (fgets(temp,SIZE_WORD,f)){
        strcpy(v[tot],temp);
        v[tot][strlen(temp)-1]='\0'; //add terminator char
        tot++;
    }
    *tot_aux=tot;
    return v;
}


//Funcoes de pesquisa de words

int* pesquisa(char *word, int **v, int *totv, int sizemsg, char* msg, int *tot_aux)
{
    //Declaração de variáveis*************************************************************
    int i, c, j, sizeWord=strlen(word), space=0, posFirstLetter;
    char lastChar = word[sizeWord-1];
    int a=word[0]-97, b=word[1]-97, *secondLetter=v[b], *firstLetter=v[a];
    int aux=0, totFirstLetter=totv[a], totSecondLetter=totv[b];
    int *finds=NULL, totf=0;
    //************************************************************************************

    for (i=0; i<totFirstLetter; i++){

        posFirstLetter=firstLetter[i];

        //Atualizacao da posicao da segunda letra relativamente ao avanco da primeira
        while (secondLetter[aux]<posFirstLetter){aux++;}

        for (c=aux; c<totSecondLetter; c++){
            space=secondLetter[c]-posFirstLetter;
            if ((secondLetter[c]+(sizeWord-2)*space)>sizemsg){break;}

            //Ver se a ultima letra existe na mensagem com este espacamento
            if (msg[posFirstLetter+(sizeWord-1)*space]!=lastChar){continue;}

            for (j=2; j<sizeWord-1; j++){
                if (msg[posFirstLetter+j*space]!=word[j]){break;}
            }
            //Encontrou uma palavra
            if (j==sizeWord-1){
                finds=realloc(finds,++totf*sizeof(int));
                finds[totf-1]=space;
            }

        }

    }
    *tot_aux=totf;
    return finds;
}

// Funções para verificar quantas palavras existem com mesmo espaçamento na msg

void organiza_finds (int **v,int *totv,int tot_palavras)
{
    int i;
    for (i=0; i<tot_palavras; i++){
        v[i]=insertSort(v[i], totv[i]);
    }
}

int* compara_pesquisas (int **finds,int *tot_finds, int tot_words, int *tot_aux, int *spacing)
{
    int i, j, k, min_spacing=-1, tot, max_finds=1;
    int *v=NULL;

    for (i=0; i<tot_words; i++){
        for (j=0; j<tot_finds[i]; j++){
            tot=1;
            for (k=i+1; k<tot_words; k++){
                if (existe_numero(finds[k],tot_finds[k],finds[i][j])){
                    tot++;
                    if (tot==max_finds){
                        if (finds[i][j]<min_spacing){
                            min_spacing=finds[i][j];
                        }
                    }
                    if (tot>max_finds){
                        max_finds=tot;
                        min_spacing=finds[i][j];
                    }
                }
            }
        }
    }

    //Neste caso não existem palavras com mesmo espacamento ou não existem palavras encontradas
    if (min_spacing==-1){
        //Verificar se existem palavras encontradas
        for (i=0; i<tot_words; i++){
            if (tot_finds[i]!=0){break;}
        }
        if (i==tot_words){return NULL;}

        //Existem palavras, por isso vamos encontrar a que tem menor espaçamento
        min_spacing=INT_MAX;
        for (i=0; i<tot_words; i++){
            if (tot_finds[i]==0){continue;}
            min_spacing=minimo(min_spacing,finds[i][0]);
        }
    }

    //Contruir o vetor com as palavras de espaçamento minimo
    tot=0;
    for (i=0; i<tot_words; i++){
        if (existe_numero(finds[i],tot_finds[i],min_spacing)){
            v=realloc(v,++tot*sizeof(int));
            v[tot-1]=i;
        }
    }

    *tot_aux=max_finds;
    *spacing=min_spacing;
    return v;
}

int existe_numero (int *v,int tot,int numero)
{
    int i;
    for (i=0; i<tot; i++){
        if (v[i]>numero){break;}
        if (v[i]==numero){return 1;}
    }
    return 0;
}

int *insertSort(int *v, int tot)
{
    int i, j, k, aux;
    for (i=0; i<tot; i++){
        for (j=0; j<i; j++){
            if (v[j]>v[i]){break;}
        }
        aux=v[i];
        for (k=i; k>=j; k--){
            v[k]=v[k-1];
        }
        v[j]=aux;
    }
    return v;
}

//Função de ordenação de ranks

Rank* ordena_ranks(Mensagem *M)
{
    int tot = total_msgs(M), i, j, k;
    Rank *v = malloc(tot*sizeof(Rank)), aux;

    for (i=0; i<tot; i++){
        v[i].msg=i;
        v[i].total=encontra_msg_numero(M,i)->total_palavras;
    }

    for (i=0; i<tot; i++){
        for (j=0; j<i; j++){
            if (v[j].total<v[i].total){break;}
            if (v[j].total==v[i].total){
                if (encontra_msg_numero(M,v[j].msg)->spacing>encontra_msg_numero(M,v[i].msg)->spacing){break;}
            }
        }
        aux=v[i];
        for (k=i; k>=j; k--){
            v[k]=v[k-1];
        }
        v[j]=aux;
    }
    return v;

}
