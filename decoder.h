#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TAM_ENTRADA 1000
#define MAX_TAM_SALIDA 2000
#define TAM_TABLA_MORSE 36

typedef struct {
    char *texto;
    int longitud;
    int capacidad;
} TextBuffer;

typedef struct {
    char caracter;
    char *codigo_morse;
} EntradaMorse;

typedef struct {
    TextBuffer *entrada;
    TextBuffer *salida;
    EntradaMorse *tabla_morse;
} Decodificador;

TextBuffer* crear_texto_buffer(int capacidad_inicial);
void destruir_texto_buffer(TextBuffer *buffer);
void agregar_a_buffer(TextBuffer *buffer, const char *texto);
void limpiar_buffer(TextBuffer *buffer);

Decodificador* crear_decodificador(void);
void destruir_decodificador(Decodificador *d);
void inicializar_tabla_morse(Decodificador *d);

int decodificar_cesar(Decodificador *d, int desplazamiento);

int decodificar_ascii(Decodificador *d);
int decodificar_morse(Decodificador *d);

int es_hex_valido(const char *str);
int es_ascii_valido(const char *str);
int es_morse_valido(const char *str);
char morse_a_caracter(const char *codigo, EntradaMorse *tabla);
void mostrar_menu(void);
void mostrar_resultado(const char *metodo, const char *resultado);

