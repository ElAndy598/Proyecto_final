#include "decodificador.h"

// Tabla de codigo Morse
static const char* caracteres_morse = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static const char* codigos_morse[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", 
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", 
    "--...", "---..", "----."
};

// Funciones para TextBuffer
TextBuffer* crear_texto_buffer(int capacidad_inicial) {
    TextBuffer *buffer = (TextBuffer*)malloc(sizeof(TextBuffer));
    if (!buffer) return NULL;

    buffer->texto = (char*)malloc(capacidad_inicial * sizeof(char));
    if (!buffer->texto) {
        free(buffer);
        return NULL;
    }

    buffer->texto[0] = '\0';
    buffer->longitud = 0;
    buffer->capacidad = capacidad_inicial;

    return buffer;
}

void destruir_texto_buffer(TextBuffer *buffer) {
    if (buffer) {
        if (buffer->texto) {
            free(buffer->texto);
        }
        free(buffer);
    }
}

void agregar_a_buffer(TextBuffer *buffer, const char *texto) {
    if (!buffer || !texto) return;

    int len = strlen(texto);
    int capacidad_necesaria = buffer->longitud + len + 1;

    if (capacidad_necesaria > buffer->capacidad) {
        buffer->capacidad = capacidad_necesaria * 2;
        buffer->texto = (char*)realloc(buffer->texto, buffer->capacidad);
    }

    strcat(buffer->texto, texto);
    buffer->longitud += len;
}

void limpiar_buffer(TextBuffer *buffer) {
    if (buffer && buffer->texto) {
        buffer->texto[0] = '\0';
        buffer->longitud = 0;
    }
}

// Funciones principales del decodificador
Decodificador* crear_decodificador(void) {
    Decodificador *d = (Decodificador*)malloc(sizeof(Decodificador));
    if (!d) return NULL;

    d->entrada = crear_texto_buffer(MAX_TAM_ENTRADA);
    d->salida = crear_texto_buffer(MAX_TAM_SALIDA);
    d->tabla_morse = (EntradaMorse*)malloc(TAM_TABLA_MORSE * sizeof(EntradaMorse));

    if (!d->entrada || !d->salida || !d->tabla_morse) {
        destruir_decodificador(d);
        return NULL;
    }

    inicializar_tabla_morse(d);
    return d;
}

void destruir_decodificador(Decodificador *d) {
    if (d) {
        destruir_texto_buffer(d->entrada);
        destruir_texto_buffer(d->salida);
        if (d->tabla_morse) {
            free(d->tabla_morse);
        }
        free(d);
    }
}

void inicializar_tabla_morse(Decodificador *d) {
    if (!d || !d->tabla_morse) return;

    for (int i = 0; i < TAM_TABLA_MORSE; i++) {
        d->tabla_morse[i].caracter = caracteres_morse[i];
        d->tabla_morse[i].codigo_morse = (char*)codigos_morse[i];
    }
}

// Decodificacion Cesar
int decodificar_cesar(Decodificador *d, int desplazamiento) {
    if (!d || !d->entrada || !d->salida) return 0;

    limpiar_buffer(d->salida);
    char *ptr = d->entrada->texto;
    char caracter_decodificado[2] = {0};

    while (*ptr) {
        if (isalpha(*ptr)) {
            char base = isupper(*ptr) ? 'A' : 'a';
            caracter_decodificado[0] = (((*ptr - base) - desplazamiento + 26) % 26) + base;
        } else {
            caracter_decodificado[0] = *ptr;
        }
        agregar_a_buffer(d->salida, caracter_decodificado);
        ptr++;
    }

    return 1;
}

// Decodificacion ASCII
int decodificar_ascii(Decodificador *d) {
    if (!d || !d->entrada || !d->salida) return 0;
    if (!es_ascii_valido(d->entrada->texto)) return 0;

    limpiar_buffer(d->salida);
    char *ptr = d->entrada->texto;
    char *fin;
    char caracter_decodificado[2] = {0};

    while (*ptr) {
        while (*ptr == ' ') ptr++;
        if (!*ptr) break;

        long valor = strtol(ptr, &fin, 10);

        if (valor >= 32 && valor <= 126) {
            caracter_decodificado[0] = (char)valor;
            agregar_a_buffer(d->salida, caracter_decodificado);
        }

        ptr = fin;
    }

    return 1;
}

// Decodificacion Morse
int decodificar_morse(Decodificador *d) {
    if (!d || !d->entrada || !d->salida) return 0;
    if (!es_morse_valido(d->entrada->texto)) return 0;

    limpiar_buffer(d->salida);
    char *ptr = d->entrada->texto;
    char codigo_morse[10];
    int indice = 0;
    char caracter_decodificado[2] = {0};

    while (*ptr) {
        if (*ptr == ' ' || *ptr == '\0') {
            if (indice > 0) {
                codigo_morse[indice] = '\0';
                char c = morse_a_caracter(codigo_morse, d->tabla_morse);
                if (c != '\0') {
                    caracter_decodificado[0] = c;
                    agregar_a_buffer(d->salida, caracter_decodificado);
                }
                indice = 0;
            }

            while (*ptr == ' ') ptr++;
            continue;
        }

        if ((*ptr == '.' || *ptr == '-') && indice < 9) {
            codigo_morse[indice++] = *ptr;
        }

        ptr++;
    }

    if (indice > 0) {
        codigo_morse[indice] = '\0';
        char c = morse_a_caracter(codigo_morse, d->tabla_morse);
        if (c != '\0') {
            caracter_decodificado[0] = c;
            agregar_a_buffer(d->salida, caracter_decodificado);
        }
    }

    return 1;
}

// Validaciones
int es_ascii_valido(const char *str) {
    if (!str) return 0;

    char *ptr = (char*)str;
    char *fin;

    while (*ptr) {
        while (*ptr == ' ') ptr++;
        if (!*ptr) break;

        long valor = strtol(ptr, &fin, 10);
        if (ptr == fin || valor < 0 || valor > 127) return 0;

        ptr = fin;
    }
    return 1;
}

int es_morse_valido(const char *str) {
    if (!str) return 0;

    while (*str) {
        if (*str != '.' && *str != '-' && *str != ' ') return 0;
        str++;
    }
    return 1;
}

// Conversion Morse a caracter
char morse_a_caracter(const char *codigo, EntradaMorse *tabla) {
    if (!codigo || !tabla) return '\0';

    for (int i = 0; i < TAM_TABLA_MORSE; i++) {
        if (strcmp(codigo, tabla[i].codigo_morse) == 0) {
            return tabla[i].caracter;
        }
    }
    return '\0';
}

// Menu y resultado
void mostrar_menu(void) {
    printf("\nDECODIFICADOR UNIVERSAL\n");
    printf("1. Decodificar Cesar\n");
    printf("2. Decodificar ASCII\n");
    printf("3. Decodificar Morse\n");
    printf("4. Salir\n");
    printf("Seleccione una opcion: ");
}

void mostrar_resultado(const char *metodo, const char *resultado) {
    printf("\n--- Resultado de %s ---\n", metodo);
    printf("Texto decodificado: %s\n", resultado);
    printf("------------------------\n");
}

