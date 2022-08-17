
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define STRING_MAX 20
#define SIZE_READ_INSTRUCTION 3
#define AMOUNT_REGISTERS 20
#define AMOUNT_INSTANCES 3
#define RANGE_OF_BYTES 32000 
#define BINARY_ARRAY 15
#define SIZE_BLOCK 4 
#define AMOUNT_PAGES 2048
#define AMOUNT_ENTRY_PAGE_TABLE 99999 // ideal seria 2 ^ 28 = 268.435.456

/* 
##################################################################
# Definindo estruturas                                           #
##################################################################
*/

typedef struct {
	char i [SIZE_READ_INSTRUCTION];
	char reg_dst [SIZE_READ_INSTRUCTION];
	char reg_base [SIZE_READ_INSTRUCTION];
	int offset;
} Instruction;

typedef struct {
	int word[SIZE_BLOCK];
} Block;

typedef struct {
	char* name;
	int content;
} Register;

typedef struct {
	int reference_bit;
	int wb_flag;
} Control;

typedef struct {
	int val_bit;
	int tag;
	Block content;
	Control control;
} Cache_Block;

typedef struct {
	int hits;
	int failures;
} Performance_Meter;

typedef struct _write_buffer {
	Block block;
	int adress;
	struct _write_buffer* next;
} Write_Buffer;

typedef struct entry_page_table {
	int val_bit;
	int physical_adress;
} Entry_Table;

typedef struct tlb {
	int val_bit;
	int tag;
	int content;
} Tlb;

/*
##################################################################
# Instâncias do simulador, declaradas como variáveis globais     #
##################################################################
*/

// Block mp[AMOUNT_PAGES];
// Cache_Block cache_l1 [4][2];
// Cache_Block cache_l2 [8][4];
// Performance_Meter p [AMOUNT_INSTANCES];
// Register registers [AMOUNT_REGISTERS];
// Entry_Table page_table[AMOUNT_ENTRY_PAGE_TABLE];
// Instruction* instructions;
 Write_Buffer* write_buffer;
// Tlb TLB[4];
 int amount_of_instr;
 int cicle;
 int sync_mode;
int is_on_TLB;
// int virtual_adress;
// int physical_adress;
// int mmry_prnt[4];
// int page_prnt;
// int virtual_adress_prnt;
// int virtual_page_prnt;
// int physical_page_prnt;
// int adress_prnt;
// int wb_prnt_0;
// int wb_prnt_1;
// int wb_prnt_2;
// int wb_prnt_3;

/*
##################################################################
# Definição de funções gerais                                    #
##################################################################
*/

// inicializa e preenche o vetor de instruções a partir do arquivo
void initialize_instruction_set ();

// inicializa medidor de perfomance (contador de falhas e acertos)
void initialize_performance_meter ();

// inicializa o banco de registradores
void initialize_registers ();

// incializa memória principal
void initialize_main_memory ();

// inicializa cache L1
void initialize_cache_l1 ();

// inicializa cache L2
void initialize_cache_l2 ();

// inicializa os registradores s[0-5] com zero, para facilitar demonstração
void initialize_s_regs ();

// inicializa tabela de páginas
void initialize_page_table ();

// inicializa cache TLB
void initialize_TLB ();

// inicializa variáveis globais utilizadas por display ()
void initialize_display_variables();

// a partir de uma instrução, modifica as instâncias necessárias
void process_instruction ();

// exibe o conteúdo das instâncias
void display ();

// mostra recomendação e pergunta sobre sincronismo
void display_hello_message ();

// processa a cache, ou fila, de itens empilhados em função do write-through
void process_write_buffer ();

// insere elemento no write buffer
void insert_on_write_buffer (Block block, int adress);

// insere item no buffer de escrita do write-through
void enqueue_write_buffer (Block block);

// remove item do buffer de escrita no write-through, retornando o valor retirado
Write_Buffer* dequeue_write_buffer ();

// gerencia os bits de referência dos blocos da cache. Garante que sempre um dos blocos de ambas as caches estará desativado, tornando possível indicar o LRU das mesmas
void manage_reference_bits ();

