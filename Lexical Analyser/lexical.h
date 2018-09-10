#ifndef __LEXICAL__H
#define __LEXICAL__H

/*
 * GLOBAL TOKENS
 * 
 * As long as consistancy is preserved, all the tokens
 * are defined or can be redefined by the user.
 */
#ifndef LEX_TOKEN
#define LEX_TOKEN
#define PLUS            1
#define SUBTRACT        0
#define MULTIPLY        2
#define DIVIDE          0
#define GREATER_THAN    0
#define LESS_THAN       0
#define ASSIGN          0
#define LEFT_PAR        3
#define RIGHT_PAR       4
#define SEMI_COLON      6
#define IDENTIFIER      5
#define HEXIDECIMAL     5
#define DECIMAL         5
#define OCTAL           5

#define RESERVED_WORDS  5
#endif

#ifdef LEX_TOKEN
#define LEX_TOKEN
struct lex_token_t
{
	int p_cur;
	int value;
	char text[20];
};
#endif

/*
 * Lexical Analyser Initialization Function
 * 
 * Description:	Initialize the Lexical analyser.
 *
 * @param	file
 * @usage
 * {@code
 *		FILE *file = NULL;
 *		fopen_s(&file, "test.txt", "rb");
 *		lex_init(file);
 * }
 */
void	lex_init(FILE *);
/*
 * Lexical Analyser Parser Function
 *
 * Description:	Parses tokens from a stream of input. The user
 *				can also access the parsed string.
 *
 * @out	Returns a token number on success, -1 on fail 
 *		(unknown character),or 0 on EOF.
 * @usage
 * {@code
 *		while ((code = lex_parse()))
 *		{
 *			if (code != -1)
 *				printf("<%s, %d >\n", token.text, token.value);
 *		}
 * }
 */
int		lex_parse(void);
#endif
