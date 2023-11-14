#include "sm.h"

static void actionSTX(sm_t *sm, unsigned char data) {
	if(data == STX){
		sm->idx = 0;
		sm->qtd	= 0;
		sm->chk = 0;
		sm->state = ST_ADDR;
		sm->flag_addr = 0;
	}
}

static void actionAddr(sm_t *sm, unsigned char data){
	// Receber o endereço no formato little endian
	if(!sm->flag_addr) {
		sm->addr = (unsigned short)(data);
		sm->chk = data;
		sm->flag_addr = 1;
	} else {
		sm->flag_addr = 0;
		sm->addr |= (unsigned short)(data << 8);

		if(sm->addr == sm->my_addr){
			sm->state = ST_QTD;
			sm->chk += data;
		} else {
			sm->state = ST_STX;

			if(sm->HandleError != NULL){
				sm->HandleError("Endereço Errado...\n\r");
			}
		}
	}
}

static void actionQtd(sm_t *sm, unsigned char data) {
	if((data >= 1) && (data <= 128)){
		sm->qtd = data;
		sm->chk += data;
		sm->state = ST_DATA;
	} else {
		sm->state = ST_STX;
	}
}

static void actionData(sm_t *sm, unsigned char data) {
	sm->buffer[sm->idx++] = data;
	sm->chk += data;

	if(--sm->qtd == 0){
		sm->state = ST_CHK;
	}
}

static void actionChk(sm_t *sm, unsigned char data) {
	if(data == sm->chk) {
		sm->state = ST_ETX;
	} else {
		sm->state = ST_STX;

		if(sm->HandleError != NULL){
			sm->HandleError("Falha no Checksum...\n\r");
		}
	}
}

static void actionETX(sm_t *sm, unsigned char data) {
	if(data == ETX){
		sm->buffer[sm->idx] = '\0';

		if(sm->HandlePackage != NULL){
			sm->HandlePackage(sm->buffer);
		}
	}
}

void initialize(sm_t *sm, unsigned short my_addr, handle_t handle_function, handle_t error_function) {
	sm->state = ST_STX;
	sm->my_addr = my_addr;
	sm->buffer[0] = 0;
	sm->chk = 0;
	sm->idx = 0;
	sm->qtd = 0;
	sm->action[ST_STX] = actionSTX;
	sm->action[ST_ADDR] = actionAddr;
	sm->action[ST_QTD] = actionQtd;
	sm->action[ST_DATA] = actionData;
	sm->action[ST_CHK] = actionChk;
	sm->action[ST_ETX] = actionETX;
	sm->HandlePackage = handle_function;
	sm->HandleError = error_function;
}

void execute(sm_t *sm, unsigned char *data, int qtd){
	int i;

	for(i = 0; i < qtd; i++){
		sm->action[sm->state](sm, data[i]);
	}
}
