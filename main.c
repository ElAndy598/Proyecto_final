#include "decodificador.h"

int main(void) {
    Decodificador *decodificador = crear_decodificador();
    if (!decodificador) {
        printf("Error: No se pudo inicializar el decodificador.\n");
        return 1;
    }
    
    int opcion;
    char entrada[MAX_TAM_ENTRADA];
    int desplazamiento;
    
    printf("Bienvenido\n");
    
    do {
        mostrar_menu();
        scanf("%d", &opcion);
        getchar(); // Limpiar buffer
        
        switch (opcion) {
            case 1: // Cesar
                printf("\nIngrese el texto cifrado con Cesar: ");
                fgets(entrada, sizeof(entrada), stdin);
                entrada[strcspn(entrada, "\n")] = '\0'; // Remover salto de linea
                
                printf("Ingrese el desplazamiento: ");
                scanf("%d", &desplazamiento);
                getchar();
                
                limpiar_buffer(decodificador->entrada);
                agregar_a_buffer(decodificador->entrada, entrada);
                
                if (decodificar_cesar(decodificador, desplazamiento)) {
                    mostrar_resultado("Cesar", decodificador->salida->texto);
                } else {
                    printf("Error al decodificar Cesar.\n");
                }
                break;
                
            case 2: // ASCII
                printf("\nIngrese los codigos ASCII separados por espacios: ");
                fgets(entrada, sizeof(entrada), stdin);
                entrada[strcspn(entrada, "\n")] = '\0';
                
                limpiar_buffer(decodificador->entrada);
                agregar_a_buffer(decodificador->entrada, entrada);
                
                if (decodificar_ascii(decodificador)) {
                    mostrar_resultado("ASCII", decodificador->salida->texto);
                } else {
                    printf("Error: Codigos ASCII invalidos.\n");
                }
                break;
                
            case 3: // Morse
                printf("\nIngrese el codigo Morse (separado por espacios): ");
                fgets(entrada, sizeof(entrada), stdin);
                entrada[strcspn(entrada, "\n")] = '\0';
                
                limpiar_buffer(decodificador->entrada);
                agregar_a_buffer(decodificador->entrada, entrada);
                
                if (decodificar_morse(decodificador)) {
                    mostrar_resultado("Morse", decodificador->salida->texto);
                } else {
                    printf("Error: Codigo Morse invalido.\n");
                }
                break;
                
            case 4:
                printf("\nGracias por usar el Decodificador Universal\n");
                break;
                
            default:
                printf("Opcion invalida. Intente nuevamente.\n");
                break;
        }
        
        if (opcion != 4) {
            printf("\nPresione Enter para continuar");
            getchar();
        }
        
    } while (opcion != 4);
    
    destruir_decodificador(decodificador);
    return 0;
}
