
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token 	NOTOKEN GREAT NEWLINE GREATGREAT SMALL AND PIPE ANDGREAT ANDGREATGREAT

%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
%}

%%

goal:	
	commands
	;

commands: 
	commands command
	| command
	;

command: 
	simple_command
	|
    ;

simple_command:	
	pipeCMD ALLIOMODIFIERS NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| NEWLINE
	| error NEWLINE { yyerrok; }
	;
//command for pipe
pipeCMD:
	pipeCMD PIPE command_and_args
	| command_and_args
	;

ALLIOMODIFIERS:
	iomodifier_opt
	| background_opt
	| ALLIOMODIFIERS background_opt
	| background_opt iomodifier_opt
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
           printf("   Yacc: insert argument \"%s\"\n", $1);


	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;


command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);

	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

background_opt:
	 AND	{
		printf ("	Yacc: insert background operation\n");
		Command::_currentCommand._background = 1;
	}
	;
	
iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	| GREATGREAT WORD {
		printf("   Yacc: insert output and set append \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._append = 1;
	}
	| SMALL WORD {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	}
	| GREAT WORD SMALL WORD	{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		printf("   Yacc: insert input \"%s\"\n", $4);
		Command::_currentCommand._inputFile = $4;
	}
	| GREATGREAT WORD SMALL WORD	{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		printf("   Yacc: insert input \"%s\"\n", $4);
		Command::_currentCommand._inputFile = $4;
	}
	| SMALL WORD GREAT WORD	{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
		printf("   Yacc: insert input \"%s\"\n", $4);
		Command::_currentCommand._outFile = $4;
	}
	| SMALL WORD GREATGREAT WORD	{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
		printf("   Yacc: insert input \"%s\"\n", $4);
		Command::_currentCommand._outFile = $4;
	}
	| ANDGREAT WORD {
		printf("   Yacc: insert output and write error in \"%s\"\n", $2);
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand._errOUT = 1;
	}
	| ANDGREATGREAT WORD {
		printf("   Yacc: insert output, set append and write error in \"%s\"\n", $2);
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand._append = 1;
		Command::_currentCommand._errOUT = 1;
	}
	| /* empty */
	;
	
	
	
	
%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
