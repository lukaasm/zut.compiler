#include "Nodes.h"
#include "LookupTable.h"

#include <sstream>

extern LookupTable table;

const std::string EMPTY;

std::string CodeBlock::Code()
{
    std::stringstream result;
    
    for(NodeVector::iterator it = m_statements.begin(); it != m_statements.end(); ++it)
    {
        result << (*it)->Code();
    }
    
    return result.str();
}

std::string IfBlock::Code()
{
    table.PushLabel( table.GetNextLabel() );

    std::stringstream result;

    if( m_condition->Type() == E_Number || m_condition->Type() == E_Variable || m_condition->Type() == E_BoolConstant )
    {
        if( m_condition->Type() != E_BoolConstant && m_condition->NType() == E_Float )
        {
            FAILURE("Cannot use float as condition");
        }
        
        result << "SUB " << m_condition->Code()  << ",0" << std::endl;
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else if ( m_condition->Type() == E_ArrayVariable )
    {
        if ( m_condition->NType() == E_Float )
        {
            FAILURE("Cannot use float as condition");
        }
        
        result << m_condition->Code();
        
        m_condition = new PreparedCode("@R7", m_condition->NType() );
        result << "SUB " << m_condition->Code() << ",0" << std::endl;
            
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else if( m_condition->Type() == E_ArithmeticOperation )
    {
        result << m_condition->Code();
        
        result << "SUB " << table.TopElement()->Code() << ",0" << std::endl;
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else if( m_condition->Type() == E_Comparison )
    {
        result << m_condition->Code() << "label.e" << table.PeekLabel() << std::endl;
    }
    else if( m_condition->Type() == E_LogicOperation )
    {
        result << m_condition->Code();
        result << "SUB " << table.TopElement()->Code() << ",0" << std::endl;
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else
    {
       FAILURE( "Incorrect if condition");
    }

    result << "label.s" << table.PeekLabel() << ":" << m_body->Code();
    if( m_body2 )
    {
        result << "JMP label.end" << table.PeekLabel() << std::endl;
    }
    
    result << "label.e" << table.PeekLabel() << ":";

    if( m_body2 )
    {
        result << m_body2->Code() << "label.end" << table.PeekLabel() << ":";
    }

    table.PopLabel();

    return result.str();
}

std::string WhileBlock::Code()
{
    table.PushLabel( table.GetNextLabel() );

    std::stringstream result;

    result << "label.cond" << table.PeekLabel() << ":";

    if( m_condition->Type() == E_Number || m_condition->Type() == E_Variable || m_condition->Type() == E_BoolConstant )
    {
        if( m_condition->Type() != E_BoolConstant && m_condition->NType() == E_Float )
        {
            FAILURE("Cannot use float as condition");
        }
        
        result << "SUB " << m_condition->Code()  << ",0" << std::endl;
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else if ( m_condition->Type() == E_ArrayVariable )
    {
        if ( m_condition->NType() == E_Float )
        {
            FAILURE("Cannot use float as condition");
        }

        result << m_condition->Code();
        
        m_condition = new PreparedCode("@R7", m_condition->NType() );
        result << "SUB " << m_condition->Code() << ",0" << std::endl;
            
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else if( m_condition->Type() == E_ArithmeticOperation )
    {
        result << m_condition->Code();
        
        result << "SUB " << table.TopElement()->Code() << ",0" << std::endl;
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else if( m_condition->Type() == E_Comparison )
    {
        result << m_condition->Code() << "label.e" << table.PeekLabel() << std::endl;
    }
    else if( m_condition->Type() == E_LogicOperation )
    {
        result << m_condition->Code();
        result << "SUB " << table.TopElement()->Code() << ",0" << std::endl;
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else
    {
       FAILURE( "Incorrect while condition");
    }
    
    result << "label.s" << table.PeekLabel() << ":" << m_body->Code();
    result << "JMP label.cond" << table.PeekLabel() << std::endl;
    result << "label.e" << table.PeekLabel() << ":";

    table.PopLabel();

    return result.str();
}

std::string ForBlock::Code()
{
    table.PushLabel( table.GetNextLabel() );

    std::stringstream result;
    result << m_init->Code();

    result << "label.cond" << table.PeekLabel() << ":";

     if( m_condition->Type() == E_Number || m_condition->Type() == E_Variable || m_condition->Type() == E_BoolConstant )
    {
        if( m_condition->Type() != E_BoolConstant && m_condition->NType() == E_Float )
        {
            FAILURE("Cannot use float as condition");
        }
        
        result << "SUB " << m_condition->Code()  << ",0" << std::endl;
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else if ( m_condition->Type() == E_ArrayVariable )
    {
        if ( m_condition->NType() == E_Float )
        {
            FAILURE("Cannot use float as condition");
        }
        
        result << m_condition->Code();
        
        m_condition = new PreparedCode("@R7", m_condition->NType() );
        result << "SUB " << m_condition->Code() << ",0" << std::endl;
            
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else if( m_condition->Type() == E_ArithmeticOperation )
    {
        result << m_condition->Code();
        
        result << "SUB " << table.TopElement()->Code() << ",0" << std::endl;
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else if( m_condition->Type() == E_Comparison )
    {
        result << m_condition->Code() << "label.e" << table.PeekLabel() << std::endl;
    }
    else if( m_condition->Type() == E_LogicOperation )
    {
        result << m_condition->Code();
        result << "SUB " << table.TopElement()->Code() << ",0" << std::endl;
        result << "JZ label.e" << table.PeekLabel() << std::endl;
    }
    else
    {
       FAILURE( "Incorrect while condition");
    }

    result << "label.s" << table.PeekLabel() << ":" << m_body->Code();
    result << m_after->Code();
    result << "JMP label.cond" << table.PeekLabel() << std::endl;
    result << "label.e" << table.PeekLabel() << ":";

    table.PopLabel();

    return result.str();
}

std::string Comparison::Code()
{
    std::stringstream result;
    
    if( m_lhs->Type() == E_Number || m_lhs->Type() == E_Variable )
    {
    }
    else if ( m_lhs->Type() == E_ArrayVariable )
    {
        result << m_lhs->Code();
        
        if ( m_lhs->NType() == E_Float )
        {
            result << "FMOV F5,@F7" << std::endl;
            m_lhs = new PreparedCode("F5", m_lhs->NType() );
        }
        else
        {
            result << "MOV R5,@R7" << std::endl;
            m_lhs = new PreparedCode("R5", m_lhs->NType() );
        }
    }
    else if( m_lhs->Type() == E_ArithmeticOperation )
    {
        result << m_lhs->Code();
        m_lhs = table.TopElement();
    }
    else
    {
        std::cerr << "incorrect node type\n";
    }

    if( m_rhs->Type() == E_Number || m_rhs->Type() == E_Variable)
    {
    }
    else if ( m_rhs->Type() == E_ArrayVariable )
    {
        result << m_rhs->Code();
    
        if ( m_rhs->NType() == E_Float )
        {
            result << "FMOV F6,@F7" << std::endl;
            m_rhs = new PreparedCode("F6", m_rhs->NType() );
        }
        else
        {
            result << "MOV R6,@R7" << std::endl;
            m_rhs = new PreparedCode("R6", m_rhs->NType() );
        }
    }
    else if ( m_rhs->Type() == E_ArithmeticOperation )
    {
        result << m_rhs->Code();
        m_rhs = table.TopElement();
    }
    else
    {
        std::cerr << "Incorrect node type\n";
    }
    
    NumberType lType = m_lhs->NType();
    if( lType != m_rhs->NType() )
    {
        if( lType == E_Float || m_rhs->NType() == E_Float )
        {
            lType = E_Float;
            WARNING("Conversion from int to float");
        }
        else
        {
            FAILURE( "Type mismatch in comparison");
        }
    }
    
    switch( lType )
    {
        case E_Int:
            result << "MOV R1," << m_lhs->Code() << std::endl
                   << "MOV R2," << m_rhs->Code() << std::endl
                   << "SUB R1,R2" << std::endl
                   << OperatorToAsmName() << ' ';
            break;
        case E_Float:
            result << "FMOV F1," << m_lhs->Code() << std::endl
                   << "FMOV F2," << m_rhs->Code() << std::endl
                   << "FSUB F1,F2" << std::endl
                   << OperatorToAsmName() << ' ';
            break;
        default:
            FAILURE( "None type in comparison ");
    }
    
    return result.str();
}

Declaration::Declaration( NumberType type, Variable *var )
    : Node( E_Declaration ), m_numberType( type ), m_name( var->Name() ), m_variable( var )
{
    table.AddSymbol( m_numberType, m_name, table.GetNextAddress() );
}
 
std::string Declaration::Code()
{
    return EMPTY;
}

std::string Variable::Code()
{
    std::stringstream result;
    
    const Symbol & symbol = table.GetSymbol( m_name );
    
    result << "#" << symbol.Offset;
    m_numberType = symbol.Type;
    
    return result.str();
}

std::string BoolConstant::Code()
{
    std::stringstream result;
    result << ( m_value ? 1 : 0 );
    return result.str();
}

std::string Number::Code()
{
    std::stringstream result;
    switch( m_numberType )
    {
        case E_Int:
            result << m_value.ival;
            break;
        case E_Float:
            result << m_value.fval;
            break;
    }

    return result.str();
}

std::string Assignment::Code()
{
    std::stringstream result;
    if ( m_lhs->Type() != E_Variable )
    {
        if( m_lhs->Type() == E_ArrayVariable )
        {           
            result << m_lhs->Code();
            
            if ( m_lhs->NType() == E_Float )
            {
                result << "MOV F6,F7" << std::endl;
                m_lhs = new PreparedCode( "@F6", m_lhs->NType() );
            }
            else
            {
                result << "MOV R6,R7" << std::endl;
                m_lhs = new PreparedCode( "@R6", m_lhs->NType() );
            }
        }
        else
        {
            FAILURE( "Assignment left value must be variable" );
        }
    }
    
    if ( m_rhs->Type() == E_BoolConstant || m_rhs->Type() == E_LogicOperation )
    {
        if( m_lhs->NType() != E_Bool )
        {
            FAILURE( "Tried to assign bool to non-bool variable" );
        }
        
        if( m_rhs->Type() == E_BoolConstant )
        {
            result << "MOV " + m_lhs->Code() + "," + m_rhs->Code() << std::endl;
            return result.str();
        }
            
        result << m_rhs->Code();
        
        Node* n = table.TopElement();
        
        result << "MOV" << ' ' + m_lhs->Code() + "," + n->Code() << std::endl;
        
        return result.str();
    }

    NumberType lType = m_lhs->NType();
    NumberType rType = m_rhs->NType();
    if( lType != rType )
    {
        if( lType == E_None )
        {
            FAILURE( "Not a number (Type: None)" );
        }
        
        if( lType == E_Float && rType == E_Int )
        {
            WARNING( "int to float conversion" );
            lType = E_Float;
        }
        else if( lType == E_Int && rType == E_Float )
        {
            WARNING( "float to int conversion" );
            lType = E_Float;
        }
        else
        {
            FAILURE( "Type mismatch in assignment, conversion is not allowed" );
        }
    }
    
    std::string movAsmCmd;
    switch(lType)
    {
        case E_Int:
            movAsmCmd = "MOV";
            break;
        case E_Float:
            movAsmCmd = "FMOV";
            break;
        case E_Bool:
            movAsmCmd = "MOV";
            break;
        default:
            FAILURE( "None Type in assignment" );
    }
    
    if( m_rhs->Type() == E_Number || m_rhs->Type() == E_Variable )
    {
        result << movAsmCmd + ' ' + m_lhs->Code() + "," + m_rhs->Code() << std::endl;
        return result.str();
    }
    else if( m_rhs->Type() == E_ArithmeticOperation)
    {
        result << m_rhs->Code();
        
        Node* n = table.TopElement();
        
        result << movAsmCmd << ' ' + m_lhs->Code() + "," + n->Code() << std::endl;
        return result.str();
    }
    else if( m_rhs->Type() == E_ArrayVariable)
    {
        result << m_rhs->Code();
        if ( m_rhs->NType() == E_Int )
            result << movAsmCmd << ' ' + m_lhs->Code() + "," + "@R7" << std::endl;
        else
            result << movAsmCmd << ' ' + m_lhs->Code() + "," + "@F7" << std::endl;
        return result.str();
    }
    else
    {
        FAILURE( "Incorrect right side of assignment" );
    }
}

std::string Indexer::Code()
{
    if ( m_value == -1 )
    {
        return m_node->Code();
    }
            
    std::stringstream result;
    result << m_value;
    return result.str();
}

std::string ArrayVariable::Code()
{
    std::stringstream result;
    
    const Symbol & symbol = table.GetSymbol( m_name );
    if( symbol.Size.empty() )
    {
        FAILURE("Indexer used on non-array variable: " << m_name);
    }
    
    if ( NType() == E_Int )
        result << "MOV R7," << symbol.Offset << std::endl;
    else
        result << "FMOV F7," << symbol.Offset << std::endl;
        
    std::vector< int > size = symbol.Size;
    
    int dim = size.size();
    
    for ( int i=0; i < dim; i++ )
    {
        if ( NType() == E_Int )
            result << "MOV R" << i << "," << m_indexes[ i ]->Code() << std::endl;
        else
            result << "FMOV F" << i << "," << m_indexes[ i ]->Code() << std::endl;
    }
        
    for ( int i = 1; i < dim; i++ )
    {
        int s = size[ i - 1 ];
        int j = i - 1 ;
        while ( --j >= 0 )
        {
            s *= size[ j ];
        }
        
        if ( NType() == E_Int )
            result << "MUL R" << i << "," << s << std::endl;
        else
            result << "FMUL F" << i << "," << s << std::endl;
    }
    
    for ( int i = 0; i < dim; i++ )
    {
        if ( NType() == E_Int )
            result << "ADD R7,R" << i << std::endl;
        else
            result << "FADD F7,F" << i << std::endl;
    }
    
    return result.str();
}

ArrayDeclaration::ArrayDeclaration( NumberType type )
    : Node( E_ArrayDeclaration ), m_numberType(type)
{
    while ( true )
    {
        Node *indexerOrName = table.TopElement();
        
        if( indexerOrName->Type() == E_Variable )
        {
            m_name = static_cast< Variable* >( indexerOrName )->Name();
            break;
        }
        
        else if ( indexerOrName->Type() == E_Indexer )
        {
            m_size.push_back( static_cast< Indexer* >( indexerOrName )->Get() );
        }
        else
        {
            FAILURE( "Expected indexer or variable name for array" );
        }
    }

    int mem = 1;
    for( int i = 0; i < m_size.size(); i++ )
        mem *= m_size.at( i );
        
    table.AddSymbol( m_numberType, m_name, table.GetNextAddress( mem ), m_size );
}

std::string ArrayDeclaration::Code()
{
    return EMPTY;
}

std::string ArithmeticOperation::Code()
{
    std::stringstream result;
  
    if( m_lhs->Type() == E_Number || m_lhs->Type() == E_Variable )
    {
    }
    else if( m_lhs->Type() == E_ArithmeticOperation )
    {
        result << m_lhs->Code();
        m_lhs = table.TopElement();
    }
    else
    {
        WARNING("Incorrect node type");
    }
    
    //expand rvalue
    if( m_rhs->Type() == E_Number || m_rhs->Type() == E_Variable )
    {
    }
    else if( m_rhs->Type() == E_ArithmeticOperation )
    {
        result << m_rhs->Code();
        m_rhs = table.TopElement();
    }
    else
    {
        WARNING("Incorrect node type");
    }
    
    AddTempSymbol();
    
    switch(NType())
    {
        case E_Int:
            result << "MOV R1," << m_lhs->Code() << std::endl;
            result << "MOV R2," << m_rhs->Code() << std::endl;
            result << OperatorToAsmName() << " R1,R2" << std::endl;
            result << "MOV #" << table.GetSymbol( table.GetCurrentTemp() ).Offset << ",R1" << std::endl;;
            break;
        case E_Float:
            result << "FMOV F1," << m_lhs->Code() << std::endl;
            result << "FMOV F2," << m_rhs->Code() << std::endl;
            result << 'F' << OperatorToAsmName() << " F1,F2" << std::endl;
            result << "FMOV #" << table.GetSymbol( table.GetCurrentTemp() ).Offset << ",F1" << std::endl;;
            break;
        default:
            FAILURE( "None as type in arithmetic operation" );
    }
    
    return result.str();
}

std::string LogicOperation::CodeAsOR()
{
    table.PushLabel(table.GetNextLabel());
			
    std::stringstream result;
    
    result << "orStart" << table.PeekLabel() << ":";
    
    //condition 1:
    switch(m_lhs->Type())
    {
        case  E_BoolConstant:
            result << "ADD 0," << m_lhs->Code() << std::endl;
            result << "JZ " << "orSecondCond" << table.PeekLabel() << std::endl;
            break;
        case  E_Comparison:
            result << m_lhs->Code() << "orSecondCond" << table.PeekLabel() << std::endl;
            break;
        case  E_LogicOperation:
            result << m_lhs->Code();
            result << "ADD " << table.TopElement()->Code() << ",0" << std::endl;
            result << "JZ " << "orSecondCond" << table.PeekLabel() << std::endl;
            break;
        case  E_Variable:
            switch(m_lhs->NType())
            {
                case E_Bool:
                case E_Int:
                    result << "SUB " << m_lhs->Code()  << ",0" << std::endl;
                    result << "JZ " << "orSecondCond" << table.PeekLabel() << std::endl;
                    break;
                default:
                    FAILURE("Wrong variable type in logic operation");
            }
            break;
        default:
            FAILURE( "Wrong node type in left side of logic operation:" << m_lhs->Type() ); 
    }
    result << "JMP " << "orTrue" << table.PeekLabel() << std::endl;
    result << "orSecondCond" << table.PeekLabel() << ":";
    
    //condition 2:
    switch(m_rhs->Type())
    {
        case E_BoolConstant:
            result << "ADD 0," << m_rhs->Code() << std::endl;
            result << "JZ " << "orFalse" << table.PeekLabel() << std::endl;
            break;
        case  E_Comparison:
            result << m_rhs->Code() << "orFalse" << table.PeekLabel() << std::endl;
            break;
        case  E_LogicOperation:
            result << m_rhs->Code();
            result << "ADD " << table.TopElement()->Code() << ",0" << std::endl;
            result << "JZ " << "orFalse" << table.PeekLabel() << std::endl;
            break;
        case  E_Variable:
            switch(m_rhs->NType())
            {
                case E_Bool:
                case E_Int:
                    result << "SUB " << m_rhs->Code()  << ",0" << std::endl;
                    result << "JZ " << "orFalse" << table.PeekLabel() << std::endl;
                    break;
                default:
                    FAILURE( "Wrong variable type in logic operation" );
            }
            break;
        default:
            FAILURE( "Wrong node type in right side of logic operation:" << m_rhs->Type() );
    }
    
    //result:
    AddTempSymbol();
    
    //true:
    result << "orTrue" << table.PeekLabel() << ":";
    result << "MOV #" << table.GetSymbol( table.GetCurrentTemp() ).Offset << ",1" << std::endl;
    result << "JMP " << "orEnd" << table.PeekLabel() << std::endl;
    
    //false:
    result << "orFalse" << table.PeekLabel() << ":";
    result << "MOV #" << table.GetSymbol( table.GetCurrentTemp() ).Offset << ",0" << std::endl;
    
    result << "orEnd" << table.PeekLabel() << ":";
    
    table.PopLabel();
    
    return result.str();
}

std::string LogicOperation::CodeAsAND()
{
    table.PushLabel( table.GetNextLabel() );

    std::stringstream result;

    result << "andStart" << table.PeekLabel() << ":";

    //condition 1:
    switch(m_lhs->Type())
    {
        case E_BoolConstant:
            result << "ADD 0," << m_lhs->Code() << std::endl;
            result << "JZ " << "andFalse" << table.PeekLabel() << std::endl;
            break;
        case E_Comparison:
            result << m_lhs->Code() << "andFalse" << table.PeekLabel() << std::endl;
            break;
        case E_LogicOperation:
            result << m_lhs->Code();
            result << "ADD " << table.TopElement()->Code() << ",0" << std::endl;
            result << "JZ " << "andFalse" << table.PeekLabel() << std::endl;
            break;
        case E_Variable:
            switch(m_lhs->NType())
            {
                case E_Bool:
                case E_Int:
                    result << "SUB " << m_lhs->Code()  << ",0" << std::endl;
                    result << "JZ " << "andFalse" << table.PeekLabel() << std::endl;
                    break;
                default:
                    FAILURE("wrong variable type in logic operation");
            }
            break;
        default:
            FAILURE( "Wrong node type in left side of logic operation:" << m_lhs->Type());
    }

    //condition 2:
    switch(m_rhs->Type())
    {
        case E_BoolConstant:
            result << "ADD 0," << m_rhs->Code() << std::endl
                << "JZ " << "andFalse" << table.PeekLabel() << std::endl;
            break;
        case E_Comparison:
            result << m_rhs->Code() << "andFalse" << table.PeekLabel() << std::endl;
            break;
        case E_LogicOperation:
            result << m_rhs->Code();
            result << "ADD " << table.TopElement()->Code() << ",0" << std::endl;
            result << "JZ " << "andFalse" << table.PeekLabel() << std::endl;
            break;
        case E_Variable:
            switch(m_rhs->NType())
            {
                case E_Bool:
                case E_Int:
                    result << "SUB " << m_rhs->Code()  << ",0" << std::endl;
                    result << "JZ " << "andFalse" << table.PeekLabel() << std::endl;
                    break;
                default:
                    FAILURE( "Wrong variable type in logic operation" );
            }
            break;
        default:
            FAILURE( "Wrong node type in right side of logic operation:" << m_rhs->Type() );
    }

    //result:
    AddTempSymbol();

    //true:
    result << "andTrue" << table.PeekLabel() << ":";
    result << "MOV #" << table.GetSymbol( table.GetCurrentTemp() ).Offset << ",1" << std::endl;
    result << "JMP " << "andEnd" << table.PeekLabel() << std::endl;

    //false:
    result << "andFalse" << table.PeekLabel() << ":";
    result << "MOV #" << table.GetSymbol( table.GetCurrentTemp() ).Offset << ",0" << std::endl;
    result << "andEnd" << table.PeekLabel() << ":";

    table.PopLabel();

    return result.str();
}
