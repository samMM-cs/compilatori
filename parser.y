%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "symb_tab.h"
  
  extern int yylex(void);
  extern int yylineno;
  void yyerror(const char *s);
%}

%union {
  char id[4];
  char *string;
  double num;
}

%token <id> CITY_ID TOUR_ID
%token <string> CITY_NAME WORD
%token <num> NUM
%token SECTION_SEP
%token TOUR_SEP
%token NEWLINE

%type <string> tour_name
%start file
%%

file: city_section SECTION_SEP NEWLINE tour_section SECTION_SEP NEWLINE stops_section { printResults(); };

city_section: city_section city_line | city_line;

city_line: CITY_ID CITY_NAME '(' NUM ',' NUM ')' NEWLINE {
  insertCity($1, $2, $4, $6);
  free($2);
};

tour_section: tour_section TOUR_SEP NEWLINE tour_block | tour_block;

tour_block: TOUR_ID NEWLINE tour_name NEWLINE CITY_ID NEWLINE {
  insertTour($1, $3, $5);
  free($3);
};

tour_name: WORD { $$ = $1; } | tour_name WORD {
  char *tmp = malloc(strlen($1) + strlen($2) + 2);
  sprintf(tmp, "%s %s", $1, $2);
  free($1);
  free($2);
  $$ = tmp;
};

stops_section: stops_section stop_line | stop_line;

stop_line: TOUR_ID CITY_ID optional_newline {
  addStop($1, $2);
};

optional_newline: NEWLINE | %empty ;

%%

void yyerror(const char *s) {
  fprintf(stderr, "Syntax Error on line %d: %s\n", yylineno, s);
}

int main(int argc, char **argv) {
  extern FILE *yyin;
  if (argc > 1) {
    FILE *f = fopen(argv[1], "r");
    if (!f) {
      perror(argv[1]);
      exit(EXIT_FAILURE);
    }
    yyin = f;
  }

  if (yyparse())
    exit(EXIT_FAILURE);
} 