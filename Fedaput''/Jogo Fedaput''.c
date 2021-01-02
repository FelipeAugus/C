/// FELIPE AUGUSTO SOUZA SILVA; NOTURNO;
/// Trabalho 1 - jogo fedaputa;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
///STRUCTS
typedef struct Carta{
    int num;  // 1 a 10;
    int naip; // 1 - paus / 2 – copas / 3 – espadas / 4 - ouro; de acordo com a força no game, 1 mais forte 4 mais fraco;
    int forca; //Usado para lutas
}carta;
typedef struct Jogador{
    char nome[20];
    int vida;
    carta mao[5]; //vetor para armazenar as cartas que ele está segurando;
    int faz; //quantidade de mesas que ele diz fazer, atualizado em cada rodada;
}jog;
typedef struct Baralho{ //Lista/Pilha (depende de como o ctrl é usado, e como são feitas as remoções/adições na struct) estática de cartas
    carta c[40]; //A, 2, 3, 4, 5, 6, 7, Q, J, K; * 4 naipe cada;
    int ctrl; //controlar a qtde ou topo do baralho
}baralho;


///FUNÇÕES
baralho* CriaBaralho();
carta CriaCarta(int num, int naip, int forca);
baralho* EmbaralhaCarta(baralho *bL);

void GirarMesa(jog J[], int qt, int vezes, int direcao);

void ImprimeCartas(carta M[], int cont);

int IniciaJogadores(jog J[], int qt);

void DistribuiCartas(jog J[], int qt, int *rodada, int *op, int *RA);
void Faz(jog J[], int QtJog,int rodada);
int Luta(jog J[], int *qtd, int *rodada);
int TestaVida(jog J[], int *qtd, int *rodada);

void ImprimeVetCartaNoArq(carta M[], int cont);
void ImprimeCartaNoArq(carta M);
void limpa_linha();

////
int main(){

    //Para imprimir a data;
    struct tm *data;
    time_t dt;
    time(&dt);
    data = localtime(&dt);
    //
    FILE *arq; //ponteiro para algum arquivo

    int continua = 0; //variavel de controle do loop;
    while(continua == 0 || continua == 1){
        continua = 0;

        arq = fopen("log.txt", "w"); //cria o arquivo, se existe sobrescreve; "w"
        fputs("\t\t\t\tFEDAPUTA\t", arq); //escreve no arquivo, no final;
        fprintf(arq, "\t %d/%d/%d\n\n", data->tm_mday, data->tm_mon+1, (data->tm_year+1900)%2000);
        fclose(arq); //fecha o arquivo

        ///Inicialização dos jogadores
        //Váriaveis para controle do game
        int rodada = 0;//Quantidade de cartas para mão
        int op = 1; //Define o sentido se rodada esta aumentando ou diminuindo. 1 aumenta, 0 diminui. Alteraçoes são feitas na Função 'DistribuiCartas'
        int QtJog; //Qtde de jogadores

        do{ //leitura de uma quantidade valida de jogadores
            printf("______________FEDAPUT*_______________\n\n\n");
            printf("Quantos serao os jogadores [2 a 8] ?");
            scanf("%d", &QtJog);
            limpa_linha(); //limpa buffer;
            if(QtJog<2 || QtJog>8){
                printf("Quantidade invalida! pressione ENTER para continuar;");
                getchar();
                system("cls");
            }
        }while(QtJog<2 || QtJog>8);
        //

        jog J[QtJog]; //vetor de jogadores, de 2 a 8, de acordo com o que o usuario entrou;
        if(IniciaJogadores(J, QtJog)) printf("%d jogadores inicializados. \n", QtJog);

        ///Jogo
        int ArqRodada = 1; //Controle do numero de rodadas para dar fprintf no Arquivo;

        while(QtJog>1){ //enquanto tiver mais de um jogador continua o jogo
            DistribuiCartas(J, QtJog, &rodada, &op, &ArqRodada);
            printf("Cartas distribuidas! Pressione ENTER PARA CONTINUAR");
            getchar();
            system("cls");
            Faz(J, QtJog, rodada); //tava no main essa função, por ter muito printf e um scanf, etc., mas achei muito grande e transformei em função.
            if(Luta(J, &QtJog, &rodada)){ //retorna 1 se houver mais de 1 jogador, para continuar o jogo.
                    GirarMesa(J, QtJog, 1, 1); //Se a rodada X começou do P2, a rodada X+1 começa do P3
                    printf("Proxima rodada.");
            }
        }//se tiver apenas 1 jogador, ou nenhum, sai do while.

        ///Declaração do vencedor
        arq = fopen("log.txt", "a"); //escreve no arquivo, no final; "a"
        fprintf(arq, "\n\n\tO vencedor da partida eh **%s** com %d vidas restantes.", J[0].nome, J[0].vida);
        fclose(arq);

        printf("O vencedor da partida e %s com %d vidas restantes. Parabens!", J[0].nome, J[0].vida);

        getchar ();
        system("cls");
        ///Fim

        ///Jogar novamente?
        while(continua!=1 && continua!=2){
            printf("Desejam jogar outra partida?\n[1]SIM\t[2]NAO\n");
            scanf("%d", &continua);
            limpa_linha();
            if(continua!=1 && continua!=2) printf("Opcao invalida!");
            system("cls");
        }
        //se sim, perde o arquivo da anterior, pois ele vai sobrescrever o que estava escrito antes; senão encerra o executavel.

        ///---
    }


    return 57;
}
////

//Insere as 40 cartas, em um Baralho, uma Lista estática de cartas;
baralho* CriaBaralho(){
    //como estamos inserindo apenas no topo da lista, é semelhante a insere pilha;
    baralho* b = (baralho*)malloc(sizeof(baralho));
    b->ctrl = 0;
    b->c[b->ctrl] = CriaCarta(4, 1, b->ctrl+1); //zap c0
    b->ctrl++;
    b->c[b->ctrl] = CriaCarta(7, 2, b->ctrl+1); //copas c1
    b->ctrl++;
    b->c[b->ctrl] = CriaCarta(1, 3, b->ctrl+1); //espadilha c2
    b->ctrl++;
    b->c[b->ctrl] = CriaCarta(7, 4, b->ctrl+1); //7ouro c3
    b->ctrl++;
    for(int i = 3; i>0; i--){ //inserir as cartas 3, 2 e 1 = A;
        for(int np = 1; np<=4; np++){// primeiro paus, depois coração, depois espada e depois ouro; na sequencia da força;
            if(i!=1 || np!=3){ //já inseriu o epada então não reenserir
                b->c[b->ctrl] = CriaCarta(i, np, b->ctrl+1);
                b->ctrl++;
            }
        }
    }
    for(int i = 10; i>=4; i--){ // inserir as cartas 10 = K, 9 = J, 8 = Q, 7, 6, 5 e 4;
        for(int np = 1; np<=4; np++){
            if((i!=7 || np!=2) && (i!=7 || np!=4) && (i!=4 || np!=1)){ //já inseriu as outras manilhas então não reenserir
                b->c[b->ctrl] = CriaCarta(i, np, b->ctrl+1);
                b->ctrl++;
            }
        }
    }
    b = EmbaralhaCarta(b); //transforma em uma pilha de forma embaralhada;
    return b;
}
//Recebe o valores da struct Carta e passa de volta para ela;
carta CriaCarta(int num, int naip, int forca){
    carta c;
    c.num = num;
    c.naip = naip;
    c.forca = forca;
    return c;
}
//Recebe uma lista de cartas, pode ser acessado em qualquer pos, e passa os membros dessa lista, de forma randomica, para uma pilha de carta, que só vai ser acessada no topo
baralho* EmbaralhaCarta(baralho *bL){
    //bL = lista; bP = pilha;
    baralho* bP = (baralho*)malloc(sizeof(baralho)); //aloca a pilha de cartas
    bP->ctrl = 0; //define o topo;

    for(bP->ctrl; bP->ctrl<40; bP->ctrl++){ //topo de 0 a 39; 40cartas;
        srand((unsigned)time(NULL)); //alimentar o rand
        int pos = rand()%bL->ctrl; //gerar uma pos aleatória para remoção;
        carta aux = bL->c[pos]; //aux recebe a carta que sera removida da lst e adicionada na pilha;

        //adiciona no topo da pilha;
        bP->c[bP->ctrl] = aux;

        //remove da lista
        while(pos != bL->ctrl){
            bL->c[pos] = bL->c[pos+1];
            pos++;
        }
        bL->ctrl--; //reduz a quantidade de cartas da lista;

    }// o topo se incrementa automaticamente com o 'for';

    free(bL);//todos elementos da lista foram embaralhados para pilha e agora ela esta vazia e pode ser liberada;

    return bP; //retorna a pilha;
}

//Faz as rotações da mesa, no fedaputa tem várias rotações na mesa...
void GirarMesa(jog J[], int qt, int vezes, int direcao){ //direcao 1 roda horario; -1 volta o giro
    for(int i = 0; i<vezes;i++){
        if(direcao == 1){
            jog aux = J[0];
            int i;
            for(i = 1; i<qt; i++){
                J[i-1] = J[i]; // 0 = 1; 1 = 2; 2 = 3; 3 = 4; 4 = 5; 5 = 6; 6 = 7;
            }
            J[i-1] = aux; //7 = 0;
        }else{
            int i = qt-1;
            jog aux = J[i];
            for( i ; i>0; i--){
                J[i] = J[i-1]; // 7 = 6; 6 = 5; 5 = 4; 4 = 3; 3 = 2; 2 = 1; 1 = 0;
            }
            J[i] = aux; //0 = 7
        }
    }
    ///     --> DistribuiCartas --> Faz --> Luta --> VoltaPraGirarMesa
}

//autoexplicativa k
void ImprimeCartas(carta M[], int cont){

    for(int i = 0; i<cont; i++){
        printf("CARTA %d \t", i+1);
    }
    printf("\n");
    for(int i = 0; i<cont; i++){
        int aux = M[i].num;
        if(aux == 10 || aux == 9 || aux == 8 || aux == 1){
            if(aux == 1) aux = 65;
            else if(aux == 10) aux = 75;
            else if(aux == 9) aux = 74;
            else if (aux == 8) aux = 81;
            if(M[i].naip == 1){
                printf("%c de PAUS\t",aux);
            }else if(M[i].naip == 2){
                printf("%c de COPAS\t",aux);
            }else if(M[i].naip == 3){
                printf("%c de ESPADA\t",aux);
            }else{
                printf("%c de OURO\t",aux);
            }
        }else{
            if(M[i].naip == 1){
                printf("%d de PAUS\t",aux);
            }else if(M[i].naip == 2){
                printf("%d de COPAS\t",aux);
            }else if(M[i].naip == 3){
                printf("%d de ESPADA\t",aux);
            }else{
                printf("%d de OURO\t",aux);
            }
        }
    }
    printf("\n");
}

//Inicia a struct jogadores recebendo o nome, dando 5 vida aos jogadores, e colocando 0 na quantidade que dizem fazer;
int IniciaJogadores(jog J[], int qt){
    FILE *arq = fopen("log.txt", "a"); //adiciona no fim do arquivo
    fputs("\t\t\tJOGADORES:\n", arq); //escreve no arquivo
    if(qt>=2 && qt<=8){
        for(int i = 0; i<qt; i++){
            J[i].vida = 5;
            J[i].faz = 0;
            printf("Qual nome do Player %d? (digite o nome e pressione a tecla ENTER)\n", i+1);
            gets(J[i].nome);
            fprintf(arq,"- %s \n", J[i].nome); //escreve no arquivo
        }
        fclose(arq); //fecha o arquivo
        return 1;
    }else return 0;
}

//Chama a função DaCarta e passa a quantidade de cartas de a cordo com a rodada, além disso controla as rodadas;
void DistribuiCartas(jog J[], int qt, int *rodada, int *op, int *RA){

    FILE *arq;
    arq = fopen("log.txt", "a");
    fprintf(arq, "\n\n\t\t\t\tRODADA %d\n", *RA);
    fclose(arq);
    (*RA)+=1; //incrementa as rodadas para contar no Arquivo;

    if(*op == 2 || *rodada ==-1){ //quer dizer que deu empate na luta, então vai ser só uma carta a partir de agora...
        *op = 2;
        *rodada = 1;
    }else{
        //atualizar os cotroladores do game;
        if(*rodada == 1 || *rodada == 0) *op = 1;
        //se rodada 1, é o minimo, entao passa op para 1 e as rodadas vao aumentar; e para iniciar o jogo a variavel rodada é iniciada como 0
        else if(*rodada == 5) *op = 0; //se rodada 5, é o maximo, entao passa op para 0 e as rodadas vao diminuir

        if(*op == 1) *rodada = (*rodada)+1;
        else if(*op == 0) *rodada = (*rodada)-1;
    }
    //cria o baralho
    baralho *B = CriaBaralho();
    for(int i = 0; i<qt; i++){
        arq = fopen("log.txt", "a");

        for(int j = 0; j<*rodada; j++){
            //remove e passa topo pilha;
            //no ultimo loop do for na função embaralha, ele dá um topo++ só que nao adiciona nada, entao tem que reduzir antes para nao dar erro;
            B->ctrl--; //reduz topo, para fazer a remoção lógica;
            J[i].mao[j] = B->c[B->ctrl]; //topo da pilha para mão do jogador;

        }

        fprintf(arq, "\n\tMao de %s :\n", J[i].nome);
        fclose(arq);
        ImprimeVetCartaNoArq(J[i].mao, *rodada);
    }

    free(B); //pode deletar B, já que não tornara a acessar esse baralho;
    ///     --> Faz --> Luta --> GirarMesa --> VoltaPraDistribuiCartas
}
//Os jogadores declaram aqui quantas mesas irão fazer;
void Faz(jog J[], int QtJog,int rodada){
    FILE *arq = fopen("log.txt", "a");
    fprintf(arq, "\n\t\t\tFazem:\n");
    fclose(arq);

    int fazendo = 0; //variavel para controlar quantas mesas os jogadores dizem fazer;
    for(int i = 0; i<QtJog; i++){
        printf("PRESSIONE ENTER PARA MOSTRAR A MAO DE %s:", J[i].nome);
        getchar();
        system("cls");

        int ok = 0;
        while(ok == 0){
            ImprimeCartas(J[i].mao, rodada);

            if(i!=0) printf("\nANTERIORES FAZEM:  %d\n\n", fazendo);
            printf("\nQuantas mesas vc faz %s? ", J[i].nome);
            int f;
            scanf("%d", &f);
            limpa_linha();
            system("cls");
            if(f<0 || f>rodada){ //não tem como fazer rodadas negativas, ou acima da quant cartas em maos;
                printf("ERRO: Quantidade de rodadas invalidas.\n");
            }else{
                if(i == QtJog-1){ //se for o ultimo jogador a dizer quantas faz, tem que verificar as quantidades anteriores
                    if(fazendo+f != rodada){
                    //se o tanto que os anteriores tiverem fazendo+o que ele diz que vai fazer for = o numero de cartas em maos entao nao pode fazer; senao ok
                        J[i].faz = f;

                        arq = fopen("log.txt", "a");
                        fprintf(arq, "> %s disse que faz %d mesas\n", J[i].nome, J[i].faz);
                        fclose(arq);

                        ok = 1;
                    }else{
                        printf("Voce deve fazer mais ou menos rodada.\n");
                        printf("Pois o numero de rodadas de todos jogadores somados deve ser diferente da quantidade de carta em maos\n");
                    }
                }else{ //se nao for o ultimo jogador, apenas soma a quantidade que esse jogador faz a variavel aux, 'fazendo'.
                    J[i].faz = f;

                    arq = fopen("log.txt", "a");
                    fprintf(arq, "> %s disse que faz %d mesas\n", J[i].nome, J[i].faz);
                    fclose(arq);

                    fazendo += f;
                    ok =1;
                }
            }
        }

        arq = fopen("log.txt", "a");
        fprintf(arq, "\n");
        fclose(arq);
    }
    ///     --> Luta --> GirarMesa --> DistribuiCartas --> VoltaPraFaz
}
//Aqui é uma das principais funções, onde veremos quem venceu cada mesa e se está perdendo vida ou sendo eliminado, é o campo de batalha do game;
int Luta(jog J[], int *qtd, int *rodada){
    FILE *arq;
    int qt = *qtd;
    int rod = *rodada;
    carta mesa[qt];
    int GirosMesa = 0;
    //quando alguem ganha a rodada, a proxima começa dele, mas o jogo depois continua o gro normal; entao tem que girar pro prox e depois voltar, pra isso essa varialvel de control;

    for(rod; rod>0; rod--){
        arq = fopen("log.txt", "a");
        fprintf(arq, "\t\t\tJogadas:\n");
        fclose(arq);

        int jogada; //variavel para ler qual carta o usuario quer jogar
        int i; //repetição
        int Mtopo = 0; //em qual posição do vet mesa vai colocar a carta
        for( i = 0; i<qt; i++){
            int ok = 0;

            printf("PRESSIONE ENTER PARA PASSAR PARA VEZ DE %s:", J[i].nome);
            getchar();
            system("cls");

            while(ok==0){

                if(i!=0){
                        printf("\nMESA: \n");
                        ImprimeCartas(mesa, i);
                }

                printf("\n\nMAO DE %s: \n", J[i].nome);
                ImprimeCartas(J[i].mao, rod);
                printf("\nQual carta deseja jogar %s? ", J[i].nome);
                scanf("%d", &jogada);
                limpa_linha();

                if(jogada<=0 || jogada>rod) printf("ERRO: Valor inválido"); //se tiver fora do numero de cartas imprime inválido
                else ok = 1; //senao da ok e sai do while
                system("cls");

            }

            jogada--; //para passar para valores de vetor

            carta aux = J[i].mao[jogada]; // "backup" da remove lista
            while(jogada != rod){  //função remove lista estatica
                J[i].mao[jogada] = J[i].mao[jogada+1];  //a partir da posição que retiramos da mão do jogador vamos "descendo" os elementos;
                jogada++;
            }//

            arq = fopen("log.txt", "a");
            fprintf(arq, "> %s jogou na mesa um ", J[i].nome);
            fclose(arq);
            ImprimeCartaNoArq(aux);

            mesa[Mtopo] = aux; //passa a carta retirada da mão, que está no backup da remove lista, para mesa;
            Mtopo++; //incrementa topo da mesa;

        } //fim da repetição, todos jogadores já jogaram suas cartas nesse turno;

        ImprimeCartas(mesa, i); //mostra a mesa
        int forte = 0;
        for(int f = 1; f<qt; f++){
            if(mesa[forte].forca > mesa[f].forca) forte = f; //quer dizer que a carta lançada pelo Player[0] é mais fraca que a do Player[F]
        }
        printf("\n\n%s ganhou essa mesa\n\n", J[forte].nome); //diz quem ganhou essa mesa

        arq = fopen("log.txt", "a");
        fprintf(arq, "%\n\t\t\tMesa\n");
        fclose(arq);
        ImprimeVetCartaNoArq(mesa, i);
        arq = fopen("log.txt", "a");
        fprintf(arq, ">> %s ganhou essa mesa\n\n", J[forte].nome);
        fclose(arq);

        J[forte].faz--; //como o jogador já fez uma rodada, diminui um da quantidade que diz que faria;
        GirarMesa(J, qt, forte, 1); //a rodada seguinte começa na posição de quem ganhou a atual então tem que girar a mesa até essa pessoa
        GirosMesa += forte; //mas a prox rodada, começa de acordo com o inicio dessa, entao incrementar o controlador para voltar depois.
    }//fim da rodada

    GirosMesa %= qt; //se tiver dado uma volta completa, volta no mesmo pt, entao so preciso das voltas incompletas
    GirarMesa(J, qt, GirosMesa, -1); //volta a mesa para o inicio dessa rodada;

    TestaVida(J, qtd, rodada);

    if(*qtd > 1) return 1;
    else return 0;
    ///     --> GirarMesa --> DistribuiCartas--> Faz --> VoltaPraLuta
}
//faz a remoção de jogadores com zero vidas ou remove as vidas
int TestaVida(jog J[], int *qtd, int *rodada){
    int qt = *qtd;
    int vidas = 0;
    jog *BackupJ = (jog*)malloc(qt*sizeof(jog));

    int BackupQt = *qtd;
    FILE *arq;

    for(int j = 0; j<qt; j++){
        vidas += J[j].vida; //conta qual a quantidade total de vidas de todos jogadores.
    }
    if(vidas == qt){ //se vidas for = a quantia de jogadores, quer dizer que cada um tem apenas uma, então se todos morrem pode haver empate;
        for(int i = 0; i<qt; i++){
            BackupJ[i] = J[i];
        }
    }

    ///remoção
    for(int j = 0; j<qt; j++){
        if(J[j].faz != 0){ //se o jogador fez menos ou mais mesas do que disse que faria;
            J[j].vida--;   //ele perde vida;
            if(J[j].vida!=0){ //se perdeu vida, mas tiver mais que zero:
                printf("> %s voce perdeu uma vida! Voce esta com %d vidas\n", J[j].nome, J[j].vida);

                arq = fopen("log.txt", "a");
                fprintf(arq, "\n> %s perdeu uma vida, e agora esta com %d vidas\n",J[j].nome, J[j].vida);
                fclose(arq);
            }
        }
        if(J[j].vida == 0){ //se as vidas tiverem chegado a zero:
            printf(">> %s suas vidas acabaram, voce foi eliminado!\n", J[j].nome);

            arq = fopen("log.txt", "a");
            fprintf(arq, "\n>>> %s perdeu uma vida e ficou sem vidas!\n", J[j].nome);
            fclose(arq);

            int del = j; //del seria comumente "pos" na remoção;
            while(del != qt){  //função remove lista estatica
                J[del] = J[del+1];
                del++;
            }//

            (*qtd)-=1; //como tem -1 jogador, diminui o controlador de jogadores.
            qt = *qtd;
            j--;
            //ao remover, os elementos "descem", então temos q diminuir o j, pois se removemos jog[3] o jog[4] vira o [3] e talvez o 4 está sem vidas tbm;
        }
    }
    ///fim da remoção

    if(qt == 0){ //se todos tiverem morrido; volta os mesmos jogadores com as mesmas vidas.
        for(int i = 0; i<BackupQt; i++){
            J[i] = BackupJ[i];
        }
        (*qtd) = BackupQt;
        (*rodada) = -1; //para ser apenas uma carta !!

        printf("\n| | | Teremos rodadas extras para desempate! | | | \n");

        arq = fopen("log.txt", "a");
        fprintf(arq, "\n\t\t\t| | | Rodadas extras para desempate | | |\n");
        fclose(arq);

    }
    free(BackupJ);
}

//autoexplicativa k
void ImprimeVetCartaNoArq(carta M[], int cont){
    FILE *arq = fopen("log.txt", "a");
    for(int i = 0; i<cont; i++){
        int aux = M[i].num;
        if(aux == 10 || aux == 9 || aux == 8 || aux == 1){
            if(aux == 1) aux = 65;
            else if(aux == 10) aux = 75;
            else if(aux == 9) aux = 74;
            else if (aux == 8) aux = 81;
            if(M[i].naip == 1){
                fprintf(arq, "`%c de PAUS`\t",aux);
            }else if(M[i].naip == 2){
                fprintf(arq, "`%c de COPAS`\t",aux);
            }else if(M[i].naip == 3){
                fprintf(arq, "`%c de ESPADA`\t",aux);
            }else{
                fprintf(arq, "`%c de OURO`\t",aux);
            }
        }else{
            if(M[i].naip == 1){
                fprintf(arq, "`%d de PAUS`\t",aux);
            }else if(M[i].naip == 2){
                fprintf(arq, "`%d de COPAS`\t",aux);
            }else if(M[i].naip == 3){
                fprintf(arq, "`%d de ESPADA`\t",aux);
            }else{
                fprintf(arq, "`%d de OURO`\t",aux);
            }
        }
    }
    fprintf(arq, "\n");
    fclose(arq);
}

//autoexplicativa k
void ImprimeCartaNoArq(carta M){
    FILE *arq = fopen("log.txt", "a");
    int aux = M.num;
    if(aux == 10 || aux == 9 || aux == 8 || aux == 1){
        if(aux == 1) aux = 65;
        else if(aux == 10) aux = 75;
        else if (aux == 9) aux = 74;
        else if (aux == 8) aux = 81;
        if(M.naip == 1){
            fprintf(arq, "`%c de PAUS`;\n",aux);
        }else if(M.naip == 2){
            fprintf(arq, "`%c de COPAS`;\n",aux);
        }else if(M.naip == 3){
            fprintf(arq, "`%c de ESPADA`;\n",aux);
        }else{
            fprintf(arq, "`%c de OURO`;\n",aux);
        }
    }else{
        if(M.naip == 1){
            fprintf(arq, "`%d de PAUS`;\n",aux);
        }else if(M.naip == 2){
            fprintf(arq, "`%d de COPAS`;\n",aux);
        }else if(M.naip == 3){
            fprintf(arq, "`%d de ESPADA`;\n",aux);
        }else{
            fprintf(arq, "`%d de OURO`;\n",aux);
        }
    }
    fclose(arq);
}

//nao sei o que acontece aqui, meu getchar() nao tava funcionando, ai pesquisei na net e achei essa função pra "limpar o buffer" de acordo com blog
void limpa_linha() {
    scanf("%*[^\n]");
    scanf("%*c");
}
//END
