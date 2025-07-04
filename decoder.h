#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_SIZE 1000
#define MAX_OUTPUT_SIZE 2000
#define MORSE_TABLE_SIZE 36

// Estructura para almacenar el texto y su longitud
typedef struct {
    char *text;
    int length;
    int capacity;
} TextBuffer;

// Estructura para el código Morse
typedef struct {
    char character;
    char *morse_code;
} MorseEntry;

// Estructura principal del decodificador
typedef struct {
    TextBuffer *input;
    TextBuffer *output;
    MorseEntry *morse_table;
} Decoder;

// Funciones para manejo de TextBuffer
TextBuffer* create_text_buffer(int initial_capacity);
void destroy_text_buffer(TextBuffer *buffer);
void append_to_buffer(TextBuffer *buffer, const char *text);
void clear_buffer(TextBuffer *buffer);

// Funciones principales de decodificación
Decoder* create_decoder(void);
void destroy_decoder(Decoder *decoder);
void init_morse_table(Decoder *decoder);

// Funciones de decodificación específicas
int decode_caesar(Decoder *decoder, int shift);
int decode_hexadecimal(Decoder *decoder);
int decode_ascii(Decoder *decoder);
int decode_morse(Decoder *decoder);

// Funciones auxiliares
int is_valid_hex(const char *str);
int is_valid_ascii_codes(const char *str);
int is_valid_morse(const char *str);
char morse_to_char(const char *morse_code, MorseEntry *table);
void print_menu(void);
void print_result(const char *method, const char *result);

#endif
