#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include "meuconio.h"

struct letra{
	struct letra *ant, *prox;
	char letra, negrito;
};
typedef struct letra Letra;

struct linha{
	struct linha *top,*botton;
	Letra *inicioL;
	int nro;
};
typedef struct linha Linha;

struct cursor{
	Linha *linha;
	Letra *letra;
	int pos, y;
};
typedef struct cursor Cursor;
void renderizarTela(Linha *inicio, Linha *primeira, Linha *ultima, Cursor *cursor, int *lin1, int *recEsq, int *recDir, char **negrito);
Letra *ultimaLetra(Letra *L);
int ultimaLinhaParagrafo(Linha *L);
int PosicaoLetra(Letra *L);
void removeEspacos(Linha *inicio, Cursor **cursor, int *lin1, int *recEsq);

void printC(int colIni, int colFim, int y, char frase[]){ 
     int x = ((colFim+colIni)/2)-(strlen(frase)/2);
     if(x<0) x=1;
     gotoxy(x,y);
     printf("%s",frase);
}
void tela(char titulo[]){
	int i;
    clrscr();
    
    //horizontal
    for(i=2;i<=79;i++){
    	gotoxy(i,2);		printf("%c",205);
    	gotoxy(i,25);		printf("%c",205);
    }
    //vertical
    for(i=3;i<=24;i++){
    	gotoxy(1,i);		printf("%c",186);
    	gotoxy(80,i);		printf("%c",186);
    }
    //cantos
    gotoxy(1,2);		printf("%c",201);
    gotoxy(80,2);		printf("%c",187);
    gotoxy(1,25);		printf("%c",200);
    gotoxy(80,25);		printf("%c",188);
    
    printC(1,80,1,titulo);
}
void aviso(char frase[]){
	tela("(!) AVISO (!)");
    //informacoes
	printC(1,80,12,frase);
	getch();
}
void pintaLinha(int yi, int yf, int color){
	textbackground(color);
	while(yi<=yf){
		gotoxy(1,yi);
		printf("                                                                                ");
		yi++;
	}
}
void desenhaLinha(int y, char caractere){
	int i;
	gotoxy(1,y);
	for(i=1; i<=80; i++)
		printf("%c",caractere);
}
void atualizaRodape(Cursor *cursor, char insert, Linha *inicio, char negrito){
	pintaLinha(24,26,9);
	gotoxy(2,25);
	textcolor(15);
	printf("COL: %d, LIN: %d |",cursor->pos, distanciaLinha(inicio,cursor->linha)+1);
	if(!insert)
		textcolor(10);
	printf(" INSERT |");
	textcolor(15);
	if(negrito)
		textcolor(10);
	printf(" NEGRITO(F10)");
	textcolor(15);
	printf(" ||||");
	desenhaLinha(24,'-');
	textbackground(0);
	gotoxy(cursor->pos, cursor->y);
	
}
void cabecalho(){
	pintaLinha(1,2,9);
	textcolor(15);
	gotoxy(1,1);
	printf("F2-Abrir F3-Salvar F4-Sair F5-Exibir");
	int i;
	for(i=1;i<=80;i++){
		gotoxy(i,2);
		printf("-");
	}
	textbackground(0);
}
Letra* criaLetra(char letra){
	Letra *nc = (Letra*)malloc(sizeof(Letra));
	nc->letra = letra;
	nc->ant = NULL;
	nc->prox = NULL;
	return nc;
}
Linha* criaLinha(Letra *l){
	Linha *nc = (Linha*)malloc(sizeof(Linha));
	nc->top=NULL;
	nc->botton=NULL;
	nc->nro=0;
	nc->inicioL=l;

	while(l!=NULL && l->letra!='\n'){
		nc->nro = nc->nro+1;
		l = l->prox;
	}
	return nc;
}

void InicializaEditor(Linha **L, Linha **primeira, Linha **ultima, Cursor **cursor, int *lin1){
	*L = criaLinha(criaLetra('\n'));
	*primeira = *ultima = *L;
	(*cursor)->linha = *L;
	(*cursor)->letra = (*L)->inicioL; //caixinha que o cursor aponta
	(*cursor)->pos=1;
	(*cursor)->y=3;
	//the quick brown fox jump over the lazy dog
}

int distanciaLinha(Linha *inicio, Linha *l){
	int i = 0;
	while(inicio!=l && inicio != NULL){
		i++;
		inicio = inicio->botton;
	}
	if(inicio == l)
		return i;
	else 
		return -1;
}

void insereLetra(char letra, Cursor **cursor, int **linha1, int **recEsq, int **recDir, Linha *inicio){

	Letra *l;
	int espaco, esquerda;
	
	if(primeiraLinhaParagrafo((*cursor)->linha)){
		espaco = **recDir - **linha1;
		esquerda = **linha1;
	}
	else{
		espaco = **recDir - **recEsq;
		esquerda = **recEsq;
	}
	
	if(**linha1>0 || **recEsq>0 || **recDir!=79)
		removeEspacos(inicio,&*cursor,*linha1,*recEsq);
		
	if((*cursor)->linha->nro < espaco){
		
		if((*cursor)->letra->ant == NULL){
			l = criaLetra(letra);
			l->prox = (*cursor)->letra;
			(*cursor)->linha->inicioL=l;
			(*cursor)->letra->ant = l;
		}
		else{
			if((*cursor)->letra->ant->letra == 21 && letra == 21)
			{
				l = (*cursor)->letra->ant->ant;
				free((*cursor)->letra->ant);
				if(l!=NULL){
					l->prox = (*cursor)->letra;
					(*cursor)->letra->ant = l;
				}
				else{
					(*cursor)->linha->inicioL = (*cursor)->letra;
					(*cursor)->letra->ant = NULL;
				}
			}else{
				l = criaLetra(letra);
				l->prox = (*cursor)->letra;
				(*cursor)->letra->ant->prox=l;
				l->ant=(*cursor)->letra->ant;
				(*cursor)->letra->ant = l;
			}
		}
		
		
		if(letra!=21){
			(*cursor)->linha->nro=(*cursor)->linha->nro+1;
		}
	}
	
	(*cursor)->pos=PosicaoLetra((*cursor)->letra);
}
void substituiLetra(char tecla, Cursor **cursor){
	(*cursor)->letra->letra = tecla;
	(*cursor)->letra = (*cursor)->letra->prox;
	(*cursor)->pos = (*cursor)->pos+1;

}

int qtdPalavras(Letra *L){
	int qtd = (L==NULL)?0:1;
	while(L!=NULL){
		if(L->letra == ' ' && L->prox->letra!='\n')
			qtd++;
		L = L->prox;
	}
	return qtd;
}

int primeiraLinhaParagrafo(Linha *L){
    if(L->top==NULL)
        return 1;
    if(ultimaLinhaParagrafo(L->top))
        return 1;
    return 0;
}

int ultimaLinhaParagrafo(Linha *L){
	Letra *letra = L->inicioL;
	letra = ultimaLetra(letra);
	if(letra->ant==NULL)
		return 0;
	letra = letra->ant;
	if(letra->letra==21 && letra->ant!=NULL)
		letra = letra->ant;
	if(letra->letra=='.' || letra->letra=='!' || letra->letra=='?')
		return 1;
	return 0;		
}

void removeEspacos(Linha *inicio, Cursor **cursor, int *lin1, int *recEsq){
	Letra *letra, *aux;
	int posEspaco;
	
	while(inicio!=NULL){
		if(primeiraLinhaParagrafo(inicio))
			posEspaco = *lin1+1;
		else
			posEspaco = *recEsq+1;
			
		letra = inicio->inicioL;
		while(letra!=NULL){
			if(letra->letra==' '){
					
				if(letra->ant==NULL){
					if((*cursor)->letra == letra)
						(*cursor)->letra = (*cursor)->letra->prox;
					aux = letra;
					inicio->inicioL = letra->prox;
					inicio->inicioL->ant = NULL;
					free(aux);
					letra = NULL;
					inicio->nro = inicio->nro-1;
				}
				else{
					letra = letra->prox;
					while(letra->letra==' '){
						if((*cursor)->letra == letra)
							(*cursor)->letra = (*cursor)->letra->prox;
						aux = letra;
						letra->ant->prox = letra->prox;
						letra->prox->ant = letra->ant;
						letra = letra->prox;
						free(aux);
						inicio->nro = inicio->nro-1;
					}
				}
			}
			if(letra!=NULL)
				letra = letra->prox;
			else
				letra = inicio->inicioL;
			posEspaco++;
		}
		(*cursor)->pos = PosicaoLetra((*cursor)->letra);
		inicio = inicio->botton;
	}
}
Letra *letraPos(Linha *L, int pos){
	Letra *letra = L->inicioL;
	while(letra->prox!=NULL && pos>0){
		letra = letra->prox;
		pos--;
	}
	return letra;
}

int PosicaoLetra(Letra *L){
	int i=0;
	while(L!=NULL){
		if(L->letra!=21)
			i++;
		L = L->ant;
	}
	if(i==0) i=1;
	return i;
}

int qtdEspacos(Letra *L){
	int i=0;
	while(L!=NULL){
		if(L->letra==' ')
			i++;
		L=L->prox;
	}
	return i;
}
void formataTexto(Linha **inicio, Linha **primeira, Linha **ultima, Cursor **cursor, int *lin1, int *recEsq, int *recDir){
	Linha *auxInicio, *auxLin;
	Letra *auxL, *auxLetra, *nova, *resto, *primeiraLetra;
	auxInicio = *inicio;
	int parag=1, novalinha, temEspaco=0, espDisponivel,  qtdLetras = *recDir, i, nro, qtdEsp;
	
	removeEspacos(*inicio, &*cursor, lin1, recEsq);
	while(auxInicio!=NULL){
		resto = NULL;
		novalinha = 0;
		if(parag){
			parag = 0;
			qtdEsp=*lin1;
		}
		else
			qtdEsp=*recEsq;
		i = qtdEsp;
		primeiraLetra = auxInicio->inicioL;
		auxL = primeiraLetra;
		
		if(auxL->letra!='\n'){
			while(i>0){//cursor pos não incrementa
				auxLetra = criaLetra(' ');
				auxLetra->prox = auxInicio->inicioL;
				auxInicio->inicioL->ant = auxLetra;
				auxInicio->inicioL = auxLetra;
				auxInicio->nro = auxInicio->nro+1;
				i--;
			}
		}
		while(auxInicio->nro>qtdLetras){
			i=qtdEsp+1;
			nro = qtdLetras;
			while(i<qtdLetras && auxL!=NULL){
				if(auxL->letra==' '){
					resto = auxL->prox;
					nro = i;
				}
				auxL = auxL->prox;
				i++;
			}
			if(resto==NULL)//dimuir ultima e mudar linha para que o cursor aponta
				resto = auxL->prox;
			resto->ant->prox = criaLetra('\n');
			resto->ant->prox->ant = resto->ant;
			resto->ant = NULL;
			auxLin = criaLinha(resto);
			auxLin->botton = auxInicio->botton;
			auxLin->top = auxInicio;
			auxInicio->botton = auxLin;
			if(auxLin->botton!=NULL)
				auxLin->botton->top = auxLin;
			else
				*ultima = auxLin;
			auxInicio->nro = nro;
			novalinha=1;
			if((*cursor)->linha==auxInicio){
				(*cursor)->linha = auxLin;
			}
			(*cursor)->y = (*cursor)->y+1;
			(*cursor)->pos = PosicaoLetra((*cursor)->letra);
		}
			
		if(auxInicio->botton!=NULL && !ultimaLinhaParagrafo(auxInicio) && auxInicio->nro>0){
			if(!novalinha){
				auxLetra = auxInicio->botton->inicioL;
				if(auxLetra->letra!='\n'){ 	
					auxL = ultimaLetra(auxL); auxL = auxL->ant;			
					if(auxL->letra!=' ' && auxInicio->nro<qtdLetras){
						nova = criaLetra(' ');
						nova->prox = auxL->prox;
						auxL->prox->ant = nova;
						auxL->prox = nova;	
						nova->ant = auxL;
						auxInicio->nro = auxInicio->nro+1;
						auxL = auxL->prox;
					}
					i=0;
					while(i<qtdLetras - auxInicio->nro && auxLetra !=NULL){//insere palavras de baixo
						i=0;
						while(auxLetra->letra!=' ' && auxLetra->letra!='.' && auxLetra->letra!='!' && auxLetra->letra!='?' && auxLetra->letra!='\n'){
							auxLetra = auxLetra->prox;
							i++;
						}
						if(auxLetra->letra!='\n'){
							if(auxLetra->letra!=' ')
								i++;
							auxLetra = auxLetra->prox;
							int k=i;
							if(i<=qtdLetras - auxInicio->nro){
								while(k>0){
									auxInicio->botton->inicioL = auxInicio->botton->inicioL->prox;
									auxInicio->botton->inicioL->ant->prox = auxL->prox;
									auxL->prox->ant = auxInicio->botton->inicioL->ant;
									auxL->prox = auxInicio->botton->inicioL->ant;
									auxInicio->botton->inicioL->ant->ant = auxL;
									auxInicio->botton->inicioL->ant = NULL;
									auxInicio->nro = auxInicio->nro+1;
									auxInicio->botton->nro = auxInicio->botton->nro-1;
									k--;
									auxL = auxL->prox;
								}
							}
						}else
							i = qtdLetras - auxInicio->nro;
						
						
					}
					if(auxInicio->botton->nro==0 || (auxInicio->botton->nro==1 && auxInicio->botton->inicioL->letra==' ')){
						
						auxLin = auxInicio->botton;
						auxInicio->botton = auxLin->botton;
						if(auxLin->botton!=NULL)
							auxLin->botton->top = auxInicio;
						if((*cursor)->linha == auxLin){
							(*cursor)->linha = auxInicio;
							(*cursor)->letra = ultimaLetra((*cursor)->linha->inicioL);
							(*cursor)->y = (*cursor)->y-1;
						}
						if(auxLin == *ultima)
							*ultima = (*ultima)->top;
						if(auxLin->nro==1)
							free(auxLin->inicioL->prox);
						free(auxLin->inicioL);
						free(auxLin);
					}
				}
			}
			auxL = primeiraLetra;
			while(auxInicio->nro<qtdLetras && qtdEspacos(auxInicio->inicioL)>qtdEsp){
				if(auxL!=NULL && (auxL->letra=='.' || auxL->letra=='!' || auxL->letra=='?') && auxL->prox!=NULL && auxL->prox->letra=='\n')
					parag=1;
	
				if(auxL!=NULL && auxL->letra==' ' && auxL->ant!=NULL){
					nova = criaLetra(' ');
					nova->prox = auxL->prox;
					auxL->prox->ant = nova;
					auxL->prox = nova;	
					nova->ant = auxL;
					auxL = auxL->prox;
					auxInicio->nro = auxInicio->nro+1;
					while(auxL->prox!=NULL && auxL->prox->letra==' ')
						auxL = auxL->prox;
				}
				if(auxL!=NULL)
					auxL = auxL->prox;
				else
					auxL = primeiraLetra;
			}
		}else if(ultimaLinhaParagrafo(auxInicio)){
			parag=1;
		}
		auxInicio = auxInicio->botton;
	}
	while(distanciaLinha(*primeira,*ultima)>20)
		*ultima = (*ultima)->top;
		
	(*cursor)->pos = PosicaoLetra((*cursor)->letra);
		
	if((*cursor)->letra == NULL){
		(*cursor)->letra = ultimaLetra((*cursor)->linha->inicioL);
		(*cursor)->pos = (*cursor)->linha->nro+1;
	}
}

void renderizarTela(Linha *inicio, Linha *primeira, Linha *ultima, Cursor *cursor, int *lin1, int *recEsq, int *recDir, char **negrito){ 
	
	Letra *aux; Linha *p = primeira;
	cabecalho();
	pintaLinha(3,23,7);
	textcolor(0);
	gotoxy(1,3);
	int cor=0, y=3, parag=1;
	**negrito = 0;
	while(primeira != ultima->botton){
		aux = primeira->inicioL;
		while(aux!=NULL){
			
			if(primeira!=ultima || aux->letra!='\n'){
				if(aux->letra == 21){
					**negrito = !(**negrito);
					if(cor==0)
						cor = 9;
					else
						cor = 0;
					textcolor(cor);
				}
				else{
					printf("%c",aux->letra);
				}
				
			}	
			aux = aux->prox;
		}
		primeira=primeira->botton;
	}
	textcolor(15);
	gotoxy(cursor->pos,cursor->y);
}


void novaLinha(Cursor **cursor, Linha **inicio, Linha **primeira, Linha **ultima, int **linha1, int **recEsq, int **recDir){
	Linha *nl;
	int espacamento;
	if(primeiraLinhaParagrafo((*cursor)->linha))
		espacamento = **linha1;
	else
		espacamento = **recEsq;

	if((*cursor)->pos == espacamento+1){ //enter no comeco da linha
		nl = criaLinha(criaLetra('\n'));
		if((*cursor)->linha->top == NULL){//caso o cursor esteja na primeira linha
			nl->botton = *inicio;
			(*inicio)->top = nl;
			*inicio = nl;
			*primeira = *inicio;
			
			if(distanciaLinha(*primeira, *ultima) == 21)
				*ultima = (*ultima)->top;
		}
		else{ //caso nao
			nl->botton = (*cursor)->linha;
			(*cursor)->linha->top->botton = nl;
			nl->top = (*cursor)->linha->top;
			(*cursor)->linha->top = nl;
			if( distanciaLinha(*primeira, *ultima) ==21 ){
				if(*ultima == (*cursor)->linha){
					*primeira = (*primeira)->botton;
				}
				else
					*ultima = (*ultima)->top;
			}
				
			
		}
		if(primeiraLinhaParagrafo((*cursor)->linha))
			espacamento = **linha1;
		else
			espacamento = **recEsq;
	}
	else if((*cursor)->pos == (*cursor)->linha->nro+1){//enter no final da linha
		if((*cursor)->linha->botton == NULL){//caso nao tenha mais linhas abaixo
			
			(*cursor)->linha->botton = criaLinha(criaLetra('\n'));
			(*cursor)->linha->botton->top = (*cursor)->linha;
			(*cursor)->linha = (*cursor)->linha->botton;
			(*cursor)->letra = (*cursor)->linha->inicioL;
			
			*ultima = (*ultima)->botton;
			if(distanciaLinha(*primeira, *ultima) == 21){
				*primeira = (*primeira)->botton;
			}
		}
		else{ //caso tenha
			nl = criaLinha(criaLetra('\n'));
			nl->botton = (*cursor)->linha->botton;
			(*cursor)->linha->botton->top = nl;
			nl->top = (*cursor)->linha;
			(*cursor)->linha->botton = nl;
			(*cursor)->linha = nl;
			(*cursor)->letra = nl->inicioL;
			
			if((*cursor)->linha->top == *ultima){
					*primeira = (*primeira)->botton;
					*ultima = (*ultima)->botton;
			}
			else if( distanciaLinha(*primeira, *ultima) == 21)
				*ultima = (*ultima)->top;
			
		}
		if(ultimaLinhaParagrafo((*cursor)->linha->top))
			espacamento = **linha1;
		else
			espacamento = **recEsq;
	}else{
		nl = criaLinha(NULL);
		nl->inicioL = (*cursor)->letra;

		(*cursor)->letra->ant->prox = criaLetra('\n');
		(*cursor)->letra->ant->prox->ant = (*cursor)->letra->ant;
		(*cursor)->letra->ant=NULL;
		
		nl->nro = PosicaoLetra(ultimaLetra(nl->inicioL))-1;
		(*cursor)->linha->nro = PosicaoLetra(ultimaLetra((*cursor)->linha->inicioL))-1;
		
		nl->top = (*cursor)->linha;
		if((*cursor)->linha->botton!=NULL){//ligações necessárias caso haja linhas abaixo
			(*cursor)->linha->botton->top=nl;
			nl->botton = (*cursor)->linha->botton;
		}

		(*cursor)->linha->botton=nl;
		(*cursor)->linha = nl;
		
		
		if((*ultima)==(*cursor)->linha->top && distanciaLinha(*primeira, *ultima) == 20){
			*ultima = (*ultima)->botton;
			*primeira = (*primeira)->botton;
		}
		else if( distanciaLinha(*primeira, *ultima) == 21)
			*ultima = (*ultima)->top;
		else if((*ultima)==(*cursor)->linha->top)
			*ultima = (*ultima)->botton;
		espacamento = **recEsq;
	}
	
	(*cursor)->pos=PosicaoLetra((*cursor)->letra);
	if((*cursor)->y<23)
		(*cursor)->y = (*cursor)->y + 1;
	
}

Letra *ultimaLetra(Letra *L){
	while(L->prox!=NULL)
		L = L->prox;
	return L;
}

void removeLetra(Linha *inicio,Cursor **cursor, int **linha1, int **recEsq, int **recDir, Linha **primeira, Linha **ultima){
	Letra *l, *laux;
	Linha *lin; int espacamento;
	if(primeiraLinhaParagrafo((*cursor)->linha))
		espacamento = **linha1;
	else
		espacamento = **recEsq;
		
	if(**linha1>0 || **recEsq>0 || **recDir!=79)
		removeEspacos(inicio,&*cursor,*linha1,*recEsq);
	l = (*cursor)->letra->ant;
	
	if(l!=NULL && l->letra == 21){
		l = l->ant;
		if(l!=NULL && l->letra == 21){
			if(l->ant==NULL){
				
				(*cursor)->linha->inicioL = (*cursor)->letra;
				(*cursor)->letra->ant = NULL;
			}
			else{
				l->ant->prox = (*cursor)->letra;
				(*cursor)->letra->ant = l->ant;
				
			}
			free(l->prox);
			free(l);
			l=(*cursor)->letra->ant;
		}
		else if(l!=NULL && l->letra != 21){
			(*cursor)->letra = (*cursor)->letra->ant;
		}
		else{
			free((*cursor)->letra->ant);
			(*cursor)->letra->ant = NULL;
			(*cursor)->linha->inicioL = (*cursor)->letra;
		}
		(*cursor)->pos = PosicaoLetra((*cursor)->letra)+1;
	}
	
	if((*cursor)->linha->nro==0 && (*cursor)->linha->top!=NULL){

		if(distanciaLinha(*primeira,*ultima)==20 && (*primeira)->top!=NULL){
			*primeira = (*primeira)->top;
		}
		else
			(*cursor)->y = (*cursor)->y-1;
		if(*ultima == (*cursor)->linha)
			*ultima = (*ultima)->top;
		lin = (*cursor)->linha;
		free(lin->inicioL);
		//posicionar o cursor na linha superior e atualizar pos
		(*cursor)->linha = (*cursor)->linha->top;
		(*cursor)->letra = ultimaLetra((*cursor)->linha->inicioL);//aponta para ultima letra
		(*cursor)->pos = PosicaoLetra((*cursor)->letra);
					
		lin->top->botton = lin->botton;
		if(lin->botton!=NULL)
			lin->botton->top = lin->top;
		free(lin);
	}
	else if((*cursor)->linha->nro>0 && (*cursor)->pos==1 && (*cursor)->linha->top!=NULL){
		if((*cursor)->linha->top->nro>0){
			l = ultimaLetra((*cursor)->linha->top->inicioL);
			l = l->ant;
			laux = l;
			lin = (*cursor)->linha->top;
			if(primeiraLinhaParagrafo(lin))
				espacamento = **linha1;
			else
				espacamento = **recEsq;
			
			while(lin->nro < **recDir-espacamento && (*cursor)->linha->nro>0){
				(*cursor)->linha->inicioL = (*cursor)->linha->inicioL->prox;
				(*cursor)->linha->inicioL->ant = NULL;
				(*cursor)->letra->prox = l->prox;
				l->prox->ant = (*cursor)->letra;
				l->prox = (*cursor)->letra;
				(*cursor)->letra->ant = l;
				(*cursor)->linha->nro = (*cursor)->linha->nro-1;
				lin->nro = lin->nro+1;
				(*cursor)->letra = (*cursor)->linha->inicioL;
				l = l->prox;
			}
			if((*cursor)->linha->nro==0){
				if(*ultima == (*cursor)->linha)
					*ultima = (*ultima)->top;
				if(distanciaLinha(*primeira,*ultima)==19 && (*primeira)->top != NULL){
					*primeira = (*primeira)->top;
				}
				else
					(*cursor)->y = (*cursor)->y-1;
				free((*cursor)->linha->inicioL);
				lin->botton = (*cursor)->linha->botton;
				if(lin->botton!=NULL)
					lin->botton->top = lin;
				free((*cursor)->linha);
				if((*ultima)->botton != NULL)
					*ultima = (*ultima)->botton;
				
				
			}
			(*cursor)->linha = lin;
			(*cursor)->letra = laux->prox;
			(*cursor)->pos = PosicaoLetra((*cursor)->letra);
			
		}else{
			if(*ultima == (*cursor)->linha)
				*ultima = (*ultima)->top;
			free((*cursor)->linha->top->inicioL);
			(*cursor)->linha->top->inicioL = (*cursor)->linha->inicioL;
			(*cursor)->linha->top->botton = (*cursor)->linha->botton;
			if((*cursor)->linha->botton != NULL)
				(*cursor)->linha->botton->top = (*cursor)->linha->top;
			lin = (*cursor)->linha;
			(*cursor)->linha= (*cursor)->linha->top;
			(*cursor)->linha->nro = lin->nro;
			free(lin);
			
			if((*ultima)->botton != NULL)
				*ultima = (*ultima)->botton;
			(*cursor)->y = (*cursor)->y-1;
		}
	}
	else if((*cursor)->pos==2){//quando só há uma letra
		if((*cursor)->letra->letra == 21){
			(*cursor)->letra = (*cursor)->letra->prox;
			if(l->ant!=NULL)
				free(l->ant);
			free(l->prox);
		}
			
		(*cursor)->linha->inicioL = (*cursor)->letra;
		(*cursor)->letra->ant = NULL;
		(*cursor)->linha->nro = (*cursor)->linha->nro-1;
		
		
		free(l);
	}
	else if((*cursor)->pos>2){
		l->ant->prox = l->prox;
		l->prox->ant = l->ant;
		(*cursor)->linha->nro = (*cursor)->linha->nro-1;
		free(l);
		if((*cursor)->letra->letra==21)
			(*cursor)->letra = (*cursor)->letra->prox;
	}
	(*cursor)->pos = PosicaoLetra((*cursor)->letra);
}

void destruirLetras(Letra **letra){
	if(*letra!=NULL){
		destruirLetras(&(*letra)->prox);
		free(*letra);
		*letra = NULL;
	}
}

void destruirEditor(Linha **inicio){
	if(*inicio!=NULL){
		destruirLetras(&(*inicio)->inicioL);
		destruirEditor(&(*inicio)->botton);
		free(*inicio);
		*inicio = NULL;
	}
}

void exibeEstrutura(Linha *inicio){
	int i= 0;
	Letra *aux;
	while(inicio!=NULL){
		printf("\nlinha %d:\n",++i);
		aux = inicio->inicioL;
		while(aux!=NULL){
			if(aux->letra == '\n')
				printf("-");
			else
				printf("%c",aux->letra);
			aux = aux->prox;
		}
		printf("\n");
		inicio=inicio->botton;
	}
}
