#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Project37032_funcoes_auxiliares.h"


int main(){

    FILE *f, *keywords;
    Mensagem* lista_msgs=NULL, *msg_em_analise=NULL, *auxiliar;
    Rank *ranking=NULL;
    char *msg=NULL, **palavras_chave=NULL;
    int tot_p_chave=0, i, c, **findings=NULL, *tot_finds, msg_numero=0;
    double tempo_pesquisa;
 	clock_t inicio, fim;

 	//*******Variáveis de menu*******//
 	int choice, rank;
 	char filename[30], filename2[30];

 	do{
        printf ("|----------------|\n");
        printf ("| Menu principal |\n");
        printf ("|----------------|\n\n");
        printf ("<1> Carregar ficheiro de mensagens e criar dicionarios\n");
        printf ("<2> Carregar ficheiro de palavras-chave\n");
        printf ("<3> Pesquisar palavras-chave\n");
        printf ("<4> Sair\n");
        printf ("\nEscolha uma opcao: ");
        do{
            scanf("%d", &choice);
        }while(choice!=1&&choice!=2&&choice!=3&&choice!=4);

 	switch(choice)
        {
        case 1: if (lista_msgs!=NULL){system("cls"); printf ("Ja existe um ficheiro de mensagens guardado em memoria!\n\n"); break;}

                //Ler as mensagens do ficheiro e guardar informações em estrutura
                system("cls");
                printf ("|-----------------------|\n");
                printf ("| Ficheiro de mensagens |\n");
                printf ("|-----------------------|\n\n");
                fflush(stdin);
                printf ("Introduza o nome do ficheiro de mensagens: ");
                gets(filename);

                f=fopen(filename, "rb"); if (f==NULL) {printf ("Ficheiro nao existente!\n\n"); break;}
                printf ("A analisar o ficheiro... ");
                lista_msgs=total_mensagens_ficheiro(f);
                printf ("Analise concluida!\n");

                //Criar dicionário das posições de cada letra para cada mensgem
                printf ("A criar dicionario para cada mensagem... ");
                msg_em_analise=lista_msgs;

                while (msg_em_analise!=NULL){
                    msg = le_msg_ficheiro(f,msg_em_analise->pos_msg,msg_em_analise->sizemsg);
                    msg_em_analise->dicc=constroi_dicc(msg,msg_em_analise->sizemsg,&(msg_em_analise->size_each_char));
                    msg_em_analise=msg_em_analise->nseg;
                    free(msg);
                }

                printf ("Processo concluido!\n\n");
                break;

        case 2: if (palavras_chave!=NULL){system("cls"); printf ("Ja existe um ficheiro de palavras-chave guardado em memoria\n\n"); break;}
                //Ler ficheiro de keywords
                system("cls");
                fflush(stdin);
                printf ("|----------------------------|\n");
                printf ("| Ficheiro de palavras-chave |\n");
                printf ("|----------------------------|\n\n");
                printf ("Introduza o nome do ficheiro de palavras-chave: ");
                gets(filename2);

                keywords=fopen(filename2, "r"); if (keywords==NULL) {printf ("Ficheiro nao existente!\n\n"); break;}
                palavras_chave = ler_keywords(keywords,&(tot_p_chave));
                printf ("Ficheiro carregado com sucesso!\n\n");
                break;

        case 3: if (palavras_chave==NULL||lista_msgs==NULL){system("cls"); printf ("Faltam carregar ficheiros para iniciar a pesquisa\n\n"); break;}

                //Pesquisa de palavras nas mensagens
                system("cls");
                printf ("|----------|\n");
                printf ("| Pesquisa |\n");
                printf ("|----------|\n\n");

                msg_em_analise=lista_msgs;
                findings = (int**) malloc(tot_p_chave*sizeof(int*));
                tot_finds = (int*) malloc(tot_p_chave*sizeof(int));
                msg_numero=0;

                inicio=clock();
                printf ("Analisando mensagens 0%%...");

                while (msg_em_analise!=NULL){

                    if (msg_numero==25){printf ("25%%...");}
                    if (msg_numero==50){printf ("50%%...");}
                    if (msg_numero==75){printf ("75%%...");}

                    msg = le_msg_ficheiro(f,msg_em_analise->pos_msg,msg_em_analise->sizemsg);

                    for (i=0; i<tot_p_chave; i++){
                        findings[i] = pesquisa(palavras_chave[i],msg_em_analise->dicc,msg_em_analise->size_each_char,msg_em_analise->sizemsg,msg,&(tot_finds[i]));
                    }
                    //Ordena de forma crescente o espaçamento das palavras encontradas
                    organiza_finds(findings,tot_finds,tot_p_chave);

                    //Vetor de palavras encontradas com messmo espaçamento
                    msg_em_analise->palavras_encontradas = compara_pesquisas(findings,tot_finds,tot_p_chave,&(msg_em_analise->total_palavras),&(msg_em_analise->spacing));

                    //Libertação de recursos
                    free(msg);
                    for (i=0; i<tot_p_chave; i++){
                        free(findings[i]);
                    }

                    msg_numero++;
                    msg_em_analise=msg_em_analise->nseg;
                }

                printf ("100%%\n");
                fim=clock();
                tempo_pesquisa = ((double) (fim-inicio)) / CLK_TCK;
                printf ("Tempo total de pesquisa: %.3f segundos\n\n", tempo_pesquisa);

                //Lista com os ranks de cada mensagem, ordenada de forma decrescente
                ranking = ordena_ranks(lista_msgs);

                //Ver as melhores mensagens
                printf ("Introduza o numero de mensagens mais perigosas que pretende ver: ");
                scanf ("%d", &rank);
                if (rank<1){rank = 1;}
                if (rank>100){rank = 100;}
                printf ("\n");

                for (i=0; i<rank; i++){
                    auxiliar = encontra_msg_numero(lista_msgs,ranking[i].msg);
                    printf ("%d. Mensagem numero: %d, Total de palavras encontradas = %d\nEspacamento: %d, Palavras: ", i+1, ranking[i].msg, auxiliar->total_palavras, auxiliar->spacing);
                    for (c=0; c<auxiliar->total_palavras; c++){
                        printf ("%s, ", palavras_chave[auxiliar->palavras_encontradas[c]]);
                    }
                    printf ("\n");
                }
                printf ("\n\n");
                break;
        }
    }while (choice!=4);

    return 0;
}


