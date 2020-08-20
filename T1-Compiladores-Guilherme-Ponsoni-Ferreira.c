/*
 * Trabalho I de Compiladores - Analisador Léxico
 * Nome: Guilherme Ponsoni Ferreira
 * RA: 171025367
 * 
 * Compilador utilizado: gcc (tdm64-1) 4.9.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simbolos.h"

FILE *arquivo_entrada, *arquivo_saida;
char simbolo[20], proximo = '\0';
int linha = 1;

int VERIFICA_CHAR(char c, char arr[], int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        if (c == arr[i])
        {
            return 1;
        }
    }
    return 0;
}

int EH_SIMBOLO_ESPECIAL(char c)
{
    return VERIFICA_CHAR(c, simbolos_especiais, sizeof(simbolos_especiais) / sizeof(simbolos_especiais[0]));
}

int EH_LETRA(char c)
{
    return VERIFICA_CHAR(c, letras, sizeof(letras) / sizeof(letras[0]));
}

int EH_DIGITO(char c)
{
    return VERIFICA_CHAR(c, digitos, sizeof(digitos) / sizeof(digitos[0]));
}

int EH_LETRA_OU_DIGITO(char c)
{
    return EH_LETRA(c) || EH_DIGITO(c);
}

int EH_PALAVRA_RESERVADA(char palavra[])
{
    int i;
    for (i = 0; i < sizeof(palavras_reservadas); i++)
    {
        if (!strcmp(palavra, palavras_reservadas[i]))
        {
            return 1;
        }
    }

    return 0;
}

void CONCATENA_CHAR_NA_STRING(char string[], char c)
{
    strcat(string, (char[2]){(char)c, '\0'});
}

void STRING_TO_UPPERCASE(char s[])
{
    int i = 0;

    while (s[i] != '\0')
    {
        if (s[i] >= 'a' && s[i] <= 'z')
        {
            s[i] = s[i] - 32;
        }

        i++;
    }
}

void PROXIMO()
{
    proximo = getc(arquivo_entrada);

    if (proximo == '\n')
    {
        linha++;
        return;
    }
}

void ESCREVER_LINHA()
{
    fprintf(arquivo_saida, "\n%d:", linha);
}

void ESCREVER_SIMBOLO()
{
    fprintf(arquivo_saida, simbolo);
}

void ESCREVER_ESPACO()
{
    fprintf(arquivo_saida, " ");
}

void ERRO(int type)
{
    switch (type)
    {
    case 1:
        printf("\nErro: Simbolo especial desconhecido na linha: %d", linha);
        break;
    case 2:
        printf("\nErro: Identificador ou numero mal formados na linha: %d", linha);
        break;
    default:
        printf("\nErro na linha: %d", linha);
        break;
    }

    exit(2);
}

void CODIGO(char s[], int type)
{
    strcpy(simbolo, "");

    switch (type)
    {
    case 1:
        printf("\nCaracter especial %s", s);
        break;
    case 2:
        printf("\nCaracter especial composto %s", s);
        break;
    case 3:
        ESCREVER_ESPACO();
        strcat(simbolo, "C_");
        printf("\nPalavra reservada %s", s);
        break;
    case 4:
        ESCREVER_ESPACO();
        strcat(simbolo, "ID_");
        printf("\nIdentificador %s", s);
        break;
    case 5:
        ESCREVER_ESPACO();
        printf("\nDigito %s", s);
        break;
    default:
        ERRO(1);
        return;
    }

    strcat(simbolo, s);
    ESCREVER_SIMBOLO();
}

void ANALISADOR_LEXICO()
{
    int type;
    char atomo[10];
    strcpy(atomo, "\0");

    while (proximo == ' ' || proximo == '\t')
    {
        PROXIMO();
    }

    if (EH_SIMBOLO_ESPECIAL(proximo))
    {
        char s[3];
        strcpy(s, "");
        CONCATENA_CHAR_NA_STRING(s, proximo);

        PROXIMO();

        switch (proximo)
        {
        case '=':
            CONCATENA_CHAR_NA_STRING(s, proximo);
            PROXIMO();
            type = 2;
            break;
        default:
            type = 1;
            break;
        }

        CODIGO(s, type);
    }
    else if (EH_LETRA(proximo))
    {
        strcpy(atomo, "");

        do
        {
            CONCATENA_CHAR_NA_STRING(atomo, proximo);
            PROXIMO();
        } while (EH_LETRA_OU_DIGITO(proximo));

        if (EH_PALAVRA_RESERVADA(atomo))
        {
            type = 3;
        }
        else
        {
            type = 4;
        }

        STRING_TO_UPPERCASE(atomo);
        CODIGO(atomo, type);
    }
    else if (EH_DIGITO(proximo))
    {
        strcpy(atomo, "");

        do
        {
            CONCATENA_CHAR_NA_STRING(atomo, proximo);
            PROXIMO();
        } while (EH_DIGITO(proximo));

        if (EH_LETRA(proximo))
        {
            ERRO(2);
            return;
        }

        type = 5;
        CODIGO(atomo, type);
    }
    else if (proximo == '\n')
    {
        ESCREVER_LINHA();
        PROXIMO();
    }
    else if (proximo != ' ' && proximo != '\t' && proximo != EOF)
    {
        ERRO(1);
    }
}

int main()
{
    char nome_arquivo_entrada[23] = "Trab1_Compiladores.txt";
    char nome_arquivo_saida[29] = "Trab1_Compiladores_saida.txt";
    arquivo_entrada = fopen(nome_arquivo_entrada, "r");

    if (!arquivo_entrada)
    {
        printf("Erro - Não foi possível abrir o arquivo: '%s'!", nome_arquivo_entrada);
        exit(1);
    }

    arquivo_saida = fopen(nome_arquivo_saida, "wt");

    if (!arquivo_saida)
    {
        printf("Erro - Não foi possível abrir o arquivo: '%s'!", nome_arquivo_saida);
        exit(1);
    }

    fprintf(arquivo_saida, "%d:", linha);
    PROXIMO();

    while (!feof(arquivo_entrada))
    {
        ANALISADOR_LEXICO();
    }

    return 0;
}