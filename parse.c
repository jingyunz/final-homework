/*
 * (C) 2014 HuangJinDong
 *
 * src/parse.c
 *
 * The parser implementation for the ddsh.
 *
 */

#include "globals.h"
#include "exec.h"
#include "scan.h"
#include "parse.h"
#include <string.h>
#include <stdlib.h>

/* fill the arg[][] with 0 */
#define CLRARG                                \
for (int i = 0; i <= MAXTOKENLEN; i++)        \
	memset(arg + i, 0, MAXTOKENLEN + 1);

/* globals variables */
char  commands[MAXTOKENLEN + 1];
extern char  tokenString[MAXTOKENLEN + 1];
extern char  arg[MAXTOKENLEN + 1][MAXTOKENLEN + 1];

static TokenType token;              /* holds current token */
static char number[MAXTOKENLEN + 1]; /*  holds number in []*/

/* prototypes */
static void stmt_sequence();
static void statement();
static void if_stmt();
static void while_stmt();
static void do_command();

/* get all token when syntax error happened */
static void clearToken()
{
	while (token != NEWLINE)
		token = getToken();
}

static void match(TokenType expected)
{
	if (token == expected)
		token = getToken();
	else
		fprintf(stderr, "unexpected token: %s\n", tokenString);
}

void stmt_sequence()
{
//	printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
	statement();
	while (token != ENDFILE){
		if (token == SEMI)
			match(SEMI);
		statement();
	}
}

void statement()
{
	switch(token)
	{
	case IF: 
		//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
		if_stmt();
	       	break;
	case WHILE: 
		//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
		while_stmt();
	        break;
	case COMMAND: 
		//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
		while(token != SEMI){
			token = getToken();
			if (token == NEWLINE)
				break;
		}
		//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
		do_command();
		break;
	case SEMI:
		//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
		match(SEMI);
		break;
	case NEWLINE:
		//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
		match(NEWLINE);
		break;
	default: 
		fprintf(stderr, "unexpected token %s\n", tokenString);
		break;
	}
}

void if_stmt()
{
	/*  if [ num ]; then dosometing; fi */
	match(IF);
	if (token != LPAREN){
		fprintf(stderr, "missing '[' behinds if\n");
		clearToken();
		return;
	}
	match(LPAREN);
	
	if (token != NUM){
		fprintf(stderr, "missing number in '[]'\n");
		clearToken();
		return;
	}
	else{
		match(NUM);
		strcpy(number, tokenString);
	}

	if (token != RPAREN){
		fprintf(stderr, "missing ']'\n");
		clearToken();
		return;
	}	
	match(RPAREN);

	if (token != SEMI){
		fprintf(stderr, "missing ';' behinds ']'\n");
		clearToken();
		return;
	}
	match(SEMI);

	if (token != THEN){
		fprintf(stderr, "missing 'then'\n");
		clearToken();
		return;
	}
	match(THEN);

	while(token != SEMI){
		token = getToken();
		if (token == NEWLINE)
			break;
	}

	if (token != SEMI){
		fprintf(stderr, "missing ';' behinds commands\n");
		clearToken();
		return;
	}
	match(SEMI);

	if (token != FI){
		fprintf(stderr, "missing 'fi' at the end\n");
		clearToken();
		return;
	}
	match(FI);

	if (atoi(number))
		do_command();
}

void while_stmt()
{
	/* while [ num ]; do dosomething; done */
	match(WHILE);
	if (token != LPAREN){
		fprintf(stderr, "missing '[' behinds while\n");
		clearToken();
		return;
	}
	match(LPAREN);

	if (token != NUM){
		fprintf(stderr, "missing number in '[]'\n");
		clearToken();
		return;
	}
	match(NUM);
	strcpy(number, tokenString);
	
	if (token != RPAREN){
		fprintf(stderr, "missing ']'\n");
		clearToken();
		return;
	}
	match(RPAREN);

	if (token != SEMI){
		fprintf(stderr, "missing ';' behinds ']'\n");
		clearToken();
		return;
	}
	match(SEMI);

	if (token != DO){
		fprintf(stderr, "missing 'do' behinds commands\n");
		clearToken();
		return;
	}
	match(DO);

	while(token != SEMI){
		token = getToken();
		if (token == NEWLINE)
			break;
	}

	if (token != SEMI){
		fprintf(stderr, "missing ';' behinds 'do'\n");
		clearToken();
		return;
	}
	match(SEMI);

	if (token != DONEWHILE){
		fprintf(stderr, "missing 'done' at the end\n");
		clearToken();
		return;
	}
	match(DONEWHILE);

	while(atoi(number))
		forktoexec();
}

void do_command()
{
	//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
	forktoexec();
	//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
	CLRARG;
}

void parse()
{
	token = getToken();
	//printf("%s in %s, at line %d\n", __FUNCTION__, __FILE__, __LINE__);
	stmt_sequence();
}

