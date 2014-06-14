#if !defined DEFINES_H_
#define DEFINES_H_

#include <iostream>

#define FAILURE( a ) { std::cerr << "[Error] " << a << std::endl; exit( 1 ); }
#define WARNING( a ) { std::cout << "[Warning] " << a << std::endl; }

#define DEBUG

#ifdef DEBUG
    #define SHOW( a ) { std::cout << a << std::endl; }
#else
    #define SHOW( a )
#endif

enum NodeType
{
    E_Block                   = 1,
    E_IfBlock                 = 2,
    E_Comparison              = 3,
    E_WhileBlock              = 4,
    E_ForBlock                = 5,
    E_Variable                = 6,
    E_Declaration             = 7,
    E_Assignment              = 8,
    E_BoolConstant            = 9,
    E_LogicOperation          = 10,
    E_ArithmeticOperation     = 11,
    E_Number                  = 12,
    E_ComplexCondition        = 13,
    E_ArrayDeclaration        = 14,
    E_Indexer                 = 15,
    E_ArrayVariable           = 16,
    E_PreparedCode            = 17
};

enum NumberType
{
	E_None,
	E_Int,
	E_Float,
	E_Bool
};

#endif /* DEFINES_H_ */
