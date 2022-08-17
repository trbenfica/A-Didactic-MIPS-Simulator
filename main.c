
#include "interface.h"

int main ()
{
	int menu, control = 0;
	
	// inicializa instâncias
	initialize_instruction_set ();
	initialize_registers ();
	initialize_s_regs ();
	initialize_main_memory ();
	initialize_performance_meter ();
	initialize_cache_l1 ();
	initialize_cache_l2 ();
	initialize_page_table ();
	initialize_TLB ();
	display_hello_message ();
	initialize_display_variables();
	is_on_TLB = 0;
	cicle = 0;

	do {
		if (cicle < amount_of_instr) {
			display ();
			printf ("\n\n--------------------------------------------");
			printf ("\n[Ciclo [%d] | Pressione 1 para continuar e 0 para sair]: ", cicle);
			scanf ("%d", &menu);
			printf("\n\n\n\n");
			printf("Historico de execucao:\n");
			if (menu != 0) {
				manage_reference_bits (); //checagem dos bits de validade da LRU
				process_instruction ();
				if (sync_mode == 0 && write_buffer != NULL && control == 0)
					control ++;
				else if (control > 0) {				
					if (write_buffer->next == NULL) control = 0;
					process_write_buffer ();
				}
				++ cicle;
			}
		}
		else {
			if (sync_mode == 0) {
				while (write_buffer != NULL) {
					printf ("\nLimpando write_buffer...");
					process_write_buffer ();
				}
			}
			display ();
			printf ("\n\nLista de instruções chegou ao fim. Encerrando programa...\n\n");
			break;
		}
	} while (menu != 0);
	printf ("\n");

	return EXIT_SUCCESS;
}

