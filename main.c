#include "TAD_Editor.h"
#include "TAD_listagen.h"

void movimentaCursor(Linha *inicio, Linha **primeira, Linha **ultima, char dir, Cursor **cursor, char **insert,
int **lin1, int **recEsq, int **recDir, char **negrito){
	int i;
	switch(dir){
		case 72://cima
				if((*cursor)->linha->top!=NULL){ //tem uma linha pra cima
					if((*cursor)->linha == *primeira){
						*primeira = (*primeira)->top;
						*ultima = (*ultima)->top;
						renderizarTela(inicio, *primeira, *ultima, *cursor, *lin1, *recEsq, *recDir,&*negrito);
					}
					int i=1;
					(*cursor)->linha = (*cursor)->linha->top;
					(*cursor)->letra = (*cursor)->linha->inicioL; //aponta para primeira letra
					while(i <= (*cursor)->linha->nro && i < (*cursor)->pos){
						(*cursor)->letra = (*cursor)->letra->prox;
						i++;
					}
					(*cursor)->pos = i;
					if((*cursor)->y >3)
						(*cursor)->y = (*cursor)->y-1;
						
					gotoxy((*cursor)->pos,(*cursor)->y);		
				}
				break;
		case 80://baixo
				if((*cursor)->linha->botton!=NULL){ //tem uma linha para baixo
					if((*cursor)->linha == *ultima){
						*primeira = (*primeira)->botton;
						*ultima = (*ultima)->botton;
						renderizarTela(inicio, *primeira, *ultima, *cursor, *lin1, *recEsq, *recDir, &*negrito);
					}
					i=1;
					(*cursor)->linha = (*cursor)->linha->botton;
					(*cursor)->letra = (*cursor)->linha->inicioL;
					while(i <= (*cursor)->linha->nro && i < (*cursor)->pos){
						(*cursor)->letra = (*cursor)->letra->prox;
						i++;
					}
					(*cursor)->pos = i;
					if((*cursor)->y<23)
					(*cursor)->y = (*cursor)->y+1;
					gotoxy((*cursor)->pos,(*cursor)->y);
				}
				break;
		case 75://esquerda
				if((*cursor)->pos>1){
					(*cursor)->letra = (*cursor)->letra->ant;
					(*cursor)->pos = (*cursor)->pos-1;
				}
				else if((*cursor)->linha->top!=NULL && (*cursor)->y>3){
					int i=1;
					if((*cursor)->linha == *primeira){
						*primeira = (*primeira)->top;
						*ultima = (*ultima)->top;
						renderizarTela(inicio, *primeira, *ultima, *cursor, *lin1, *recEsq, *recDir,&*negrito);
					}
					(*cursor)->linha = (*cursor)->linha->top;
					(*cursor)->letra = ultimaLetra((*cursor)->linha->inicioL);
					(*cursor)->pos = PosicaoLetra((*cursor)->letra);
					(*cursor)->y = (*cursor)->y-1;
				}
				gotoxy((*cursor)->pos,(*cursor)->y);
				break;
		case 77://direita
				if((*cursor)->letra->prox!=NULL){
					(*cursor)->letra = (*cursor)->letra->prox;
					(*cursor)->pos = (*cursor)->pos+1;
				}
				else if((*cursor)->linha->botton!=NULL){
					if((*cursor)->linha == *ultima){
						*primeira = (*primeira)->botton;
						*ultima = (*ultima)->botton;
						renderizarTela(inicio, *primeira, *ultima, *cursor, *lin1, *recEsq, *recDir, &*negrito);
					}
					(*cursor)->linha= (*cursor)->linha->botton;
					(*cursor)->letra = (*cursor)->linha->inicioL;
					(*cursor)->pos = 1;
					(*cursor)->y = (*cursor)->y+1;
					
				}
				gotoxy((*cursor)->pos,(*cursor)->y);
				break;
		case 71: //home
				(*cursor)->pos = 1;
				(*cursor)->letra = (*cursor)->linha->inicioL;
				gotoxy((*cursor)->pos,(*cursor)->y);
				break;
		case 79: //end
				(*cursor)->letra = ultimaLetra((*cursor)->linha->inicioL);
				(*cursor)->pos = PosicaoLetra((*cursor)->letra);
				gotoxy((*cursor)->pos,(*cursor)->y);
				break;
		case 73://page up
				if((*primeira)->top != NULL){
					i=0;
					while((*primeira)->top != NULL && i<21){
						i++;
						(*primeira) = (*primeira)->top;
						(*cursor)->linha = (*cursor)->linha->top;
					}
					(*cursor)->y = distanciaLinha(*primeira,(*cursor)->linha) + 3;
					i=1;
					(*cursor)->letra = (*cursor)->linha->inicioL;//aponta para primeira letra
					while(i <= (*cursor)->linha->nro && i < (*cursor)->pos){
						(*cursor)->letra = (*cursor)->letra->prox;
						i++;
					}
					(*cursor)->pos = i;
					
					while(distanciaLinha(*primeira,*ultima)>=21){
						(*ultima) = (*ultima)->top;
					}
					renderizarTela(inicio, *primeira, *ultima, *cursor, *lin1, *recEsq, *recDir, &*negrito);
				}
				
				break;
		case 81://page down
				i=0;
				while((*ultima)->botton != NULL && i<21){
					i++;
					(*ultima) = (*ultima)->botton;
					(*cursor)->linha = (*cursor)->linha->botton;
				}
				i=1;
				(*cursor)->letra = (*cursor)->linha->inicioL;//aponta para primeira letra
				while(i <= (*cursor)->linha->nro && i < (*cursor)->pos){
					(*cursor)->letra = (*cursor)->letra->prox;
					i++;
				}
				(*cursor)->pos = i;
				while(distanciaLinha(*primeira,*ultima)>=21){
					(*primeira) = (*primeira)->botton;
				}
				(*cursor)->y = distanciaLinha(*primeira,(*cursor)->linha) + 3;
				renderizarTela(inicio, *primeira, *ultima, *cursor,*lin1, *recEsq, *recDir,&*negrito);
				break;
		case 83: //delete
				if((*cursor)->letra->prox!=NULL){
					if((*cursor)->pos==1){
						(*cursor)->linha->inicioL = (*cursor)->letra->prox;
						(*cursor)->letra->prox->ant = NULL;
						free((*cursor)->letra);
						(*cursor)->letra = (*cursor)->linha->inicioL;
						(*cursor)->linha->nro = (*cursor)->linha->nro-1;
						
					}
					else{
						Letra *l = (*cursor)->letra;
						l->ant->prox = l->prox;
						l->prox->ant = l->ant;
						(*cursor)->letra = l->prox;
						free(l);
						(*cursor)->linha->nro = (*cursor)->linha->nro-1;
					}
					gotoxy((*cursor)->pos,(*cursor)->y);
				}
				renderizarTela(inicio, *primeira, *ultima, *cursor, *lin1, *recEsq, *recDir,&*negrito);
				break;
		case 82: //insert
			**insert = !(**insert);
			break;
	}
}

void manipulaF(char f, Cursor **cursor, Linha **inicio, Linha **primeira, Linha **ultima,
int **lin1, int **recEsq, int **recDir, Palavras **arvore){
	Letra *l;
	Linha *lin;
	FILE *arq;
	char c, nomeArq[20], aux;
	int correto;
	switch(f){
		case 61://F3
			textcolor(0);
			pintaLinha(1,25,7);
			tela("SALVAR ARQUIVO");
			
			gotoxy(15,12);
			printf("Nome do arquivo(com extensao): ");
			
			gets(nomeArq);
			arq = fopen(nomeArq,"w");
			lin = *inicio;
			while(lin!=NULL){
				l=lin->inicioL;
				while(l!=NULL){
					fprintf(arq,"%c",l->letra);
					l = l->prox;	
				}
				lin = lin->botton;
			}
			fclose(arq);
			break;
		case 60://F2 abrir
			textcolor(0);
			pintaLinha(1,25,7);
			tela("ABRIR ARQUIVO");
			
			gotoxy(15,12);
			printf("Nome do arquivo(com extensao): ");

			gets(nomeArq);
			if(nomeArq[0]!='\0'){
				arq = fopen(nomeArq,"r");
				destruirLG(&*arvore);
				destruirEditor(&*inicio);
				InicializaEditor(&*inicio, &*primeira, &*ultima, &*cursor, *lin1);
				if(arq!=NULL){
					while(!feof(arq)){
						c = getc(arq);
						if(c=='\n'){
							fflush(stdin);
							aux = getc(arq);
							if(aux!=-1){//se nao for ultima linha
								novaLinha(&*cursor, &*inicio, &*primeira, &*ultima, &*lin1, &*recEsq, &*recDir);
								insereLetra(aux,&*cursor, &*lin1, &*recEsq, &*recDir,*inicio);
							}
						}
						else
							insereLetra(c,&*cursor, &*lin1, &*recEsq, &*recDir,*inicio);
					}
					fclose(arq);
					
				}else	
					aviso("Arquivo nao encontrado");
			}
			
			break;
		case 68:
				insereLetra(21,&*cursor, &*lin1, &*recEsq, &*recDir, *inicio);
				break;
		case 63: 
				correto = 0;
				do{
					textcolor(0);
					pintaLinha(1,25,7);
					tela("CONFIGURACOES DE PARAGRAFO");
					gotoxy(15,11);
					printf("Primeira linha: ");
					scanf("%d",*lin1);  
					gotoxy(15,12);
					printf("Recuo Esquerdo: ");
					scanf("%d",*recEsq); 
					gotoxy(15,13);
					printf("Recuo Direito: ");
					scanf("%d",*recDir); 
					if(**lin1>0 && **lin1<79 && **recEsq>0 && **recEsq<79 && **recDir>**lin1 && **recDir>**recEsq && **recDir<80)
						correto = 1;
					else
						aviso("Configuracao de paragrafo invalida");
				}while(!correto);
				
				break;
	}
}

char tratamento(char tecla, Cursor **cursor, Linha **inicio, Linha **primeira, Linha **ultima, 
Palavras **arvore, char **insert, char palavra[], char plvSugerida[], int **linha1, int **recEsq, int **recDir, char **negrito){
	int len, i, TL, espaco;
	Letra *laux;
	char c;
	switch(tecla){
		case 13: 
			len = strlen(plvSugerida);
			if(len>0){
				
				if(primeiraLinhaParagrafo((*cursor)->linha))
					espaco = **recDir - **linha1;
				else
					espaco = **recDir - **recEsq;
					
				if((*cursor)->linha->nro+len<=espaco){
					TL = strlen(palavra);
					for(i=0; i<len; i++){
						insereLetra(plvSugerida[i],&*cursor, &*linha1, &*recEsq, &*recDir,*inicio);
						palavra[TL++]=plvSugerida[i];
					}
					palavra[TL]='\0';						
				}
				else{
					len = strlen(palavra);
					while(len>0){
						removeLetra(*inicio,&*cursor,&*linha1,&*recEsq,&*recDir,&*primeira,&*ultima);
						len--;
					}

					if(ultimaLinhaParagrafo((*cursor)->linha))
						espaco = **recDir - **linha1;
					else
						espaco = **recDir - **recEsq;
					
					if(strlen(palavra)+strlen(plvSugerida)<=espaco){//se cabe na nova linha
						novaLinha(&*cursor, &*inicio, &*primeira, &*ultima,&*linha1,&*recEsq,&*recDir);
						len = strlen(palavra);
						for(i=0; i<len; i++){
							insereLetra(palavra[i],&*cursor, &*linha1, &*recEsq, &*recDir,*inicio);
						}
						len = strlen(plvSugerida);
						TL = strlen(palavra);
						for(i=0; i<len; i++){
							insereLetra(plvSugerida[i],&*cursor,&*linha1,&*recEsq,&*recDir,*inicio);
							palavra[TL++]=plvSugerida[i];
						}
						laux = ultimaLetra((*cursor)->letra);
						laux = laux->ant; 
						laux = ultimaLetra((*cursor)->linha->top->inicioL);
						laux = laux->ant;
						palavra[TL]='\0';
					}else{
						len = strlen(palavra);
						for(i=0; i<len; i++)
							insereLetra(palavra[i],&*cursor, &*linha1, &*recEsq, &*recDir,*inicio);
						
						aviso("Sugestao excede capacidade da formatacao");
						if(strlen(palavra)>1)
							inserePlv(&*arvore,palavra);
							palavra[0]='\0';
						}	
				}
				plvSugerida[0] = '\0';
			}
			else{
				if(strlen(palavra)>1)
					inserePlv(&*arvore,palavra);
				palavra[0]='\0';
				novaLinha(&*cursor, &*inicio, &*primeira, &*ultima,&*linha1,&*recEsq,&*recDir);
			}
			return 1;
		case -32 :
			movimentaCursor(*inicio, &*primeira, &*ultima, getch(),&*cursor,&*insert, &*linha1, &*recEsq, &*recDir, &*negrito);
			return 1;
		case 8:  //back space
			removeLetra(*inicio,&*cursor,&*linha1,&*recEsq,&*recDir,&*primeira,&*ultima);
			len = strlen(palavra);
			palavra[len-1]='\0';
			return 1;
		case 0: 
			c = getch();
			if(c==62)
				return 0;
			manipulaF(c,&*cursor, &*inicio, &*primeira, &*ultima, &*linha1, &*recEsq, &*recDir, &*arvore);
			return 1;
		default: 
			if(tecla!=' ' && tecla!=',' && tecla!='!' && tecla!='?' && tecla!='.'){
				int espaco;
				if(primeiraLinhaParagrafo((*cursor)->linha))
					espaco = **recDir - **linha1;
				else
					espaco = **recDir - **recEsq;
					
				len = strlen(palavra);
				if((*cursor)->linha->nro+1<=espaco){
					palavra[len]=tecla;
					palavra[len+1]='\0';
				}
			}
			else{
				if(strlen(palavra)>1)
					inserePlv(&*arvore,palavra);
				palavra[0] = '\0';
			}
				
			if( **insert || (*cursor)->letra->prox==NULL)
				insereLetra(tecla,&*cursor,&*linha1,&*recEsq,&*recDir,*inicio);
			else
				substituiLetra(tecla,&*cursor);
			return 1;
	}
}

int main(void){

	Linha *inicio, *primeira, *ultima;
	Palavras *arvore = NULL;
	Cursor *cursor = (Cursor*)malloc(sizeof(Cursor));
	char *insert = (char*)malloc(sizeof(char)), *negrito = (char*)malloc(sizeof(char));
	char tecla, executando=1, palavra[79], plvSugerida[79];
	int *lin1, *recEsq, *recDir; 
	
	lin1 = (int*)malloc(sizeof(int));
	recEsq = (int*)malloc(sizeof(int));
	recDir = (int*)malloc(sizeof(int));
	*lin1 = *recEsq = 0; *recDir = 79;
	*insert	= 1; *negrito = 0;
	palavra[0] = '\0';
	plvSugerida[0] = '\0';

	InicializaEditor(&inicio,&primeira, &ultima, &cursor, lin1);

	atualizaRodape(cursor,*insert,inicio,*negrito);
	cabecalho();
	pintaLinha(3,23,7);

	while(executando){
		gotoxy(cursor->pos,cursor->y);
		fflush(stdin);
		tecla = getch();
		executando = tratamento(tecla,&cursor,&inicio, &primeira, &ultima, &arvore,&insert,palavra, plvSugerida, &lin1, &recEsq, &recDir,&negrito);
		if(tecla != -32){
			if(*lin1>0 || *recEsq>0 || *recDir!=79)
				formataTexto(&inicio,&primeira,&ultima,&cursor,lin1,recEsq,recDir);
	
			renderizarTela(inicio, primeira, ultima, cursor, lin1, recEsq, recDir, &negrito);
		}
		strcpy(plvSugerida,palavra);
		atualizaRodape(cursor,*insert,inicio,*negrito);
		sugerir(&arvore,plvSugerida);
		
		if(plvSugerida[0]!='\0'){
			gotoxy(50,25);
			textbackground(9);
			textcolor(8);
			printf("%s",palavra);
			textcolor(7);
			printf("%s",plvSugerida);
			textbackground(0);
		}
	}
	system("cls");
	return 0;
}
