%{
#include <iostream>
#include <fstream>
#include <string>

#include "Defines.h"
#include "Nodes.h"
#include "LookupTable.h"

extern "C"
{
    int yylex();
    int yyerror(const char *msg, ...);
}

LookupTable table;
%}

%union 
{
    char* text;
    int	  ival;
    float fval;
};

%type <text> wyr
%token <text> ID

%token <itype> TYPE_INT
%token <ftype> TYPE_FLOAT

%token <btype> TYPE_BOOL
%token <bool_true> BOOL_TRUE
%token <bool_false> BOOL_FALSE

%token <ival> LC
%token <fval> LR

%token <if> IF
%token <else> ELSE
%token <while> WHILE
%token <for> FOR
%token <do> DO

%token <compare> OP_COMPARE
%token <le> OP_LE
%token <ge> OP_GE
%token <and> OP_AND
%token <or> OP_OR

%left OP_COMPARE OP_LE OP_GE '<' '>'
%left OP_OR
%left OP_AND
%left '+' '-'
%left '*' '/'

%%
program
    : bloki                                                         {}
    ;
    
bloki
    : blok                                                          {}
    | bloki blok                                                    {}
    | sub_block                                                     {}
    ;
blok
    : declaration ';'                                               { table.PeekBlock()->Add( table.TopElement() ); }
    | assign ';'                                                    { table.PeekBlock()->Add( table.TopElement() ); }
    | if_stmt                                                       { table.PeekBlock()->Add( table.TopElement() ); }
    | while_stmt                                                    { table.PeekBlock()->Add( table.TopElement() ); }
    | for_stmt                                                      { table.PeekBlock()->Add( table.TopElement() ); }
    | sub_block                                                     { SHOW( "\n" ) table.PeekBlock()->Add( table.TopElement() ); }
    ;
if_stmt
    : IF '(' condition ')' sub_block                                { SHOW( "if" ) Node* body = table.TopElement(); Node *cond = table.TopElement(); table.PushElement( new IfBlock( body, cond ) ); }
    | IF '(' condition ')' sub_block ELSE sub_block                 { SHOW( "if - else" ) Node* body2 = table.TopElement(); Node *body = table.TopElement(); Node *cond = table.TopElement(); table.PushElement( new IfBlock( body, cond, body2 ) ); }
    ;
    
condition
    : wyr                                                           {}
    | wyr_log                                                       {}
    ;
    
comparison
    : wyr '<' wyr                                                   { SHOW( " < " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new Comparison( Comparison::Lt, lhs, rhs ) ); } 
    | wyr '>' wyr                                                   { SHOW( " > " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new Comparison( Comparison::Gt, lhs, rhs ) ); }
    | wyr OP_LE wyr                                                 { SHOW( " <= " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new Comparison( Comparison::Le, lhs, rhs ) ); }
    | wyr OP_GE wyr                                                 { SHOW( " >= " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new Comparison( Comparison::Ge, lhs, rhs ) ); } 
    | wyr OP_COMPARE wyr                                            { SHOW( " == " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new Comparison( Comparison::Eq, lhs, rhs ) ); }
    | wyr '!' '=' wyr                                               { SHOW( " != " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new Comparison( Comparison::Ne, lhs, rhs ) ); }
    ;

while_stmt
    : WHILE '(' condition ')' sub_block                             { SHOW( " while: " ) Node *body = table.TopElement(); Node *cond = table.TopElement(); table.PushElement( new WhileBlock( body, cond ) ); }
    | WHILE '(' condition ')' ';'                                   { SHOW( " while-nobody: " ) Node *body = new CodeBlock(); Node *cond = table.TopElement(); table.PushElement( new WhileBlock( body, cond ) ); }
    ;
    
for_stmt
    : FOR '(' for_init ';' for_cond ';' for_after ')' sub_block     { SHOW( " for: " ) Node *body = table.TopElement(); Node *after = table.TopElement(); Node *cond = table.TopElement(); Node *init = table.TopElement(); table.PushElement( new ForBlock( body, init, cond, after ) ); }
    | FOR '(' for_init ';' for_cond ';' for_after ')' ';'           { SHOW( " for-nobody: " ) Node *body = new CodeBlock(); Node *after = table.TopElement(); Node *cond = table.TopElement(); Node *init = table.TopElement(); table.PushElement( new ForBlock( body, init, cond, after ) ); }
    ;
    
for_init
    : assign                                                        { SHOW( " for-init " ) }
    | declaration                                                   { SHOW( " for-init " ) }
    ;
    
for_cond
    : condition                                                     { SHOW( " for-cond " ) }
    ;
    
for_after
    : assign                                                        { SHOW( " for-after " ) }
    ;

sub_block
    : sub_block_begin bloki sub_block_end                           { table.PushElement( table.TopBlock() ); }
    | sub_block_begin sub_block_end                                 { table.PushElement( table.TopBlock() ); }
    ;
    
sub_block_begin
    : '{'                                                           { SHOW( " block_start -> " ) table.PushBlock( new CodeBlock() ); }
    ;
    
sub_block_end
    : '}'                                                           { SHOW( " <- block_end " ) }
    ;

declaration
    : array_decl                                                    {}
    | TYPE_INT ident                                                { SHOW( " decl_int " ) table.PushElement( new Declaration( E_Int, static_cast< Variable* >( table.TopElement() ) ) ); }
    | TYPE_FLOAT ident                                              { SHOW( " decl_float " ) table.PushElement( new Declaration( E_Float, static_cast< Variable* >( table.TopElement() ) ) ); }
    | TYPE_BOOL ident                                               { SHOW( " decl_bool " ) table.PushElement( new Declaration( E_Bool, static_cast< Variable* >( table.TopElement() ) ) ); }
    ;

assign
    : ident '=' wyr                                                 { SHOW( "id = wyr" ) Node *rhs = table.TopElement(); Variable *lhs = static_cast< Variable* >( table.TopElement() ); table.PushElement( new Assignment( lhs, rhs ) ); }
    | array_ident '=' wyr                                           { SHOW( "arr = wyr" ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new Assignment( lhs, rhs )); }
    | ident '=' wyr_log                                             { SHOW( "id = log" ) Node *rhs = table.TopElement(); Variable *lhs = static_cast< Variable* >( table.TopElement() ); table.PushElement( new Assignment( lhs, rhs ) ); }
    | ident '=' array_ident                                         { SHOW( "id = arr" ) Node *rhs = table.TopElement(); Variable *lhs = static_cast< Variable* >( table.TopElement() ); table.PushElement( new Assignment( lhs, rhs ) ); }
    | array_ident '=' array_ident                                   { SHOW( "arr = arr" ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new Assignment( lhs, rhs ) ); }
    | declaration '=' wyr                                           { SHOW( "dec = wyr" ) Node *rhs = table.TopElement(); Variable *lhs = static_cast< Declaration* >( table.TopElement() )->GetVariable(); table.PushElement( new Assignment( lhs, rhs ) ); }
    | declaration '=' wyr_log                                       { SHOW( "dec = log" ) Node *rhs = table.TopElement(); Variable *lhs = static_cast< Declaration* >( table.TopElement() )->GetVariable(); table.PushElement( new Assignment( lhs, rhs ) ); }
    | declaration '=' array_ident                                   { SHOW( "dec = arr" ) Node *rhs = table.TopElement(); Variable *lhs = static_cast< Declaration* >( table.TopElement() )->GetVariable(); table.PushElement( new Assignment( lhs, rhs ) ); }
    ;
    
ident
    : ID                                                            { SHOW( " id( " << $1 << " )" ) table.PushElement( new Variable( $1 ) );}
    ;


wyr_log
    : wyr_log OP_OR wyr_log                                        { SHOW( " wyr_log " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new LogicOperation( LogicOperation::Or, lhs, rhs ) ); }
    | wyr_log OP_AND wyr_log                                       { SHOW( " wyr_log " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new LogicOperation( LogicOperation::And, lhs, rhs ) ); }
    | log_czynnik
    ;
    
log_czynnik
    : BOOL_TRUE                                                    { SHOW( " bool:true " ) table.PushElement( new BoolConstant( true ) ); }
    | BOOL_FALSE                                                   { SHOW( " bool:false " ) table.PushElement( new BoolConstant( false ) ); }
    | '(' wyr_log ')'                                              {}
    | ident                                                        {}
    | comparison                                                   {}
    ;
    

wyr
    : wyr '+' wyr                                                  { SHOW( " + " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new ArithmeticOperation( ArithmeticOperation::Add, lhs, rhs ) ); }
    | wyr '-' wyr                                                  { SHOW( " - " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new ArithmeticOperation( ArithmeticOperation::Sub, lhs, rhs ) ); }
    | wyr '*' wyr                                                  { SHOW( " * " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new ArithmeticOperation( ArithmeticOperation::Mul, lhs, rhs ) ); }
    | wyr '/' wyr                                                  { SHOW( " / " ) Node *rhs = table.TopElement(); Node *lhs = table.TopElement(); table.PushElement( new ArithmeticOperation( ArithmeticOperation::Div, lhs, rhs ) ); }
    | wyr_czynnik                                                  {}
    ;

wyr_czynnik
    : ident                                                        {} 
    | array_ident                                                  {}
    | LC                                                           { SHOW( "  int:" << $1 << " " ) table.PushElement( new Number( $1 ) ); }
    | LR                                                           { SHOW( "  float:" << $1 << " " ) table.PushElement( new Number( $1 ) ); }
    |'(' wyr ')'                                                   {}
    ;


// arrays
array_decl
    : TYPE_INT ident indexers_list_const                           { SHOW("array-decl-int") table.PushElement( new ArrayDeclaration( E_Int ) ); }
    | TYPE_FLOAT ident indexers_list_const                         { SHOW("array-decl-float") table.PushElement( new ArrayDeclaration( E_Float ) ); }
    ;

array_ident
    : ident indexers_list                                          { SHOW("array-id") table.PushElement( new ArrayVariable( E_None ) ); }
    ;

indexers_list_const
    :indexers_list_const indexer_const                             {}
    |indexer_const                                                 {}
    ;
indexer_const
    :'[' LC ']'                                                    { SHOW("indexer:" << $2 ) table.PushElement( new Indexer( $2 ) ); }
    ;
    
indexers_list
    :indexers_list indexer                                         {}
    |indexer                                                       {}
    ;
indexer
    :indexer_const                                                 {}
    |'[' ident ']'                                                 { SHOW("indexer-id") table.PushElement( new Indexer( table.TopElement() ) ); }
    ;
%%

int main(int argc, char *argv[])
{
    table.PushBlock( new CodeBlock() );
	yyparse();
	table.PushElement( table.TopBlock() );
	
	std::ofstream asmFile("app.asm.labels", std::ios::out);
	
	while(!table.GetElements().empty())
	{
		Node *e = table.TopElement();
		asmFile << e->Code();
	}
	asmFile.close();

	std::ifstream inAsmFile("app.asm.labels", std::ios::in);
	std::ofstream outAsmFile("app.asm", std::ios::out);
	
	std::map<std::string, int> labels;//label name, line number
	std::string line;
	int lineNo = 0;
	while(!inAsmFile.eof())
	{
		getline(inAsmFile,line);
		int colonPos = line.find(':');
		int labelStartPos = 0;
		while(colonPos != std::string::npos)//add to labels list
		{
			labels.insert(std::pair<std::string, int>(line.substr(labelStartPos, colonPos - labelStartPos), lineNo));
			labelStartPos = colonPos + 1;
			colonPos = line.find(':', labelStartPos);
		}
		lineNo++;
	}
	
	inAsmFile.clear();
	inAsmFile.seekg(0, inAsmFile.beg);
	while(true)
	{
		getline(inAsmFile,line);
		
		int colonPos = line.find(':');
		if(colonPos != std::string::npos)//add to labels list
		{
			line = line.substr(colonPos + 1);
		}
		
		for(std::map<std::string, int>::iterator it = labels.begin(); it != labels.end(); ++it)
		{
			int labelPos = line.find(it->first);
			if(labelPos != std::string::npos)
			{
				std::stringstream intStream;
				intStream << it->second;
				line.replace(labelPos, it->first.size(), intStream.str());
			}
		}
		
		if(!inAsmFile.eof())
			outAsmFile << line << std::endl;
		else
			break;
	}
	
	
	inAsmFile.close();
	outAsmFile.close();
    
    std::cout << "Build success!" << std::endl;

	return 0;
}

