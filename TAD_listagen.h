#include<stdio.h>
#include<string.h>

struct palavras{
	char letra, final;
 	struct palavras *prim;//cabeca
	struct palavras *prox;//cauda
};
typedef struct palavras Palavras;

Palavras *Cons(char letra, char final, Palavras *prim, Palavras *prox){
	Palavras *p = (Palavras*)malloc(sizeof(Palavras));
	p->letra = letra;
	p->final = final;
	p->prim = prim;
	p->prox = prox;
	return p;
}
Palavras *buscaLetra(Palavras *P, char letra){
	while(P!=NULL && P->letra!=letra)
		P = P->prox;
	return P;	
}
Palavras *criaPlv(char *plv, int i){
	if(i+1<strlen(plv))
		return Cons(plv[i],'F',criaPlv(plv,i+1),NULL);
	return Cons(plv[i],'T',NULL,NULL);
}

void inserePlv(Palavras **P, char plv[]){
	Palavras *pc;
	Palavras *aux = *P;
	int i=0;
	if(*P==NULL)
		*P = criaPlv(plv,0);
	else if(plv[0] < (*P)->letra){
		pc = criaPlv(plv,0);
		pc->prox = *P;
		*P = pc;
	}else{
		pc = buscaLetra(aux,plv[0]);
		if(pc!=NULL){
			aux=pc;
			while(pc != NULL && i+1<strlen(plv)){
				i++;
				pc = buscaLetra(pc->prim,plv[i]);
				if(pc!=NULL)
					aux = pc;
			}
			if(pc==NULL){
				pc = criaPlv(plv,i);
				if(aux->prim == NULL){
					aux->prim = pc;
				}
				else{
					if(plv[i] < aux->prim->letra){
						pc->prox = aux->prim;
						aux->prim = pc;
					}
					else{	
						aux = aux->prim;
						while(aux->prox!=NULL && plv[i] > aux->prox->letra ){
							aux = aux->prox;
						}
						pc->prox = aux->prox;
						aux->prox = pc;	
					}
				}
			}
			else{
				pc->final='T';
			}
	
		}else{
			pc = criaPlv(plv,0);
			while(aux->prox!=NULL && plv[0] > aux->prox->letra ){
				aux = aux->prox;
			}
			pc->prox = aux->prox;
			aux->prox = pc;	
		}
		
	}
}

void sugerir(Palavras **arvore, char palavra[]){
	
	Palavras *pc;
	pc = *arvore;
	int i=0, tl=0;
	if(strlen(palavra)>0 && arvore!=NULL){
		pc = buscaLetra(pc,palavra[i]);
		while(pc != NULL && i<strlen(palavra)){
			pc = pc->prim;
			i++;
			if(i<strlen(palavra))
				pc = buscaLetra(pc,palavra[i]);
		}
		if(i==strlen(palavra)){
			while(pc!=NULL && pc->prox==NULL  && pc->final == 'F'){
				palavra[tl++] = pc->letra;
				pc = pc->prim;
			}
			
			if(pc!=NULL && pc->prox==NULL && pc->final=='T'){
				palavra[tl++] = pc->letra;
				palavra[tl]='\0';
			}
			else if (pc==NULL){
				palavra[tl]='\0';
			}
			else if(pc->prox != NULL) // tem bifurcação
				palavra[0]='\0';
										
			
		}else
			palavra[0]='\0';
	}else
			palavra[0]='\0';
	
}

void destruirLG(Palavras **P){
	if(*P!=NULL){
		destruirLG(&(*P)->prim);
		destruirLG(&(*P)->prox);
		free(*P);
		*P = NULL;
	}
}

void ExibeLG(Palavras *P){
	if(P!=NULL){
		printf("%c",P->letra);
		ExibeLG(P->prim);
 		ExibeLG(P->prox);

	}
}
