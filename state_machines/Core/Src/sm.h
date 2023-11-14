/*
 * IMPLEMENTAÇÃO DE UM PROTOCOLO
 *
 * | STX | ADDR | QTD_DADOS | DADOS | CHK | ETX |
 *
 * STX			1 byte		Início da transmissão
 * ADDR			2 bytes		Endereço
 * QTD_DADOS	1 byte		Quantidade de Dados
 * DADOS		N bytes		Dados de até 128 bytes
 * CHK			1 byte		Checksum de transmissão
 * ETX			1 byte		Fim de transmissão (\n ou 0x03)
 *
 */

#ifndef SM_H_
#define SM_H_

#include <stdio.h>

/* CONSTANTES USADAS NA COMUNICAÇÃO */
#define STX		'$'
#define ETX		'\n'

/* NÚMERO MÁXIMO DE BYTES DO BUFFER DE DADOS */
#define MAX_BUFFER	128

/* POSSÍVEIS ESTADOS DA MÁQUINA DE ESTADOS DE COMUNICAÇÃO */
typedef enum {
	ST_STX = 0,
	ST_ADDR,
	ST_QTD,
	ST_DATA,
	ST_CHK,
	ST_ETX,
	ST_END
} states_t;

typedef void (*handle_t)(unsigned char *data);
typedef struct StateMachine sm_t;

/* FUNÇÕES EXECUTADAS NOS ESTADOS */
typedef void (*action_t)(sm_t *sm, unsigned char data);

struct StateMachine {
	states_t state;
	unsigned char buffer[MAX_BUFFER];
	unsigned char chk;
	unsigned char qtd;
	int idx;
	char flag_addr;
	unsigned short addr;
	unsigned short my_addr;
	action_t action[ST_END];

	handle_t HandlePackage;
	handle_t HandleError;
};

void initialize(sm_t *sm, unsigned short my_addr, handle_t handle_function, handle_t error_function);
void execute(sm_t *sm, unsigned char *data, int qtd);

#endif
