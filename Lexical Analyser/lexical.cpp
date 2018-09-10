#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "lexical.h"

/* 
 * Buffer States
 * 
 * There are for state:
 * 1. Rest State	-	It means there's no input file.
 * 2. Running State	-	It means there's an input file, 
 *						and the program is currently parsing.						
 * 3. Stop State	-	It means the program has completed parsing.
 * 4. Ready State	-	It means there's an input file, 
 *						and the program can start parsing.
 */
#ifndef LEX_BUFFER_STATES
#define LEX_BUFFER_STATES
#define LEX_REST_STATE      0
#define LEX_RUNNING_STATE   1
#define LEX_STOP_STATE      2
#define LEX_READY_STATE     3
#endif

#ifndef LEX_BUFFER_STRUCT
#define LEX_BUFFER_STRUCT
struct lex_buffer_t
{
	/* Read a file from disk and keep
	 * later for to act a buffer during
	 * parsing process.
	*/
	FILE *lex_input_file;

	/* Keeps track of the current read position
	 * in the input file. This is necessary to 
	 * for error reporting. If there's and error
	 * at line number (n), ithe probelm would know
	 * exactly the error position.
	 */
	int lex_current_pos;

	 /* Keeps track from when an input file is open
	  * until it is closed or reach EOF.
	  */
	int lex_buffer_state;
};
#endif

#ifdef LEX_TOKEN
struct lex_token_t token;
const char *reserved_word[] = { "if", "then", "else", "while", "do" };
#endif

#ifndef STATE_DIAGRAM
#define STATE_DIAGRAM
#endif

#define LEX_FATAL_ERROR(s,a)	{   \
									fprintf(stderr, s, a); \
									return NULL;    \
								}
#define IS_DIGIT(a)				(a >= '0' && a <= '9')
#define IS_WHOLE_OCT_DIGIT(a)	(a >= '0' && a <= '7')
#define IS_NATURAL_OCT_DIGIT(a)	(a >= '1' && a <= '7')
#define IS_NATURAL_DIGIT(a)		(a >= '1' && a <= '9')
#define IS_HEX_LETTER(a)		(a >= 'a' && a <= 'f' || a >= 'A' && a <= 'F')
#define IS_LETTER(a)			(a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z')

typedef int LEX_BUFFER_STATE;
typedef struct lex_buffer_t* LEX_BUFFER;

LEX_BUFFER lex_buffer;
static char c;

/* 
 * Create buffer Function
 * 
 * Description:	Create, and initialize the buffer structure.
 *
 * @param	file
 * @out		lex_buffer_t*
 */
LEX_BUFFER	lex_create_buffer(FILE* file);
/*
 * Next Character Function
 *
 * Description:	Fetches the next character from input file.
 *
 * @param	buffer structure.
 */
void		lex_next_ch(LEX_BUFFER lex_buffer);
/*
 * Initialize buffer Function
 *
 * Description:	Create, and initialize the buffer structure.
 *
 * @param	buffer strucure
 * @param	buffer state
 * @param	file
 */
void	lex_init_buffer(LEX_BUFFER lex_buffer, LEX_BUFFER_STATE buffer_state, FILE *file);

LEX_BUFFER lex_create_buffer(FILE* file)
{
    struct lex_buffer_t *lex_buffer = (LEX_BUFFER)malloc(sizeof(struct lex_buffer_t));
    if (lex_buffer == NULL)
        LEX_FATAL_ERROR("malloc memory allocation failed.", NULL);
    lex_init_buffer(lex_buffer, LEX_READY_STATE, file);
    return lex_buffer;
}

void lex_init_buffer(LEX_BUFFER lex_buffer, LEX_BUFFER_STATE buffer_state,FILE *file)
{
    lex_buffer->lex_input_file = file;
	lex_buffer->lex_current_pos = 0;
    lex_buffer->lex_buffer_state= buffer_state;
}

void lex_next_ch(LEX_BUFFER lex_buffer)
{
	if (!((c = fgetc(lex_buffer->lex_input_file)) != EOF))
	{
		lex_buffer->lex_buffer_state = LEX_STOP_STATE;
	}
	lex_buffer->lex_current_pos++;
}

void lex_init(FILE *file)
{
	lex_buffer = lex_create_buffer(file);
}

void lex_destroy()
{
	fclose(lex_buffer->lex_input_file);
	free(lex_buffer);
}

#ifdef LEX_TOKEN
void lex_concat_token()
{
	if (token.p_cur < 20)
		token.text[token.p_cur++] = c;
	lex_next_ch(lex_buffer);
}
void lex_reset_token()
{
	memset(token.text, 0, 20);
	token.p_cur = 0;
	token.value = 0;
}
#endif

// Problems
// 1. Do we need tables to store actual data?
// 2. What information should we return for the token?
// 3. Is it the responsibility of the user(programmer)
// to organize returned information?
// 4. If no, then the programmer is compelled to complete these tasks.

int lex_parse(void)
{
	if (lex_buffer->lex_buffer_state == LEX_STOP_STATE)
	{
		lex_destroy();
		return 0;
	}
	
	if (lex_buffer->lex_buffer_state != LEX_RUNNING_STATE)
	{
		lex_buffer->lex_buffer_state = LEX_RUNNING_STATE;
		lex_next_ch(lex_buffer);
	}
	lex_reset_token();
	// This is where it all gets down :D 
#ifdef STATE_DIAGRAM
	if (IS_LETTER(c))
	{
		while (IS_LETTER(c) || IS_DIGIT(c))
		{
			lex_concat_token();
		}
		for (int j = 0; j < RESERVED_WORDS; ++j)
		{
			if (strcmp(token.text, reserved_word[j]) == 0)
				return 15;
		}
		return IDENTIFIER;
	}
	else if (c == '0') 
	{ 
		lex_next_ch(lex_buffer);
		if (c == 'x') 
		{ 
			lex_next_ch(lex_buffer);
			while (IS_HEX_LETTER(c) || IS_DIGIT(c))
			{
				lex_concat_token();
			}
			return HEXIDECIMAL;
		}
		else if (IS_DIGIT(c))
		{
			if (IS_NATURAL_OCT_DIGIT(c))
			{
				lex_concat_token();
				while (IS_WHOLE_OCT_DIGIT(c))
				{
					lex_concat_token();
				}
			}
			return OCTAL;
		}
		return DECIMAL;
	}
	else if (IS_NATURAL_DIGIT(c))
	{
		while (IS_DIGIT(c))
		{
			lex_concat_token();
		}
		return DECIMAL;
	}
	else if (c == '+') { lex_concat_token(); return PLUS;			}
	else if (c == '-') { lex_concat_token(); return SUBTRACT;		}
	else if (c == '*') { lex_concat_token(); return MULTIPLY;		}
	else if (c == '/') { lex_concat_token(); return DIVIDE;			}
	else if (c == '>') { lex_concat_token(); return GREATER_THAN;	}
	else if (c == '<') { lex_concat_token(); return LESS_THAN;		}
	else if (c == '=') { lex_concat_token(); return ASSIGN;			}
	else if (c == '(') { lex_concat_token(); return LEFT_PAR;		}
	else if (c == ')') { lex_concat_token(); return RIGHT_PAR;		}
	else if (c == ';') { lex_concat_token(); return SEMI_COLON;		}
	else
	{
		switch (c)
		{
		case ' ' :
		case '\r':
		case '\n':
		case '\t': lex_concat_token(); return -1;
		default  : break;
		}
	}
#endif
	LEX_FATAL_ERROR("[lex parse error]: unknown token :%d", lex_buffer->lex_current_pos)
	return 0;
}
