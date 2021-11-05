# Editor de textos com listas encadeadas e lista generalizada de palavras

Funcionalidades:

Além das setas de navegação, as seguintes teclas funcionam:

• Home – coloca o cursor no início da linha corrente (dada pelo ponteiro linha);
• End – coloca o cursor no final da linha corrente (dada pelo ponteiro linha);
• Page Up – rolagem de uma tela para cima;
• Page Down – rolagem de uma tela para baixo;
• Back Space – apaga um caracter (à esquerda do cursor);
• Del – apaga um caracter (sob o cursor);
• Insert - 
  ligado: Insere novos caracteres; 
  desligado: escreve em cima dos caracteres existentes.
• F10 – Negrito: o caracter § (21 da tabela ASCII) indica início ou fim de uma palavra ou trecho em Negrito.
• F2 Abrir: função para ler de um arquivo texto e carregar para a estrutura do editor (cada linha do arquivo não deverá ser maior que 79 caracteres);
• F3 Salvar: função para percorrer toda a estrutura do editor e gravar em um arquivo texto;
• F5 Exibir: função para exibir o texto na tela de maneira formatada de acordo com a configuração prévia: Primeira Linha, Recuo Esquerdo e Recuo Direito.
• É possível pressionar enter no meio de uma linha e assim criar outra abaixo cujo conteúdo são as letras a partir do cursor.
• O usuário pode formatar o texto e continuar digitando com ele formatado, de modo que se a palavra digitada couber na linha superior ela será puxada a não ser que essa linha seja final de parágrafo.
• Quando os parâmetros de formatação estão definidos não é possível adicionar vários espaços consecutivos, pois o espaçamento entre as palavras será ajustado conforme a formatação.
• Ao pressionar o Backspace e caso o cursor esteja posicionado na primeira letra da linha corrente seu conteúdo será jogado para a linha superior (o quanto couber). E assim se a linha ficar vazia ela será removida do editor.

Ao digitar as iniciais de uma palavra, o algoritmo indica uma palavra já  conhecida, armazenada na estrutura. A cada nova palavra digitada com mais de dois caracteres, essa deverá ser inserida na lista generalizada, para o algoritmo “aprender”
