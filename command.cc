
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


#include "command.h"

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append = 0;
	_errOUT = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}
	

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append = 0;
	_errOUT = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background     Append      \n" );
	printf( "  ------------ ------------ ------------ ------------  ----------      \n" );
	printf( "  %-12s %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO", _append?"YES":"NO");
	printf( "\n\n" );
	
}
void
Command::ChangeDir(char * dir)
{
	char s[100];
	printf("dir before change: %s\n",getcwd(s,100));
	
	int stat= chdir(dir);
	if(dir == NULL)	
		chdir(getenv("HOME"));
	else if(stat<0)
		perror( "MAZEN! cd error" );
	
		
		printf("dir after change: %s\n",getcwd(s,100));
}
void child_LOG(int signum)
{
	FILE *log=fopen("log.txt","a");
	time_t t = time(NULL);
	fprintf(log,"a child has been terminated at: %s \n",ctime(&t));
	fclose(log);
}


void
Command::execute()
{
	// Don't do anything if there are no simple commands
	
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}
	
	
	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec
	int defaultin = dup( 0 );
	int defaultout = dup( 1 );
	int defaulterr = dup( 2 );
	int outfd;int infd;int errfd;
	
	
	
	if (_inputFile) {
		    infd = open(_inputFile, O_RDONLY);
		    if (infd < 0) {
		        perror("MAZEN! create inputfile");
		        exit(2);
		    } else
		        dup2(infd, 0);

		} else
		    infd = dup(defaultin);
		    
		    
		if(_outFile){
				if(_append==1)
					{outfd = open( _outFile, O_CREAT | O_WRONLY | O_APPEND, 0666 );}
				else
					{outfd = open( _outFile, O_CREAT | O_WRONLY | O_TRUNC, 0666 );}
				if ( outfd < 0 ) {
					perror( "MAZEN! create outfile" );
					exit( 2 );
				}else
					dup2( outfd, 1 );
				if(_errOUT == 1)
					dup2( outfd, 2 );
			}else
				outfd = dup(defaultout);
		
			
		
			if(_errFile){
			if(_append==1)
				{errfd = open( _errFile, O_CREAT | O_WRONLY | O_APPEND, 0666 );}
				else
					{errfd = open( _errFile, O_CREAT | O_WRONLY | O_TRUNC, 0666 );}
				if ( errfd < 0 ) {
					perror( "MAZEN! create errfile" );
					exit( 2 );
					}else
						dup2( errfd, 2 );
			}else
				errfd = dup(defaulterr);
	
	
	
	
	// Print contents of Command data structure
	for (int i = 0; i < _numberOfSimpleCommands; i++) {
	
		if( strcmp(_simpleCommands[i]->_arguments[0],"exit")==0 )
			{
				printf("\tGoodBye Mazen\n");
				kill(getpid(),SIGQUIT);
				exit( 0 );	
			}
		
		
		printf("\n ENTERED LOOP  I = %d\n\n",i);
		
		int fdpipe[2];
		pipe(fdpipe);
		
		printf("\n INITIALISED PIPES \n\n");
		
		
		printf("\n!!!!!!!!!! before if i/o changers for command: %s !!!\n",_simpleCommands[i]->_arguments[0]);
		printf("\n!!!!!!!!!! i= %d ----	numberofcmds= %d !!!!!!!!!\n",i,_numberOfSimpleCommands);
		if(i==0)
		{
printf("\ni = 0 redirect input to inFile if needed using dup2 for command: %s!!\n",_simpleCommands[i]->_arguments[0]);
			dup2(infd, 0);
			close(infd);	
		}else if(i != 0)
		{
printf("\ni != 0 redirect input to pipe[0] using dup2 for command: %s!!!!!!!!!!\n",_simpleCommands[i]->_arguments[0]);
			//dup2(fdpipe[0],0);
			fdpipe[0]=infd;
			close(fdpipe[0]);
		}
		if(i ==_numberOfSimpleCommands-1)
		{
printf("\ni == nOFcmds redirect output to outfile using dup2 for command: %s!!!\n",_simpleCommands[i]->_arguments[0]);
			dup2(outfd,1);	
			close(outfd);
		}
		else if(i !=_numberOfSimpleCommands-1)
		{
printf("\ni != nOFcmds redirect output to pipe[1] using dup2 for command: %s!!!\n",_simpleCommands[i]->_arguments[0]);
			//dup2(fdpipe[1],1);
			fdpipe[1]=outfd;
			close(fdpipe[0]);
		}

	
		int pid = fork();
		if ( pid == -1 ) {
			perror("perror in fork");
			exit( 2 );
		}

		if (pid == 0) {
		
			
			
			if( strcmp(_simpleCommands[i]->_arguments[0],"cd")==0 )
			{
				printf("\nChange Directory\n");
				ChangeDir(_simpleCommands[i]->_arguments[1] );	
				printf("\n");
				continue;
			}
			
			print();
			
			
			//Child
			// close file descriptors that are not needed
			// You can use execvp() instead if the arguments are stored in an array
			
			
			printf("\n!!!!!!!!!! executing command %s !!!!!!!!!\n",_simpleCommands[i]->_arguments[0]);
			
			execvp(_simpleCommands[i]->_arguments[i], _simpleCommands[i]->_arguments);
			
			// exec() is not suppose to return, something went wrong
			fprintf(stderr, "error with %s: ", _simpleCommands[i]->_arguments[0]);
			perror("");
			exit( 2 );
	    
	    
	    
	    }else if(pid > 0)
	    {
	    	dup2( defaultin , 0 );
	    	dup2( defaultout , 1 );
	    	dup2( defaulterr , 2 );
	    }
	    
	    
	    
	    
	    if(_background == 0)
		{
			waitpid( pid, 0, 0 );
		}
	    
    signal (SIGCHLD, child_LOG);
    
    
}
	
	dup2( defaultin , 0 );
	dup2( defaultout, 1 );
	dup2( defaulterr, 2 );
	
	close( defaultin );
	close( defaultout );
	close( defaulterr );
	
	
	
	
	
	// Clear to prepare for next command
	clear();
	// Print new prompt
	prompt();	
}

// Shell implementation

void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

int 
main()
{
	signal(SIGINT, SIG_IGN);
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

