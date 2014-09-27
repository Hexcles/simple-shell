%{
const int _numargs = 255;
char *_args[255];
int _argcount = 0;
%}

WORD	[a-zA-Z0-9\/\.\+\[_-]+
SPECIAL	[()><|&;*]

%%
	_argcount = 0; 
	_args[0] = NULL; 

{WORD}|{SPECIAL} {  
	  if(_argcount < _numargs-1) {
	    _args[_argcount++] = strdup(yytext);
	    _args[_argcount] = NULL;
	  }
	}

[ \t]+

.

%%

char **parse_line(const char *line) {
	YY_BUFFER_STATE buffer = yy_scan_string(line);
	yylex();
	yy_delete_buffer(buffer);
	return _args;
}
