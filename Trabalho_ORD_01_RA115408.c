// TRABALHO 01 - ORD
// HENRIQUE RIBEIRO FAVARO
// RA: 115.408

//LED first fit -> Espaços encadeados, e insere no primeiro que couber, só deixa ainda na LED caso o que sobrar seja maior que 50 Bytes

// Sobra menores que 50 Bytes devem ser descartadas quando forem ser inseridos registros no dados.dat

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define TAM_MAX_REG 256
#define DELIM_STR "|"
#define TAM_STR 50

FILE *arq;

short leia_aux(FILE*, char*, int);                    //utilizada para ler as instruções passadas por operações.txt
void menu(int, char[], int, int*);                    //utilizada para gerenciar e executar as instruções passadas por operações;txt
int busca_reg(int, char*, int*, int*);                //faz a busca e confere a existência de um registro com certa chave dentro de dados.dat
int insere_reg(char*, short, int*, int*, int *);   //faz a inserção de um registro dentro da dados.dat, verificando a LED, se não couber em nenhum espaço, coloca no final
int remove_reg(int);                                  //Faz a remoção de um registro, colocando em seu lugar o seekposition da LED onde pode ser utilizado
short leia_reg_first(FILE*, char*, int);              //utilizada para ler os registros do livros.txt -> leitura primaria para jogar em .dat

/* Roda direto na primeira chamada do programa, identificando qual função deve ser 
executada, inserção de dados, consulta ou remoção. Leva como base os parametros 
passados na inicialização do programa para que se possa diferenciar o que fazer dentro
do programa */
int main(int argc, char **argv) {

    FILE *aux;

    if (argc < 3) {
        fprintf(stderr, "Numero incorreto de argumentos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-i|-e) nome_arquivo\n", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "-i") == 0) {

        arq = fopen("dados.dat","wb");
        aux = fopen(argv[2],"r");

        printf("Modo de importacao ativado ... nome do arquivo = %s\n", argv[2]);

        if(arq == NULL)
        {
            printf("\nErro na criação do arquivo final\n...");
            exit(1);
        }

        short tamreg;
        char buffer[TAM_MAX_REG];
        int leng = 0;
        int head = -1;
        int success = 0;
        char lixo[3];

        buffer[0] = '\0';
        tamreg = leia_reg_first(aux, buffer, TAM_MAX_REG);

        fwrite(&head, sizeof(int), 1, arq);

        while (tamreg > 0)
        {
            fwrite(&tamreg,sizeof(short), 1, arq);
            fwrite(buffer, sizeof(char), tamreg, arq);

            buffer[0] = '\0';
            tamreg = leia_reg_first(aux, buffer, TAM_MAX_REG);
            success++;
        }
        if(success > 0)
        {
            printf("Operacao concluida com sucesso", success);
        }
        else
        {
            printf("A operacao falhou");
        }
        fclose(aux);
        fclose(arq);
        return 0;
    } 
    else if (strcmp(argv[1], "-e") == 0) 
    {
        aux = fopen(argv[2],"r");
        arq = fopen("dados.dat","r+b");

        if(arq == NULL)
        {
            printf("\nErro na abertura do arquivo à ser modificado\n...");
            exit(1);
        }

        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);

        short tamreg;
        char keyword[TAM_MAX_REG];
        int type;
        int taminst;
        int success = 0;
        char lixo[3];
        int led;

        fread(&led, sizeof(int), 1, arq);     //Faz a leitura da LED contida no arquivo

        taminst = leia_aux(aux, keyword, TAM_MAX_REG);

        while (taminst > 0)
        {
            if (keyword[0] == 'b')
            {
                type = 1;
                success++;
            }
            else if (keyword[0] == 'i')
            {
                type = 2;
                success++;
            }
            else if(keyword[0] == 'r')
            {
                type = 3;
                success++;
            }
            else
            {
                type = 0;
            }
            
            menu(type, keyword, taminst, &led);
            taminst = leia_aux(aux, keyword, TAM_MAX_REG);
        }
        if(success > 0)
        {
            printf("Operacao concluida com sucesso, foram enviados %d comandos", success);
        }
        else
        {
            printf("O envio dos comandos falhou");
        }
        return 0;
    } 
    else 
    {
        fprintf(stderr, "Opcao \"%s\" nao suportada!\n", argv[1]);
        return 0;
    }
}

/* Função para a leitura de uma linha do arquivo de operacoes, lendo até o
caracter de quebra de linha, e retornando por parâmetro a string lida de 
dentro do arquivo passado por parâmetro também */
short leia_aux(FILE *entrada, char *campo, int tam) 
{
    short i = 0;
    char letter = fgetc(entrada);
    while(letter != EOF && letter != '\n')
    {
        if (tam > 1)
        {   
            campo[i] = letter;
            i++;
            tam--;
        }
        letter = fgetc(entrada);
    }
    campo[i] = '\0';
    return i;
}

/* Função menu, utilizada para selecionar por meio de chaves, as quais foram enviadas
por meio da main, direcionando o programa para uma busca, inserção ou remoção dentro
do arquivo de dados */
void menu(int escolha, char string[], int tam, int *led)
{
    int i;
    char key[5];
    char reg[TAM_MAX_REG];
    char auxy[TAM_MAX_REG];
    int keynumber = 0;
    int cont = 0;
    short sizereg = 0;
    int seekposition = -1;
    int seekleft = -1;
    char *campo;
    int sobra = 0;
    int vazio = 0;

    reg[0] = '\0';

    switch (escolha)
    {
        case 1:

            for(i=2; i<tam; i++)
            {
                key[cont] = string[i];
                cont++;
            }
            key[cont] = '\0';
            keynumber = atoi(key);

            printf("\nBusca pelo registro de chave %d\n", keynumber);

            if(busca_reg(keynumber, reg, &sizereg, &seekposition))
            {
                printf("%s (%d bytes)\n\n", reg, sizereg);
            }
            else
            {
                printf("Erro: Registro nao encontrado!\n\n");
            }

        break;

        case 2:

            for(i=2; i<tam; i++)
            {
                reg[cont] = string[i];
                cont++;
            }
            reg[cont] = '\0';

            sizereg = strlen(reg);
            strcpy(auxy, reg);

            campo = strtok(auxy, DELIM_STR);
            keynumber = atoi(campo);

            printf("\nInsercao do registro de chave %d (%d bytes)\n", keynumber,sizereg);
            sobra = insere_reg(reg, sizereg, &seekleft, &seekposition, &vazio);
            if(sobra > 50)
            {
                printf("Local: offset = %d bytes (0x%x)\n", seekposition, seekposition);
                printf("Tamanho do espaco: %d bytes\n", vazio);
                printf("Tamanho da sobra: %d bytes\n", sobra);
                printf("Offset da sobra: %d bytes (0x%x)\n\n", seekleft, seekleft);
                
            }
            else if(sobra == -1)
            {
                printf("Local: fim do arquivo\n\n");

            }
            else
            {
                printf("Local: offset = %d bytes (0x%x)\n", seekposition, &seekposition);
                printf("Tamanho do espaco: %d bytes\n\n", vazio);
                
            }    

        break;

        case 3:

            for(i=2; i<tam; i++)
            {
                key[cont] = string[i];
                cont++;
            }
            key[cont] = '\0';
            keynumber = atoi(key);

            printf("\nRemocao do registro de chave %d\n", keynumber);

            if(busca_reg(keynumber, reg, &sizereg, &seekposition))
            {
                if(remove_reg(seekposition))
                {
                    printf("Registro removido! (%d bytes)\n", sizereg);
                    printf("Posicao: offset = %d bytes\n\n", seekposition);
                }
                else
                {
                    printf("Erro: Problema interno na remocao!\n\n");
                }
                
            }
            else
            {
                printf("Erro: Registro nao encontrado!\n\n");
            }

        break;

        default:
            return 0;
        break;
    }
}


/* Função para realizar a busca dentro do arquivo de dados retorando sucesso
ou não, e também por parâmetro, retorna o registro encontrado, seu tamanho
e sua posição em relação aos bytes do arquivo de dados */
int busca_reg(int key, char *reg, int *size, int *seekposition)
{
    rewind(arq);                      //leva o ponteiro de arquivo pra posição 0
    int led;
    fread(&led, sizeof(int), 1, arq);

    int found = FALSE;
    short leng;
    char *keyword;
    int keynumber;
    int cont = 0;
    char buffer[TAM_MAX_REG];
    char auxi[TAM_MAX_REG];

    while (found != TRUE)
    {
        *seekposition = ftell(arq);
        fread(&leng, sizeof(short), 1, arq);

        if (feof(arq)) 
        {
            return FALSE;
        }
        if (leng < TAM_MAX_REG)
        {
            fread(buffer, sizeof(char), leng, arq);
            buffer[leng] = '\0';
            strcpy(auxi, buffer);
            keyword = strtok(buffer, DELIM_STR);
            keynumber = atoi(keyword);

            if (key == keynumber)
            {
                strcpy(reg, auxi);
                found = TRUE;
            }
            else
            {
                found = FALSE;
                reg[0] = '\0';
            } 
        }
    }
    if (found)
    {
        *size = leng;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/* Função utilizada para a inserção de um registro dentro do arquvi de dados,
buscando na LED registrada no cabeçalho do arquivo de dados para ver se existe 
ou não uma sobra que se possa inserir tal registro, caso exista a sobra, realiza
a inserção no local, caso contrário, insere no final do arquivo. Se a inserção
em uma sobra resultar em uma sobra maior do que 50 bytes, essa sobra será registrada
no começo da LED de sobras */
int insere_reg(char *reg, short sizereg, int *seekleft, int *seekputted, int *vazio)
{

    short space;        
    short left;        
    int sucess = FALSE;

    int seekposition;
    int last = -1;
    int now = 0;
    int next;
    int led;

    rewind(arq);
    last = -1;
    now = 0;
    fread(&next, sizeof(int), 1, arq);
    led = next;
    
    if(next == -1)
    {
        fseek(arq, 0, SEEK_END);
        fwrite(&sizereg, sizeof(short), 1, arq);
        fwrite(reg, sizeof(char), sizereg, arq);
        return -1;
    }
    else
    {
        last = now;    
        now = next;    
        fseek(arq, next, SEEK_SET);
        fread(&space, sizeof(short), 1, arq);
        fread(&next, sizeof(int), 1, arq);   

        while ((space < sizereg) && now != -1)
        {
            last = now;
            now = next;
            fseek(arq, next, SEEK_SET);
            fread(&space, sizeof(short), 1, arq);
            fread(&next, sizeof(int), 1, arq);
        }

        left = space - sizereg;

        if (now == -1)
        {
            fseek(arq, 0, SEEK_END);
            fwrite(&sizereg, sizeof(short), 1, arq);
            fwrite(reg, sizeof(char), sizereg, arq);
            return -1;
        }
        else if ((left+2) > 50)
        {
            fseek(arq, now, SEEK_SET);
            fwrite(&sizereg, sizeof(short), 1, arq);
            fwrite(reg, sizeof(char), sizereg, arq);
            *seekputted = now;
            left -= 2;
            now = now + sizereg + 2;
            fwrite(&left, sizeof(short), 1, arq);
            fwrite(&led, sizeof(int), 1, arq);
            fseek(arq, (last+2), SEEK_SET);
            fwrite(&next, sizeof(int), 1, arq);
            fseek(arq, 0, SEEK_SET);
            fwrite(&now, sizeof(int), 1, arq);
            *seekleft = now;
            *vazio = space;
            return left;
        }
        else 
        {
            fseek(arq, now, SEEK_SET);
            fwrite(&sizereg, sizeof(short), 1, arq);
            fwrite(reg, sizeof(char), sizereg, arq);
            if (last == 0)
            {   
                fseek(arq, last, SEEK_SET);
            }
            else
            {
                fseek(arq, (last+2), SEEK_SET);
            }
            fwrite(&next, sizeof(int), 1, arq);
            *seekputted = now;
            *seekleft = -1;
            *vazio = space;
            return left;
        }
    }
}

/* Função que realiza a remoção de um registro de dentro do arquivo de dados,
registrando na LED de sobras qual a posição em bytes do arquivo removido, para 
que se possa realizar uma inserção dentro desta sobra futuramente */
int remove_reg(int seekposition)
{
    int led;

    rewind(arq);
    fread(&led, sizeof(int), 1, arq);

    if(led == -1)
    {
        rewind(arq);
        fwrite(&seekposition, sizeof(int), 1, arq);
        seekposition += 2;
        fseek(arq, seekposition, SEEK_SET);
        fwrite(&led, sizeof(int), 1, arq);
        return TRUE;
    }
    else
    {
        seekposition += 2;
        fseek(arq, seekposition, SEEK_SET);
        fwrite(&led, sizeof(int), 1, arq);
        rewind(arq);
        seekposition -= 2;
        fwrite(&seekposition, sizeof(int), 1, arq);
        return TRUE;
    }  
}

/* Função utilizada na criação do arquivo de dados, onde tem-se a leitura
do arquivo de livros, calculando o tamanho de cada registro e os retornando
por meio dos parâmetros para a função main escrevê-los dentro do arquivo
de dados final, o qual será utilizado nas operações futuras */
short leia_reg_first(FILE *fp, char *campo, int tam)
{
    short i = 0;
    char letter = fgetc(fp);
    while(letter != EOF && letter != '\n')
    {
        if (tam > 1)
        {   
            campo[i] = letter;
            i++;
            tam--;
        }
        letter = fgetc(fp);
    }
    campo[i] = '\0';
    return i;
}