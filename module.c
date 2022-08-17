
#include "interface.h"

/*
##################################################################
# Funções principais                                             #
##################################################################
*/


Block mp[AMOUNT_PAGES];
Cache_Block cache_l1 [4][2];
Cache_Block cache_l2 [8][4];
Performance_Meter p [AMOUNT_INSTANCES];
Register registers [AMOUNT_REGISTERS];
Entry_Table page_table[AMOUNT_ENTRY_PAGE_TABLE];
Instruction* instructions;
Write_Buffer* write_buffer;
Tlb TLB[4];
int amount_of_instr;
int cicle;
int sync_mode;
int is_on_TLB;
int virtual_adress;
int physical_adress;
int mmry_prnt[4];
int page_prnt;
int virtual_adress_prnt;
int virtual_page_prnt;
int physical_page_prnt;
int adress_prnt;
int wb_prnt_0;
int wb_prnt_1;
int wb_prnt_2;
int wb_prnt_3;

void initialize_instruction_set ()
{
	Instruction* tmp = (Instruction*) malloc (sizeof (Instruction));
	FILE* ptr = fopen ("instructions.dsk", "r");
	int count = 0;

	printf ("\nCarregando instruções do arquivo...");

	if (ptr == NULL) {
		printf ("\nErro: \"instructions.dsk\" não foi criado ou não está disponível.\n");
		printf ("Encerrando o programa...\n\n");
		exit (EXIT_FAILURE);
	}
	else {
		int j = 0;

		for (char c = getc (ptr); c != EOF; c = getc (ptr))
			if (c == '\n') ++count;

		amount_of_instr = count;
		rewind (ptr);

		instructions = (Instruction*) malloc (sizeof (Instruction) * count);
		while (fscanf (ptr, "%2s $%2s, %d($%2s)", tmp->i, tmp->reg_dst, &tmp->offset, tmp->reg_base) == 4) {
			instructions[j] = *tmp;
			++j;
		}
	}

	printf (" OK\n");
	fclose (ptr);
}

void initialize_registers ()
{
	FILE* ptr = fopen ("names_registers.txt", "r");
	char* name;
	srand (time (0));

	printf ("Inicializando registradores...");

	for (int i = 0; i < AMOUNT_REGISTERS; i++) {
		name = (char*) malloc (sizeof (char) * 5);
		if (fscanf (ptr, "%2s", name) != 1) {
			printf ("\n Erro: arquivo \"names_registers.txt\" incompleto.\n");
			exit (EXIT_FAILURE);
		}
		else {
			registers[i].name = name;
			registers[i].content = rand () % AMOUNT_ENTRY_PAGE_TABLE;
		}
	}
	if (strcmp (registers[0].name, "zr") == 0) registers[0].content = 0;

	printf (" OK\n");
	fclose (ptr);
}

void initialize_s_regs ()
{
	for (int i = 0; i < AMOUNT_REGISTERS; i++)
		if (strcmp (registers[i].name, "s0") == 0) registers[i].content = 0;
	for (int i = 0; i < AMOUNT_REGISTERS; i++)
		if (strcmp (registers[i].name, "s1") == 0) registers[i].content = 0;
	for (int i = 0; i < AMOUNT_REGISTERS; i++)
		if (strcmp (registers[i].name, "s2") == 0) registers[i].content = 0;
	for (int i = 0; i < AMOUNT_REGISTERS; i++)
		if (strcmp (registers[i].name, "s3") == 0) registers[i].content = 0;
	for (int i = 0; i < AMOUNT_REGISTERS; i++)
		if (strcmp (registers[i].name, "s4") == 0) registers[i].content = 0;
	for (int i = 0; i < AMOUNT_REGISTERS; i++)
		if (strcmp (registers[i].name, "s5") == 0) registers[i].content = 0;
	for (int i = 0; i < AMOUNT_REGISTERS; i++)
		if (strcmp (registers[i].name, "s6") == 0) registers[i].content = 0;
	for (int i = 0; i < AMOUNT_REGISTERS; i++)
		if (strcmp (registers[i].name, "s7") == 0) registers[i].content = 0;
	for (int i = 0; i < AMOUNT_REGISTERS; i++)
		if (strcmp (registers[i].name, "s8") == 0) registers[i].content = 0;
}

void initialize_performance_meter ()
{
	printf ("Inicializando medidor de desempenho...");

	// obs: p [0] = cache L1 | p [1] = cache L2 | p [2] = cache TLB

	for (int i = 0; i < AMOUNT_INSTANCES; i++) {
		p[i].hits = 0;
		p[i].failures = 0;
	}

	printf (" OK\n");
}

void initialize_main_memory ()
{
	printf ("Inicializando memória principal...");

	srand (time (0));
	for (int i = 0; i < AMOUNT_PAGES; i++) {
		for (int j = 0; j < SIZE_BLOCK; j++)
			mp[i].word[j] = rand () % 10000;
	}

	printf (" OK\n");
}

void initialize_cache_l1 ()
{
	printf ("Inicializando cache L1...");

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 2; j++) {
			cache_l1 [i][j].val_bit = 0;
			cache_l1 [i][j].tag = 0;
			cache_l1 [i][j].content.word[0] = 0;
			cache_l1 [i][j].content.word[1] = 0;
			cache_l1 [i][j].content.word[2] = 0;
			cache_l1 [i][j].content.word[3] = 0;
			cache_l1 [i][j].control.reference_bit = 0;
			cache_l1 [i][j].control.wb_flag = 0;
		}
	}

	printf (" OK\n");
}

void initialize_cache_l2 ()
{
	printf ("Inicializando cache L2...");

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			cache_l2 [i][j].val_bit = 0;
			cache_l2 [i][j].tag = 0;
			cache_l2 [i][j].content.word[0] = 0;
			cache_l2 [i][j].content.word[1] = 0;
			cache_l2 [i][j].content.word[2] = 0;
			cache_l2 [i][j].content.word[3] = 0;
			cache_l2 [i][j].control.reference_bit = 0;
			cache_l2 [i][j].control.wb_flag = 0;
		}
	}

	printf (" OK\n");
}

void initialize_page_table ()
{
	printf ("Inicializando tabela de páginas...");

	for (int i = 0; i < AMOUNT_ENTRY_PAGE_TABLE; i++) {
		page_table[i].physical_adress = rand () % 2048; 
	}

	printf (" OK\n");
}

void initialize_display_variables() 
{
	for(int a=0; a<4; a++){
		mmry_prnt[a] = 0;		
	}
	page_prnt = 0;

	virtual_adress_prnt = 0;
	virtual_page_prnt = 0;
	physical_page_prnt = 0;
	adress_prnt = 0;
	wb_prnt_0 = 0;
	wb_prnt_1 = 0;
	wb_prnt_2 = 0;
	wb_prnt_3 = 0;

}

void initialize_TLB ()
{
	printf ("Inicializando cache TLB...");

	for (int i = 0; i < 4; i++) {
		TLB[i].val_bit = 0;
		TLB[i].tag = -1;
		TLB[i].content = -1;
	}

	printf (" OK\n");
}
//salvando page virtual e fisica em uma via da TLB
void yank_register_to_TLB (int adress)
{
	int index = adress >> 4, control = 0; //index = pagina virtual

	for (int i = 0; i < 4; i++) {
		if (TLB[i].val_bit == 0) {
			TLB[i].val_bit = 1;
			TLB[i].content = page_table[index].physical_adress; //pagina fisica
			TLB[i].tag = index; //pagina virtual
			control ++;
			break;
		}
	}
	if (control == 0) {
		int tmp = rand () % 3; //seleciona aleatoriamente uma das vias pra serem ocupadas pela nova informacao de pagina virtual e fisica
		TLB[tmp].val_bit = 1;
		TLB[tmp].content = page_table[index].physical_adress;
		TLB[tmp].tag = index;
	}
}

// procura determinado endereço na TLB, se encontrar, retorna a posição do mesmo
int search_on_TLB (int adress)
{
	int result = -1;

	int virtual_adress_tag = adress >> 4; //endereco de pagina virtual
	for (int i = 0; i < 4; i++) {
		if (TLB[i].tag == virtual_adress_tag && TLB[i].val_bit == 1) {
			p[2].hits ++;
			result = i; //result = via q possui a informacao da pagina fisica
			is_on_TLB = 1;
			break;
		}
	}
	if (result == -1) p[2].failures ++;

	return result; 
}

// retorna o índice da via a ser substituída
static int get_lru (int set, int c)
{
	int lru = -1;

	if (c == 1) {
		for(int i = 0; i<2; i++){
		if (cache_l1[set][i].control.reference_bit == 0){
			lru = i;
			i = 2;
	}
		} 
	}
	else if (c == 2) {
		for (int i = 0; i < 4; i++) {
			if (cache_l2[set][i].control.reference_bit == 0) {
				lru = i;
				break;
			}
		}
	}

	return lru;
}

void manage_reference_bits ()
{
for (int i = 0; i < 4; i++) {
		if((cache_l1[i][0].control.reference_bit == 1) && (cache_l1[i][1].control.reference_bit == 1)){			
			cache_l1[i][0].control.reference_bit = 0;
			cache_l1[i][1].control.reference_bit = 0;
			printf ("\nResetando bits de referência para o conjunto %d da cache L1...", i);
		}
	}

	
		for (int i = 0; i < 8; i++) {
		if((cache_l2[i][0].control.reference_bit == 1) && (cache_l2[i][1].control.reference_bit == 1) && (cache_l2[i][2].control.reference_bit == 1) && (cache_l2[i][3].control.reference_bit == 1)){
			cache_l2[i][0].control.reference_bit = 0;
			cache_l2[i][1].control.reference_bit = 0;
			cache_l2[i][2].control.reference_bit = 0;
			cache_l2[i][3].control.reference_bit = 0;
			printf ("\nResetando bits de referência para o conjunto %d da cache L2...", i);
		}
	}
}

// recebe como parametro qual registrador sera usado para calculo do endereco virtual juntamente do offset
static void translate_adress (int base, int offset)
{ 
	virtual_adress = registers[base].content + offset;
	int intermediate;

	// procura na TLB
	int via = search_on_TLB (virtual_adress); //procura na tlb comparando a pagina virtual com as vias da tlb
	if (via != -1) {
		printf ("\nEndereço virtual encontrado na TLB. Atribuíndo endereço físico...");
		intermediate = TLB[via].content; //intermediate = num. pagina fisica
	}
	else {
		printf ("\nEndereço virtual não encontra-se na TLB. Recorrendo à tabela de páginas...");
		intermediate = page_table[virtual_adress >> 4].physical_adress;  //intermediate = num. pagina fisica
		yank_register_to_TLB (virtual_adress); //
	}

	physical_adress = (intermediate << 4) | (virtual_adress & 15); //concatena a pagina fisica com o offset do endereco virtual
}

static int get_index_reg (char* n)
{ //retorna qual registrador sera usado para somar com offset e encontrar o endereco fisico
	int r = -1;

	for (int i = 0; i < AMOUNT_REGISTERS; i++) {
		if (strcmp (registers [i].name, n) == 0) {
			r = i;
			break;
		}
	}

	return r;
}

int get_set_dest_1 (int byte)
{ 
	return ((byte >> 4) & 3);
}

int get_set_dest_2 (int byte)
{
	return ((byte >> 4) & 7);
}

int get_tag_2 (int byte)
{
	return ((byte >> 7) & 67108863);
}

int get_tag_1 (int byte)
{
	return ((byte >> 6) & 67108863);
}

void display_hello_message ()
{
	printf ("\n# Bem vindo ao simulador de memória MIPS!");
	printf ("\n# Afim de comportar o layout da aplicação, recomenda-se executar a mesma em tela cheia.");
	printf ("\n# Acerca do sincronismo, insira \"0\" para  write-through ou \"1\" para write-back: ");
	scanf ("%d", &sync_mode);
}

void display ()
{

	char string_titulo1[300] = "                                    ___           ___                    ___                       ___           ___           ___       ___           ___           ___           ___  " ;
	char string_titulo2[300] = "                                   /\\__\\         /\\__\\                  /\\  \\          ___        /\\__\\         /\\__\\         /\\__\\     /\\  \\         /\\  \\         /\\  \\         /\\  \\ ";
	char string_titulo3[300] = "                                  /::|  |       /:/  /                 /::\\  \\        /\\  \\      /::|  |       /:/  /        /:/  /    /::\\  \\        \\:\\  \\       /::\\  \\       /::\\  \\ ";
	char string_titulo4[300] = "                                 /:|:|  |      /:/__/                 /:/\\ \\  \\       \\:\\  \\    /:|:|  |      /:/  /        /:/  /    /:/\\:\\  \\        \\:\\  \\     /:/\\:\\  \\     /:/\\:\\  \\  ";
	char string_titulo5[300] = "                                /:/|:|__|__   /::\\  \\ ___            _\\:\\~\\ \\  \\      /::\\__\\  /:/|:|__|__   /:/  /  ___   /:/  /    /::\\~\\:\\  \\       /::\\  \\   /:/  \\:\\  \\   /::\\~\\:\\  \\  ";
	char string_titulo6[300] = "                               /:/ |::::\\__\\ /:/\\:\\  /\\__\\          /\\ \\:\\ \\ \\__\\  __/:/\\/__/ /:/ |::::\\__\\ /:/__/  /\\__\\ /:/__/    /:/\\:\\ \\:\\__\\     /:/\\:\\__\\ /:/__/ \\:\\__\\ /:/\\:\\ \\:\\__\\  ";
	char string_titulo7[300] = "                               \\/__/~~/:/  / \\/__\\:\\/:/  /          \\:\\ \\:\\ \\/__/ /\\/:/  /    \\/__/~~/:/  / \\:\\  \\ /:/  / \\:\\  \\    \\/__\\:\\/:/  /    /:/  \\/__/ \\:\\  \\ /:/  / \\/_|::\\/:/  /   ";
	char string_titulo8[300] = "                                     /:/  /       \\::/  /            \\:\\ \\:\\__\\   \\::/__/           /:/  /   \\:\\  /:/  /   \\:\\  \\        \\::/  /    /:/  /       \\:\\  /:/  /     |:|::/  /    " ;
	char string_titulo9[300] = "                                    /:/  /        /:/  /              \\:\\/:/  /    \\:\\__\\          /:/  /     \\:\\/:/  /     \\:\\  \\       /:/  /     \\/__/         \\:\\/:/  /      |:|\\/__/     "   ;
	char string_titulo10[300] ="                                   /:/  /        /:/  /                \\::/  /      \\/__/         /:/  /       \\::/  /       \\:\\__\\     /:/  /                     \\::/  /       |:|  |      "  ;
	char string_titulo11[300] ="                                   \\/__/         \\/__/                  \\/__/                     \\/__/         \\/__/         \\/__/     \\/__/                       \\/__/         \\|__|    ";

	//mostrando o titulo
	printf("\n\n\n%s\n", string_titulo1);   
	printf("%s\n", string_titulo2); 
	printf("%s\n", string_titulo3);
	printf("%s \n", string_titulo4);
	printf("%s \n", string_titulo5);
	printf("%s \n", string_titulo6);
	printf("%s \n", string_titulo7);
	printf("%s \n", string_titulo8);
	printf("%s \n", string_titulo9);
	printf("%s \n", string_titulo10);
	printf("%s \n\n", string_titulo11);





	printf("\n\n\n                                                                               -------------------------------------------------------------\n");
	printf ("                                                                               |              ::::: VETOR DE INSTRUCOES :::::              |\n"); //printando a lista d instru��es na tela e qual � a atual
	printf ("                                                                               |-----------------------------------------------------------|\n");
	printf ("                                                                               |(a seta aponta para a proxima instrucao a ser executada)   |\n");
	printf ("                                                                               |-----------------------------------------------------------|\n");

	for (int i = 0; i < amount_of_instr; i++) {
		if (strcmp (instructions[i].i, "lw") == 0) {
			if (cicle == i) 
				printf ("\t                                                                       |                ------> %s %s %3d (%s)                     |\n", instructions[i].i, instructions[i].reg_dst, instructions[i].offset, instructions[i].reg_base);
			else printf ("\t                                                                       |                       %s %s %3d (%s)                      |\n", instructions[i].i, instructions[i].reg_dst, instructions[i].offset, instructions[i].reg_base);
		}
		else {
			if (cicle == i) 
				printf ("\t                                                                       |                ------> %s %s %3d (%s)                     |\n", instructions[i].i, instructions[i].reg_dst, instructions[i].offset, instructions[i].reg_base);
			else printf ("\t                                                                       |                       %s %s %3d (%s)                      |\n", instructions[i].i, instructions[i].reg_dst, instructions[i].offset, instructions[i].reg_base);
		}
	}
	printf("                                                                               -------------------------------------------------------------\n");


	int index = virtual_adress >> 4; //virtual page
	int content = page_table[index].physical_adress; //physical page
	if(cicle != 0){
		printf ("\n Para o endere�o %d:", virtual_adress);
	}
	
	if (cicle == 0){
		virtual_adress_prnt = 0;
		virtual_page_prnt = 0;
		physical_page_prnt = 0;
		adress_prnt = 0;
	}else if (is_on_TLB && cicle > 0){
		virtual_adress_prnt = 0;
		virtual_page_prnt = 0;
		physical_page_prnt = 0;
		adress_prnt = 0;
	}else{
		virtual_adress_prnt = virtual_adress;
		virtual_page_prnt = index;
		physical_page_prnt = content;
		adress_prnt = physical_adress;
	}

	//calculando dados da mem�ria 

	int pagina;

	if(cicle > 0){
		pagina = physical_adress / 16;
		page_prnt = pagina;
		mmry_prnt[0] = mp[page_prnt].word[0];
		mmry_prnt[1] = mp[page_prnt].word[1];
		mmry_prnt[2] = mp[page_prnt].word[2];
		mmry_prnt[3] = mp[page_prnt].word[3];
	}else{
		pagina = 0;	
	}

	page_prnt = pagina;
	
	//mostrando a tabela
	if(sync_mode == 0){
		Write_Buffer* aux = write_buffer;
		if (aux != NULL) {
			wb_prnt_0 = aux->block.word[0];
			wb_prnt_1 = aux->block.word[1];
			wb_prnt_2 = aux->block.word[2];
			wb_prnt_3 = aux->block.word[3];
			aux = aux->next;
		}
		else {
			wb_prnt_0 = 0;
			wb_prnt_1 = 0;
			wb_prnt_2 = 0;
			wb_prnt_3 = 0;
		}
		printf ("\n######################################################################################################################################################################################################");
		printf ("\n|  REGISTRADORES  |                   CACHE L1                    |                                           CACHE L2                                            |           PERFORMANCE            |");
		printf ("\n|-----------------|-----------------------------------------------|-----------------------------------------------------------------------------------------------|----------------------------------|");
		printf ("\n|                 |          V1           |           V2          |          V1           |          V2           |          V3           |          V4           |    CACHE L1    |    CACHE L2     |");
		printf ("\n|-----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|----------------|-----------------|");
		printf ("\n| $%2s-->%7d   |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|hit:%3d miss:%3d|hit:%3d miss:%3d |", registers [0].name, registers [0].content, cache_l1[0][0].content.word[0], cache_l1[0][0].content.word[1], cache_l1[0][0].content.word[2], cache_l1[0][0].content.word[3], cache_l1[0][1].content.word[0], cache_l1[0][1].content.word[1], cache_l1[0][1].content.word[2], cache_l1[0][1].content.word[3], cache_l2[0][0].content.word[0], cache_l2[0][0].content.word[1], cache_l2[0][0].content.word[2], cache_l2[0][0].content.word[3], cache_l2[0][1].content.word[0], cache_l2[0][1].content.word[1], cache_l2[0][1].content.word[2], cache_l2[0][1].content.word[3], cache_l2[0][2].content.word[0], cache_l2[0][2].content.word[1], cache_l2[0][2].content.word[2], cache_l2[0][2].content.word[3], cache_l2[0][3].content.word[0], cache_l2[0][3].content.word[1], cache_l2[0][3].content.word[2], cache_l2[0][3].content.word[3], p[0].hits, p[0].failures, p[1].hits, p[1].failures);
		printf ("\n| $%2s-->%7d   |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|----------------------------------|", registers [1].name, registers [1].content, cache_l1[1][0].content.word[0], cache_l1[1][0].content.word[1], cache_l1[1][0].content.word[2], cache_l1[1][0].content.word[3], cache_l1[1][1].content.word[0], cache_l1[1][1].content.word[1], cache_l1[1][1].content.word[2], cache_l1[1][1].content.word[3], cache_l2[1][0].content.word[0], cache_l2[1][0].content.word[1], cache_l2[1][0].content.word[2], cache_l2[1][0].content.word[3], cache_l2[1][1].content.word[0], cache_l2[1][1].content.word[1], cache_l2[1][1].content.word[2], cache_l2[1][1].content.word[3], cache_l2[1][2].content.word[0], cache_l2[1][2].content.word[1], cache_l2[1][2].content.word[2], cache_l2[1][2].content.word[3], cache_l2[1][3].content.word[0], cache_l2[1][3].content.word[1], cache_l2[1][3].content.word[2], cache_l2[1][3].content.word[3]);
		printf ("\n| $%2s-->%7d   |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       --------------------       |", registers [2].name, registers [2].content, cache_l1[2][0].content.word[0], cache_l1[2][0].content.word[1], cache_l1[2][0].content.word[2], cache_l1[2][0].content.word[3], cache_l1[2][1].content.word[0], cache_l1[2][1].content.word[1], cache_l1[2][1].content.word[2], cache_l1[2][1].content.word[3], cache_l2[2][0].content.word[0], cache_l2[2][0].content.word[1], cache_l2[2][0].content.word[2], cache_l2[2][0].content.word[3], cache_l2[2][1].content.word[0], cache_l2[2][1].content.word[1], cache_l2[2][1].content.word[2], cache_l2[2][1].content.word[3], cache_l2[2][2].content.word[0], cache_l2[2][2].content.word[1], cache_l2[2][2].content.word[2], cache_l2[2][2].content.word[3], cache_l2[2][3].content.word[0], cache_l2[2][3].content.word[1], cache_l2[2][3].content.word[2], cache_l2[2][3].content.word[3]);
		printf ("\n| $%2s-->%7d   |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       |        TLB       |       |", registers [3].name, registers [3].content, cache_l1[3][0].content.word[0], cache_l1[3][0].content.word[1], cache_l1[3][0].content.word[2], cache_l1[3][0].content.word[3], cache_l1[3][1].content.word[0], cache_l1[3][1].content.word[1], cache_l1[3][1].content.word[2], cache_l1[3][1].content.word[3], cache_l2[3][0].content.word[0], cache_l2[3][0].content.word[1], cache_l2[3][0].content.word[2], cache_l2[3][0].content.word[3], cache_l2[3][1].content.word[0], cache_l2[3][1].content.word[1], cache_l2[3][1].content.word[2], cache_l2[3][1].content.word[3], cache_l2[3][2].content.word[0], cache_l2[3][2].content.word[1], cache_l2[3][2].content.word[2], cache_l2[3][2].content.word[3], cache_l2[3][3].content.word[0], cache_l2[3][3].content.word[1], cache_l2[3][3].content.word[2], cache_l2[3][3].content.word[3]);
		printf ("\n| $%2s-->%7d   |-----------------------------------------------|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       |------------------|       |", registers [4].name, registers [4].content, cache_l2[4][0].content.word[0], cache_l2[4][0].content.word[1], cache_l2[4][0].content.word[2], cache_l2[4][0].content.word[3], cache_l2[4][1].content.word[0], cache_l2[4][1].content.word[1], cache_l2[4][1].content.word[2], cache_l2[4][1].content.word[3], cache_l2[4][2].content.word[0], cache_l2[4][2].content.word[1], cache_l2[4][2].content.word[2], cache_l2[4][2].content.word[3], cache_l2[4][3].content.word[0], cache_l2[4][3].content.word[1], cache_l2[4][3].content.word[2], cache_l2[4][3].content.word[3]);
		printf ("\n| $%2s-->%7d   |                                               |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       | hit:%3d miss:%3d |       |", registers [5].name, registers [5].content, cache_l2[5][0].content.word[0], cache_l2[5][0].content.word[1], cache_l2[5][0].content.word[2], cache_l2[5][0].content.word[3], cache_l2[5][1].content.word[0], cache_l2[5][1].content.word[1], cache_l2[5][1].content.word[2], cache_l2[5][1].content.word[3], cache_l2[5][2].content.word[0], cache_l2[5][2].content.word[1], cache_l2[5][2].content.word[2], cache_l2[5][2].content.word[3], cache_l2[5][3].content.word[0], cache_l2[5][3].content.word[1], cache_l2[5][3].content.word[2], cache_l2[5][3].content.word[3], p[2].hits, p[2].failures);
		printf ("\n| $%2s-->%7d   |                                               |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       |------------------|       |", registers [6].name, registers [6].content, cache_l2[6][0].content.word[0], cache_l2[6][0].content.word[1], cache_l2[6][0].content.word[2], cache_l2[6][0].content.word[3], cache_l2[6][1].content.word[0], cache_l2[6][1].content.word[1], cache_l2[6][1].content.word[2], cache_l2[6][1].content.word[3], cache_l2[6][2].content.word[0], cache_l2[6][2].content.word[1], cache_l2[6][2].content.word[2], cache_l2[6][2].content.word[3], cache_l2[6][3].content.word[0], cache_l2[6][3].content.word[1], cache_l2[6][3].content.word[2], cache_l2[6][3].content.word[3]);
		printf ("\n| $%2s-->%7d   |                                               |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|----------------------------------|", registers [7].name, registers [7].content, cache_l2[7][0].content.word[0], cache_l2[7][0].content.word[1], cache_l2[7][0].content.word[2], cache_l2[7][0].content.word[3], cache_l2[7][1].content.word[0], cache_l2[7][1].content.word[1], cache_l2[7][1].content.word[2], cache_l2[7][1].content.word[3], cache_l2[7][2].content.word[0], cache_l2[7][2].content.word[1], cache_l2[7][2].content.word[2], cache_l2[7][2].content.word[3], cache_l2[7][3].content.word[0], cache_l2[7][3].content.word[1], cache_l2[7][3].content.word[2], cache_l2[7][3].content.word[3]);
		printf ("\n| $%2s --> %5d   |                                               |-----------------------------------------------------------------------------------------------|                                  |", registers [8].name, registers [8].content);
		printf ("\n| $%2s --> %5d   |                                                                                                                                                                                  |", registers [9].name, registers [9].content);
		printf ("\n| $%2s --> %5d   |                                                                                                                                                                                  |", registers [10].name, registers [10].content);
		printf ("\n| $%2s --> %5d   |##################################################################################################################################################################################|", registers [11].name, registers [11].content);
		printf ("\n| $%2s --> %5d   |                            TLB                                |                  TABELA DE PAGINAS                        |       MEM�RIA PRINCIPAL      |     WRITE BUFFER      |", registers [12].name, registers [12].content);
		printf ("\n| $%2s --> %5d   |---------------|---------------|---------------|---------------|----------------|--------------|------------- |------------|-----|------------------------|-----------------------|", registers [13].name, registers [13].content);
		printf ("\n| $%2s --> %5d   |  TAG  |PAG.F  |  TAG  |PAG.F  |  TAG  |PAG.F  |  TAG  |PAG.F  |  END. VIRTUAL  | PAG. VIRTUAL |  PAG. FISICA |END. FISICO |INDEX|      CONTEUDO          |%5d-%5d-%5d-%5d|", registers [14].name, registers [14].content, wb_prnt_0, wb_prnt_1, wb_prnt_2, wb_prnt_3);
		printf ("\n| $%2s --> %5d   |-------|-------|-------|-------|-------|-------|-------|-------|----------------|--------------|--------------|------------|-----|------------------------|-----------------------|", registers [15].name, registers [15].content);
		printf ("\n| $%2s --> %5d   |%7d|%7d|%7d|%7d|%7d|%7d|%7d|%7d|       %7d  |   %7d    |    %5d     |  %5d     |%5d|%5d-%5d-%5d-%5d |                       |", registers [16].name, registers [16].content, TLB[0].tag, TLB[0].content, TLB[1].tag, TLB[1].content, TLB[2].tag, TLB[2].content, TLB[3].tag, TLB[3].content,  virtual_adress_prnt, virtual_page_prnt, physical_page_prnt, adress_prnt, page_prnt, mmry_prnt[0], mmry_prnt[1], mmry_prnt[2], mmry_prnt[3]);
		printf ("\n| $%2s --> %5d   |----------------------------------------------------------------------------------------------------------------------------------------------------------|                       |", registers [17].name, registers [17].content );
		printf ("\n| $%2s --> %5d   |                                                                                                                                                                                  |", registers [18].name, registers [18].content);
		printf ("\n| $%2s --> %5d   |                                                                                                                                                                                  |", registers [19].name, registers [19].content);
		printf ("\n######################################################################################################################################################################################################");
		printf ("\n");

	}

	else if(sync_mode == 1){


		printf ("\n");
		printf ("\n######################################################################################################################################################################################################");
		printf ("\n|  REGISTRADORES  |                   CACHE L1                    |                                           CACHE L2                                            |           PERFORMANCE            |");
		printf ("\n|-----------------|-----------------------------------------------|-----------------------------------------------------------------------------------------------|----------------------------------|");
		printf ("\n|                 |          V1           |           V2          |          V1           |          V2           |          V3           |          V4           |    CACHE L1    |    CACHE L2     |");
		printf ("\n|-----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|----------------|-----------------|");
		printf ("\n| $%2s --> %5d   |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|hit:%3d miss:%3d|hit:%3d miss:%3d |", registers [0].name, registers [0].content, cache_l1[0][0].content.word[0], cache_l1[0][0].content.word[1], cache_l1[0][0].content.word[2], cache_l1[0][0].content.word[3], cache_l1[0][1].content.word[0], cache_l1[0][1].content.word[1], cache_l1[0][1].content.word[2], cache_l1[0][1].content.word[3], cache_l2[0][0].content.word[0], cache_l2[0][0].content.word[1], cache_l2[0][0].content.word[2], cache_l2[0][0].content.word[3], cache_l2[0][1].content.word[0], cache_l2[0][1].content.word[1], cache_l2[0][1].content.word[2], cache_l2[0][1].content.word[3], cache_l2[0][2].content.word[0], cache_l2[0][2].content.word[1], cache_l2[0][2].content.word[2], cache_l2[0][2].content.word[3], cache_l2[0][3].content.word[0], cache_l2[0][3].content.word[1], cache_l2[0][3].content.word[2], cache_l2[0][3].content.word[3], p[0].hits, p[0].failures, p[1].hits, p[1].failures);
		printf ("\n| $%2s --> %5d   |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|----------------------------------|", registers [1].name, registers [1].content, cache_l1[1][0].content.word[0], cache_l1[1][0].content.word[1], cache_l1[1][0].content.word[2], cache_l1[1][0].content.word[3], cache_l1[1][1].content.word[0], cache_l1[1][1].content.word[1], cache_l1[1][1].content.word[2], cache_l1[1][1].content.word[3], cache_l2[1][0].content.word[0], cache_l2[1][0].content.word[1], cache_l2[1][0].content.word[2], cache_l2[1][0].content.word[3], cache_l2[1][1].content.word[0], cache_l2[1][1].content.word[1], cache_l2[1][1].content.word[2], cache_l2[1][1].content.word[3], cache_l2[1][2].content.word[0], cache_l2[1][2].content.word[1], cache_l2[1][2].content.word[2], cache_l2[1][2].content.word[3], cache_l2[1][3].content.word[0], cache_l2[1][3].content.word[1], cache_l2[1][3].content.word[2], cache_l2[1][3].content.word[3]);
		printf ("\n| $%2s --> %5d   |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       ---------------------      |", registers [2].name, registers [2].content, cache_l1[2][0].content.word[0], cache_l1[2][0].content.word[1], cache_l1[2][0].content.word[2], cache_l1[2][0].content.word[3], cache_l1[2][1].content.word[0], cache_l1[2][1].content.word[1], cache_l1[2][1].content.word[2], cache_l1[2][1].content.word[3], cache_l2[2][0].content.word[0], cache_l2[2][0].content.word[1], cache_l2[2][0].content.word[2], cache_l2[2][0].content.word[3], cache_l2[2][1].content.word[0], cache_l2[2][1].content.word[1], cache_l2[2][1].content.word[2], cache_l2[2][1].content.word[3], cache_l2[2][2].content.word[0], cache_l2[2][2].content.word[1], cache_l2[2][2].content.word[2], cache_l2[2][2].content.word[3], cache_l2[2][3].content.word[0], cache_l2[2][3].content.word[1], cache_l2[2][3].content.word[2], cache_l2[2][3].content.word[3]);
		printf ("\n| $%2s --> %5d   |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       |        TLB        |      |", registers [3].name, registers [3].content, cache_l1[3][0].content.word[0], cache_l1[3][0].content.word[1], cache_l1[3][0].content.word[2], cache_l1[3][0].content.word[3], cache_l1[3][1].content.word[0], cache_l1[3][1].content.word[1], cache_l1[3][1].content.word[2], cache_l1[3][1].content.word[3], cache_l2[3][0].content.word[0], cache_l2[3][0].content.word[1], cache_l2[3][0].content.word[2], cache_l2[3][0].content.word[3], cache_l2[3][1].content.word[0], cache_l2[3][1].content.word[1], cache_l2[3][1].content.word[2], cache_l2[3][1].content.word[3], cache_l2[3][2].content.word[0], cache_l2[3][2].content.word[1], cache_l2[3][2].content.word[2], cache_l2[3][2].content.word[3], cache_l2[3][3].content.word[0], cache_l2[3][3].content.word[1], cache_l2[3][3].content.word[2], cache_l2[3][3].content.word[3]);
		printf ("\n| $%2s --> %5d   |-----------------------------------------------|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       |-------------------|      |", registers [4].name, registers [4].content, cache_l2[4][0].content.word[0], cache_l2[4][0].content.word[1], cache_l2[4][0].content.word[2], cache_l2[4][0].content.word[3], cache_l2[4][1].content.word[0], cache_l2[4][1].content.word[1], cache_l2[4][1].content.word[2], cache_l2[4][1].content.word[3], cache_l2[4][2].content.word[0], cache_l2[4][2].content.word[1], cache_l2[4][2].content.word[2], cache_l2[4][2].content.word[3], cache_l2[4][3].content.word[0], cache_l2[4][3].content.word[1], cache_l2[4][3].content.word[2], cache_l2[4][3].content.word[3]);
		printf ("\n| $%2s --> %5d   |                                               |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       | hit:%3d miss:%3d  |      |", registers [5].name, registers [5].content, cache_l2[5][0].content.word[0], cache_l2[5][0].content.word[1], cache_l2[5][0].content.word[2], cache_l2[5][0].content.word[3], cache_l2[5][1].content.word[0], cache_l2[5][1].content.word[1], cache_l2[5][1].content.word[2], cache_l2[5][1].content.word[3], cache_l2[5][2].content.word[0], cache_l2[5][2].content.word[1], cache_l2[5][2].content.word[2], cache_l2[5][2].content.word[3], cache_l2[5][3].content.word[0], cache_l2[5][3].content.word[1], cache_l2[5][3].content.word[2], cache_l2[5][3].content.word[3], p[2].hits, p[2].failures);
		printf ("\n| $%2s --> %5d   |                                               |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|       |-------------------|      |", registers [6].name, registers [6].content, cache_l2[6][0].content.word[0], cache_l2[6][0].content.word[1], cache_l2[6][0].content.word[2], cache_l2[6][0].content.word[3], cache_l2[6][1].content.word[0], cache_l2[6][1].content.word[1], cache_l2[6][1].content.word[2], cache_l2[6][1].content.word[3], cache_l2[6][2].content.word[0], cache_l2[6][2].content.word[1], cache_l2[6][2].content.word[2], cache_l2[6][2].content.word[3], cache_l2[6][3].content.word[0], cache_l2[6][3].content.word[1], cache_l2[6][3].content.word[2], cache_l2[6][3].content.word[3]);
		printf ("\n| $%2s --> %5d   |                                               |%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|%5d-%5d-%5d-%5d|----------------------------------|", registers [7].name, registers [7].content, cache_l2[7][0].content.word[0], cache_l2[7][0].content.word[1], cache_l2[7][0].content.word[2], cache_l2[7][0].content.word[3], cache_l2[7][1].content.word[0], cache_l2[7][1].content.word[1], cache_l2[7][1].content.word[2], cache_l2[7][1].content.word[3], cache_l2[7][2].content.word[0], cache_l2[7][2].content.word[1], cache_l2[7][2].content.word[2], cache_l2[7][2].content.word[3], cache_l2[7][3].content.word[0], cache_l2[7][3].content.word[1], cache_l2[7][3].content.word[2], cache_l2[7][3].content.word[3]);
		printf ("\n| $%2s --> %5d   |                                               |-----------------------------------------------------------------------------------------------|                                  |", registers [8].name, registers [8].content);
		printf ("\n| $%2s --> %5d   |                                                                                                                                                                                  |", registers [9].name, registers [9].content);
		printf ("\n| $%2s --> %5d   |                                                                                                                                                                                  |", registers [10].name, registers [10].content);
		printf ("\n| $%2s --> %5d   |###########################################################################################################################################################                       |", registers [11].name, registers [11].content);
		printf ("\n| $%2s --> %5d   |                            TLB                                |                  TABELA DE PAGINAS                        |       MEM�RIA PRINCIPAL      |                       |", registers [12].name, registers [12].content);
		printf ("\n| $%2s --> %5d   |---------------|---------------|---------------|---------------|----------------|--------------|--------------|------------|-----|------------------------|                       |", registers [13].name, registers [13].content);
		printf ("\n| $%2s --> %5d   |  TAG  |PAG.F  |  TAG  |PAG.F  |  TAG  |PAG.F  |  TAG  |PAG.F  |  END. VIRTUAL  | PAG. VIRTUAL |  PAG. FISICA |END. FISICO |INDEX|      CONTEUDO          |                       |", registers [14].name, registers [14].content);
		printf ("\n| $%2s --> %5d   |-------|-------|-------|-------|-------|-------|-------|-------|----------------|--------------|--------------|------------|-----|------------------------|                       |", registers [15].name, registers [15].content);
		printf ("\n| $%2s --> %5d   |%7d|%7d|%7d|%7d|%7d|%7d|%7d|%7d|       %7d  |   %7d    |    %5d     |  %5d     |%5d|%5d-%5d-%5d-%5d |                       |", registers [16].name, registers [16].content, TLB[0].tag, TLB[0].content, TLB[1].tag, TLB[1].content, TLB[2].tag, TLB[2].content, TLB[3].tag, TLB[3].content,  virtual_adress_prnt, virtual_page_prnt, physical_page_prnt, adress_prnt, page_prnt, mmry_prnt[0], mmry_prnt[1], mmry_prnt[2], mmry_prnt[3]);
		printf ("\n| $%2s --> %5d   |----------------------------------------------------------------------------------------------------------------------------------------------------------|                       |", registers [17].name, registers [17].content );
		printf ("\n| $%2s --> %5d   |                                                                                                                                                                                  |", registers [18].name, registers [18].content);
		printf ("\n| $%2s --> %5d   |                                                                                                                                                                                  |", registers [19].name, registers [19].content);
		printf ("\n######################################################################################################################################################################################################");
		printf ("\n");


	}
}

void process_write_buffer () 
{
	printf ("\nProcessando buffer de escrita...");
	if (write_buffer != NULL) {
		Write_Buffer* aux = dequeue_write_buffer ();
		int tag_l2 = get_tag_2 (aux->adress); 
		int set_l2 = get_set_dest_2 (aux->adress); 
		int wich_block = aux->adress / 16;
		int control = 0;
		for (int i = 0; i < 4; i++) { 
			if (cache_l2 [set_l2][i].tag == tag_l2) {
				printf ("\nSobrescrevendo bloco desatualizado na L2");
				cache_l2 [set_l2][i].content = aux->block;
				control ++;
				break;
			}
		}
		if (control == 0) {
			int via_l2 = get_lru (set_l2, 2);
			cache_l2[set_l2][via_l2].content = aux->block;
		}
		printf ("\nSobrescrevendo bloco desatualizado memória principal");
		mp[wich_block] = aux->block;
	}
}

int yank_block_to_L1 (Block block)
{
	int set_l1 = get_set_dest_1 (physical_adress); 
	int tag_l1 = get_tag_1 (physical_adress); 
	int subst_block = get_lru (set_l1, 1);

	if (sync_mode == 1 && cache_l1 [set_l1][subst_block].control.wb_flag == 1) {
		printf ("\n[Write-back] Tentativa de substituir bloco com conteúdo a ser propagado...");
		int set_l2 = get_set_dest_2 (physical_adress); //conjunto alvo
		int tag_l2 = get_tag_2 (physical_adress); 
		int subst_block_l2 = get_lru (set_l2, 2);
		int wich_block = physical_adress / 16;

		for (int i = 0; i < 4; i++) { 
			if (cache_l2 [set_l2][i].tag == tag_l2) {
				printf ("\n[Write-back] Atualizando bloco correspondente L2 antes de ser substituído");
				cache_l2 [set_l2][subst_block_l2].content = cache_l1 [set_l1][subst_block].content; // propaga escrita para L2
				break;
			}
		}
		printf ("\n[Write-back] Atualizando bloco correspondente na memória principal antes de ser substituído");
		mp[wich_block] = cache_l1 [set_l1][subst_block].content; // propaga escrita para MP
		cache_l1 [set_l1][subst_block].control.wb_flag = 0;
	}

	printf ("\nPuxando bloco da L2 para L1");
	cache_l1 [set_l1][subst_block].content = block; 
	cache_l1 [set_l1][subst_block].val_bit = 1; 
	cache_l1 [set_l1][subst_block].tag = tag_l1;
	cache_l1 [set_l1][subst_block].control.reference_bit = 1;
	return subst_block;
}

int yank_block_to_L2 (Block block)
{
	printf ("\nPuxando bloco da mp para L2");
	int set_l2 = get_set_dest_2 (physical_adress); //conjunto alvo
	int tag_l2 = get_tag_2 (physical_adress); 
	int subst_block = get_lru (set_l2, 2); //retorna a via
	cache_l2 [set_l2][subst_block].content = block; 
	cache_l2 [set_l2][subst_block].val_bit = 1; 
	cache_l2 [set_l2][subst_block].tag = tag_l2;
	cache_l2 [set_l2][subst_block].control.reference_bit = 1;
	return subst_block;
}

void enqueue_write_buffer (Block block)
{
	Write_Buffer* new = (Write_Buffer*) malloc (sizeof (Write_Buffer));
	new->block = block;
	new->adress = physical_adress;
	new->next = NULL;

	printf ("\nEnfileirando bloco modificado no write buffer");
	if (write_buffer == NULL) {
		write_buffer = (Write_Buffer*) malloc (sizeof (Write_Buffer));
		write_buffer = new;
	}
	else if (write_buffer->next == NULL)
		write_buffer->next = new;
	else {
		Write_Buffer* tmp = write_buffer;
		while (tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = new;
	}
}

Write_Buffer* dequeue_write_buffer ()
{
	Write_Buffer* tmp = write_buffer;

	if (tmp != NULL) {
		*tmp = *write_buffer;
		if (tmp->next == NULL) write_buffer = NULL;
		else write_buffer = write_buffer->next; 
	}

	return tmp;
}

void process_instruction ()
{
	is_on_TLB = 0; // reseta estado da variavel que informa se a info de pagina est� na tlb
	int base = get_index_reg (instructions[cicle].reg_base); // define qual registrador base ser� usado para calculo do end. virtual
	translate_adress (base, instructions[cicle].offset); // configura variável global physical_adress
	int dst = get_index_reg (instructions[cicle].reg_dst); // index do registrador destino
	int set_l1 = get_set_dest_1 (physical_adress); // conjunto destino (cache L1)
	int tag_l1 = get_tag_1 (physical_adress); // obtém TAG do endereço
	int is_on_L2 = 0; // aciona a busca na cache L2
	int is_on_mp = 0; // aciona a busca na memória principal
	int control = 0; // afirma se o valor foi encontrado após buscas nas vias
	int via = -1; // qual via da L1 contém o bloco desejado (é determinada ao longo da função)
	int wich_word = (physical_adress / 4) % 4; 


	// busca por bloco na L1
	for (int j = 0; j < 2; j++) { 
		if (cache_l1 [set_l1][j].tag == tag_l1) { 
			if (cache_l1 [set_l1][j].val_bit == 1) { 
				printf ("\nBloco encontrado na L1");
				cache_l1 [set_l1][j].control.reference_bit = 1;
				p[0].hits ++; 
				control ++;
				via = j;
				break;
			}
		}
	}
	if (control == 0) { 
		printf ("\nBloco não encontrado na L1");
		p[0].failures ++;
		is_on_L2 = 1;
	}
	// caso não ache, procura na L2
	if (is_on_L2) {
		int set_l2 = get_set_dest_2 (physical_adress); // index da cache L2
		int tag_l2 = get_tag_2 (physical_adress); // obtém TAG do endereço
		for (int j = 0; j < 4; j++) {
			if (cache_l2 [set_l2][j].tag == tag_l2) {
				if (cache_l2 [set_l2][j].val_bit == 1) { 
					printf ("\nBloco encontrado na L2");
					cache_l2 [set_l2][j].control.reference_bit = 1;
					p[1].hits ++;
					control ++;
					via = yank_block_to_L1 (cache_l2 [set_l2][j].content);
					break;
				}
			}
		}
		if (control == 0) {
			printf ("\nBloco não encontrado na L2");
			p[1].failures ++;
			is_on_mp = 1;
		}
	}
	// caso não ache, procura na MP
	if (is_on_mp) {
		int wich_block = physical_adress / 16;
		yank_block_to_L2 (mp[wich_block]);
		via = yank_block_to_L1 (mp[wich_block]);
	}

	// uma vez aqui, sabemos que o bloco estava ou foi movido para L1 e podemos então processar a instrução

	if (strcmp (instructions[cicle].i, "lw") == 0) {
		printf ("\nEscrevendo palavra no registrador");
		registers [dst].content = cache_l1 [set_l1][via].content.word[wich_word];
	}

	else if (strcmp (instructions[cicle].i, "sw") == 0) {
		printf ("\nEscrevendo palavra no bloco");
		cache_l1 [set_l1][via].content.word[wich_word] = registers [dst].content;
		if (sync_mode == 0) enqueue_write_buffer (cache_l1 [set_l1][via].content);
		else cache_l1 [set_l1][via].control.wb_flag = 1;
	}
}

