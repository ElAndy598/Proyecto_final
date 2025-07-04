#include "decoder.h"

int main(void) {
    Decoder *decoder = create_decoder();
    if (!decoder) {
        printf("Error: No se pudo inicializar el decodificador.\n");
        return 1;
    }
    
    int option;
    char input[MAX_INPUT_SIZE];
    int shift;
    
    printf("¡Bienvenido al Decodificador Universal!\n");
    
    do {
        print_menu();
        scanf("%d", &option);
        getchar(); // Limpiar buffer
        
        switch (option) {
            case 1: // César
                printf("\nIngrese el texto cifrado con César: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0'; // Remover salto de línea
                
                printf("Ingrese el desplazamiento (shift): ");
                scanf("%d", &shift);
                getchar();
                
                // Limpiar y establecer input
                clear_buffer(decoder->input);
                append_to_buffer(decoder->input, input);
                
                if (decode_caesar(decoder, shift)) {
                    print_result("César", decoder->output->text);
                } else {
                    printf("Error al decodificar César.\n");
                }
                break;
                
            case 2: // Hexadecimal
                printf("\nIngrese el texto en hexadecimal (sin espacios): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                
                clear_buffer(decoder->input);
                append_to_buffer(decoder->input, input);
                
                if (decode_hexadecimal(decoder)) {
                    print_result("Hexadecimal", decoder->output->text);
                } else {
                    printf("Error: Formato hexadecimal inválido.\n");
                }
                break;
                
            case 3: // ASCII
                printf("\nIngrese los códigos ASCII separados por espacios: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                
                clear_buffer(decoder->input);
                append_to_buffer(decoder->input, input);
                
                if (decode_ascii(decoder)) {
                    print_result("ASCII", decoder->output->text);
                } else {
                    printf("Error: Códigos ASCII inválidos.\n");
                }
                break;
                
            case 4: // Morse
                printf("\nIngrese el código Morse (separado por espacios): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                
                clear_buffer(decoder->input);
                append_to_buffer(decoder->input, input);
                
                if (decode_morse(decoder)) {
                    print_result("Morse", decoder->output->text);
                } else {
                    printf("Error: Código Morse inválido.\n");
                }
                break;
                
            case 5:
                printf("\n¡Gracias por usar el Decodificador Universal!\n");
                break;
                
            default:
                printf("Opción inválida. Intente nuevamente.\n");
                break;
        }
        
        if (option != 5) {
            printf("\nPresione Enter para continuar...");
            getchar();
        }
        
    } while (option != 5);
    
    destroy_decoder(decoder);
    return 0;
}
