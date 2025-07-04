#include "decoder.h"

// Tabla de código Morse
static const char* morse_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static const char* morse_codes[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", 
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", 
    "--...", "---..", "----."
};

// Funciones para TextBuffer
TextBuffer* create_text_buffer(int initial_capacity) {
    TextBuffer *buffer = (TextBuffer*)malloc(sizeof(TextBuffer));
    if (!buffer) return NULL;
    
    buffer->text = (char*)malloc(initial_capacity * sizeof(char));
    if (!buffer->text) {
        free(buffer);
        return NULL;
    }
    
    buffer->text[0] = '\0';
    buffer->length = 0;
    buffer->capacity = initial_capacity;
    
    return buffer;
}

void destroy_text_buffer(TextBuffer *buffer) {
    if (buffer) {
        if (buffer->text) {
            free(buffer->text);
        }
        free(buffer);
    }
}

void append_to_buffer(TextBuffer *buffer, const char *text) {
    if (!buffer || !text) return;
    
    int text_len = strlen(text);
    int needed_capacity = buffer->length + text_len + 1;
    
    if (needed_capacity > buffer->capacity) {
        buffer->capacity = needed_capacity * 2;
        buffer->text = (char*)realloc(buffer->text, buffer->capacity);
    }
    
    strcat(buffer->text, text);
    buffer->length += text_len;
}

void clear_buffer(TextBuffer *buffer) {
    if (buffer && buffer->text) {
        buffer->text[0] = '\0';
        buffer->length = 0;
    }
}

// Funciones principales del decodificador
Decoder* create_decoder(void) {
    Decoder *decoder = (Decoder*)malloc(sizeof(Decoder));
    if (!decoder) return NULL;
    
    decoder->input = create_text_buffer(MAX_INPUT_SIZE);
    decoder->output = create_text_buffer(MAX_OUTPUT_SIZE);
    decoder->morse_table = (MorseEntry*)malloc(MORSE_TABLE_SIZE * sizeof(MorseEntry));
    
    if (!decoder->input || !decoder->output || !decoder->morse_table) {
        destroy_decoder(decoder);
        return NULL;
    }
    
    init_morse_table(decoder);
    return decoder;
}

void destroy_decoder(Decoder *decoder) {
    if (decoder) {
        destroy_text_buffer(decoder->input);
        destroy_text_buffer(decoder->output);
        if (decoder->morse_table) {
            free(decoder->morse_table);
        }
        free(decoder);
    }
}

void init_morse_table(Decoder *decoder) {
    if (!decoder || !decoder->morse_table) return;
    
    for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
        decoder->morse_table[i].character = morse_chars[i];
        decoder->morse_table[i].morse_code = (char*)morse_codes[i];
    }
}

// Decodificación César
int decode_caesar(Decoder *decoder, int shift) {
    if (!decoder || !decoder->input || !decoder->output) return 0;
    
    clear_buffer(decoder->output);
    char *input_ptr = decoder->input->text;
    char decoded_char[2] = {0};
    
    while (*input_ptr) {
        if (isalpha(*input_ptr)) {
            char base = isupper(*input_ptr) ? 'A' : 'a';
            decoded_char[0] = (((*input_ptr - base) - shift + 26) % 26) + base;
        } else {
            decoded_char[0] = *input_ptr;
        }
        append_to_buffer(decoder->output, decoded_char);
        input_ptr++;
    }
    
    return 1;
}

// Decodificación Hexadecimal
int decode_hexadecimal(Decoder *decoder) {
    if (!decoder || !decoder->input || !decoder->output) return 0;
    if (!is_valid_hex(decoder->input->text)) return 0;
    
    clear_buffer(decoder->output);
    char *input_ptr = decoder->input->text;
    char decoded_char[2] = {0};
    
    while (*input_ptr && *(input_ptr + 1)) {
        char hex_pair[3] = {*input_ptr, *(input_ptr + 1), '\0'};
        int ascii_value = (int)strtol(hex_pair, NULL, 16);
        
        if (ascii_value >= 32 && ascii_value <= 126) {
            decoded_char[0] = (char)ascii_value;
            append_to_buffer(decoder->output, decoded_char);
        }
        
        input_ptr += 2;
    }
    
    return 1;
}

// Decodificación ASCII
int decode_ascii(Decoder *decoder) {
    if (!decoder || !decoder->input || !decoder->output) return 0;
    if (!is_valid_ascii_codes(decoder->input->text)) return 0;
    
    clear_buffer(decoder->output);
    char *input_ptr = decoder->input->text;
    char *end_ptr;
    char decoded_char[2] = {0};
    
    while (*input_ptr) {
        while (*input_ptr == ' ') input_ptr++; // Saltar espacios
        if (!*input_ptr) break;
        
        long ascii_value = strtol(input_ptr, &end_ptr, 10);
        
        if (ascii_value >= 32 && ascii_value <= 126) {
            decoded_char[0] = (char)ascii_value;
            append_to_buffer(decoder->output, decoded_char);
        }
        
        input_ptr = end_ptr;
    }
    
    return 1;
}

// Decodificación Morse
int decode_morse(Decoder *decoder) {
    if (!decoder || !decoder->input || !decoder->output) return 0;
    if (!is_valid_morse(decoder->input->text)) return 0;
    
    clear_buffer(decoder->output);
    char *input_ptr = decoder->input->text;
    char morse_code[10];
    int morse_index = 0;
    char decoded_char[2] = {0};
    
    while (*input_ptr) {
        if (*input_ptr == ' ' || *input_ptr == '\0') {
            if (morse_index > 0) {
                morse_code[morse_index] = '\0';
                char ch = morse_to_char(morse_code, decoder->morse_table);
                if (ch != '\0') {
                    decoded_char[0] = ch;
                    append_to_buffer(decoder->output, decoded_char);
                }
                morse_index = 0;
            }
            
            // Saltar múltiples espacios
            while (*input_ptr == ' ') input_ptr++;
            continue;
        }
        
        if ((*input_ptr == '.' || *input_ptr == '-') && morse_index < 9) {
            morse_code[morse_index++] = *input_ptr;
        }
        
        input_ptr++;
    }
    
    // Procesar último código si existe
    if (morse_index > 0) {
        morse_code[morse_index] = '\0';
        char ch = morse_to_char(morse_code, decoder->morse_table);
        if (ch != '\0') {
            decoded_char[0] = ch;
            append_to_buffer(decoder->output, decoded_char);
        }
    }
    
    return 1;
}

// Funciones auxiliares
int is_valid_hex(const char *str) {
    if (!str || strlen(str) % 2 != 0) return 0;
    
    while (*str) {
        if (!isxdigit(*str)) return 0;
        str++;
    }
    return 1;
}

int is_valid_ascii_codes(const char *str) {
    if (!str) return 0;
    
    char *ptr = (char*)str;
    char *end_ptr;
    
    while (*ptr) {
        while (*ptr == ' ') ptr++;
        if (!*ptr) break;
        
        long value = strtol(ptr, &end_ptr, 10);
        if (ptr == end_ptr || value < 0 || value > 127) return 0;
        
        ptr = end_ptr;
    }
    return 1;
}

int is_valid_morse(const char *str) {
    if (!str) return 0;
    
    while (*str) {
        if (*str != '.' && *str != '-' && *str != ' ') return 0;
        str++;
    }
    return 1;
}

char morse_to_char(const char *morse_code, MorseEntry *table) {
    if (!morse_code || !table) return '\0';
    
    for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
        if (strcmp(morse_code, table[i].morse_code) == 0) {
            return table[i].character;
        }
    }
    return '\0';
}

void print_menu(void) {
    printf("\n=== DECODIFICADOR UNIVERSAL ===\n");
    printf("1. Decodificar César\n");
    printf("2. Decodificar Hexadecimal\n");
    printf("3. Decodificar ASCII\n");
    printf("4. Decodificar Morse\n");
    printf("5. Salir\n");
    printf("Seleccione una opción: ");
}

void print_result(const char *method, const char *result) {
    printf("\n--- Resultado de %s ---\n", method);
    printf("Texto decodificado: %s\n", result);
    printf("------------------------\n");
}
