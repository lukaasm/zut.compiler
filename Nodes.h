#if !defined AST_NODE_H_
#define AST_NODE_H_

#include <vector>
#include <stack>

#include "Defines.h"
#include "LookupTable.h"

class Node
{
    public:
		Node( NodeType type ) : m_type( type ) {}
        
		virtual std::string     Code() = 0;
		NodeType                Type() { return m_type; }
		
		virtual NumberType      NType() const
		{
			FAILURE("Not a number node type: " << (int)m_type );
            return E_None;
		}
        
	protected:
		NodeType m_type;
};

typedef std::vector< Node* > NodeVector;

class CodeBlock : public Node
{
	public:
		CodeBlock() : Node( E_Block ) {}
		
		void            Add( Node* node )	{ m_statements.push_back( node ); }
		int             Size() const	    { return m_statements.size(); }
        
		std::string     Code();

	private:
		NodeVector m_statements;
		
};

class IfBlock : public Node
{
	public:
		IfBlock( Node* body, Node* condition, Node * body2 = NULL ) : Node( E_IfBlock ), m_body( body ), m_condition( condition ), m_body2( body2 ) { }
		
        NumberType      NType() const { return Node::NType(); }
        
		std::string Code();
		
	private:
		Node* m_body;
		Node* m_condition;
		Node* m_body2;
};

class WhileBlock : public Node
{
	public:
		WhileBlock( Node* body, Node* condition ) : Node(E_WhileBlock), m_body( body ), m_condition( condition ) {}
		
		std::string Code();
		
	private:
		Node * m_body;
		Node * m_condition;
};

class ForBlock : public Node
{
	public:
		ForBlock( Node* body, Node* init, Node* condition, Node* after ) : Node( E_ForBlock ), m_body( body ), m_init( init ), m_after( after ), m_condition( condition ) {}
		
		std::string Code();
		
	private:
		Node* m_body;
		Node* m_condition;
        Node* m_init;
        Node* m_after;
};

class Comparison : public Node
{
	public:
    	enum Operator
		{
			none,
			Lt,
			Gt,
			Le,
			Ge,
			Eq,
			Ne
		};
        
		Comparison( Operator op, Node* lhs, Node* rhs ): Node( E_Comparison ), m_operator( op ), m_lhs( lhs ), m_rhs( rhs)	{}
		
		std::string Code();
        
	private:
    
        std::string OperatorToAsmName()
        {
			switch(m_operator)
			{
                case Lt: return "JNL";
                case Gt: return "JNG";
                case Le: return "JG";
                case Ge: return "JL";
                case Eq: return "JNZ";
                case Ne: return "JZ";
                default:
                    FAILURE("unknown comparison operator");
			}
        }
        
		Operator    m_operator;
		Node*       m_lhs;
		Node*       m_rhs;
};    

class Variable : public Node
{
	public:
		Variable( const std::string & name, NumberType type = E_None ) : Node( E_Variable ), m_name( name ), m_numberType( type ) { }
        
        std::string Code();
        std::string Name() { return m_name; }
        
        NumberType NType() const
		{
			if ( m_numberType == E_None )
			{
				if( table.HasSymbol(m_name))
				{
					return table.GetSymbol(m_name).Type;
				}
				
                FAILURE("None as type detected while getting variable type (undeclared variable): " << m_name );
			}
            
			return m_numberType;
		}
                
    private:
        std::string m_name;
        NumberType  m_numberType;
};

class Declaration : public Node
{
	public:
        Declaration( NumberType type, Variable *var );
        
        std::string Code();
        
        NumberType NType() const { return m_numberType; }
        Variable*  GetVariable() { return m_variable; }
        
    private:
        NumberType      m_numberType;
        std::string     m_name;
        Variable*       m_variable;
};

class Assignment : public Node
{
    public:
        Assignment( Node* lhs, Node* rhs ) : Node( E_Assignment ), m_lhs( lhs ), m_rhs( rhs ){}
        
        std::string Code();
        
    private:
		Node* m_lhs;
		Node* m_rhs;
};

class LogicOperation : public Node
{
	public:
		enum Operator
		{
			None = 0,
			And  = 1,
			Or   = 2
		};
		
		LogicOperation( Operator op, Node * lhs, Node * rhs) : Node( E_LogicOperation ), m_lhs( lhs ), m_rhs( rhs ), m_operator( op ) {}
        
        std::string Code()
        {
			switch(m_operator)
			{
				case And:
					return CodeAsAND();
				case Or:
					return CodeAsOR();
				default:
					FAILURE( "Wrong logical operation" );
			}
            
            return std::string();
        }
        
        virtual NumberType      NType() const
		{
            return E_Bool;
		}

    private:
        std::string CodeAsOR();
        std::string CodeAsAND();
        
        void AddTempSymbol()
		{
			NumberType type = NType();
			table.AddSymbol( type, table.GetNextTemp(), table.GetNextAddress() );
			table.PushElement( new Variable( table.GetCurrentTemp(), type ) );
		}
        
		Node*       m_lhs;
		Node*       m_rhs;
        Operator    m_operator;
};

class BoolConstant : public Node
{
	public:
		BoolConstant( bool value ) : Node( E_BoolConstant ), m_value( value ) {}
		
		std::string Code();
		
	private:
		bool m_value;
};

class ArithmeticOperation : public Node
{
	public:
		enum Operator
		{
			None = 0,
			Add  = 1,
			Sub  = 2,
			Mul  = 3,
			Div  = 4
		};
		
		ArithmeticOperation( Operator op, Node * lhs, Node * rhs ) : Node( E_ArithmeticOperation ), m_lhs( lhs ), m_rhs( rhs ), m_operator( op ) {}
        
        virtual NumberType NType() const
		{
			NumberType left = m_lhs->NType();
			NumberType right = m_rhs->NType();
			if( left == right )
			{
				if( left == E_None )
				{
					FAILURE( "None type detected while checking type in arithmetic operation" );
				}
                
				return left;
			}
			else if( left == E_Float && right == E_Int)
			{
				WARNING( "Coversion from int to float" );
                
				return E_Float;
			}
            else if ( right == E_Float && left == E_Int )
            {
                WARNING( "Coversion from float to int" );
                
				return E_Int;
            }
			else
			{
				FAILURE( "Type mismatch in operation, conversion not allowed" );
			}
		}

        std::string Code();
                
    private:
        std::string OperatorToAsmName()
        {
			switch(m_operator)
			{
                case Add:
                    return "ADD";
                case Sub:
                    return "SUB";
                case Mul:
                    return "MUL";
                case Div:
                    return "DIV";
                default:
                    FAILURE("Unknown arithmetic operator");
			}
        }
        
        void AddTempSymbol()
		{
			NumberType type = NType();
			table.AddSymbol( type, table.GetNextTemp(), table.GetNextAddress() );
			table.PushElement( new Variable( table.GetCurrentTemp(), type ) );
		}
        
        Operator m_operator;
        Node*    m_lhs;
        Node*    m_rhs;
};

class Number : public Node
{
	public:
		Number( double value ) : Node( E_Number ), m_numberType( E_Float ) { m_value.fval = value; }
		Number( float value ) : Node( E_Number ), m_numberType( E_Float ) { m_value.fval = value;	}
		Number( int value ) : Node( E_Number ), m_numberType( E_Int ) { m_value.ival = value;	}
        
        std::string Code();
                
        virtual NumberType      NType() const { return m_numberType; }
        
    private:
        union
        {
            int     ival;
            float   fval;
        } m_value;
        
        NumberType m_numberType;
};

class Indexer: public Node
{
	public:
		Indexer( int value ) : Node( E_Indexer ), m_value( value ) { }
		Indexer( Node *node ) : Node(E_Indexer), m_value( -1 ), m_node( node ) { }
        
		int Get() const
		{
			if( m_value < 0 )
			{
				FAILURE( "Index cannot be negative" );
			}
            
			return m_value;
		}
		
		std::string Code();
        
	private:
		int     m_value;
		Node *  m_node;
		
};
    
class ArrayDeclaration : public Node
{
	public:
		ArrayDeclaration( NumberType type );
		
		std::string Code();

	private:
		std::string         m_name;
		NumberType          m_numberType;
		std::vector<int>    m_size;
};
   
class ArrayVariable : public Node
{
	public:
		ArrayVariable( NumberType type ) : Node( E_ArrayVariable ), m_numberType(type), m_name("")
		{
			while(true)
			{
				Node *indexerOrName = table.TopElement();
                
				if( indexerOrName->Type() == E_Variable)
				{
					m_name = static_cast< Variable* >( indexerOrName )->Name();
					break;
				}
				else if ( indexerOrName->Type() == E_Indexer )
				{
					m_indexes.push_back( static_cast< Indexer* >( indexerOrName ) );
				}
				else
				{
					FAILURE( "Expected indexer or variable name for array" );
				}
			}
		}
		
		std::string Code();

		NumberType NType() const
		{
			if ( m_numberType == E_None)
			{
				if ( table.HasSymbol( m_name ) )
				{
					return table.GetSymbol( m_name ).Type;
				}
                
				FAILURE( "None as type detected while getting variable type (undeclared variable)" );
			}
            
			return m_numberType;
		}
		
		std::string Name() const
		{
			return m_name;
		}
        
	private:
		std::string             m_name;
		NumberType              m_numberType;
		std::vector< Indexer* > m_indexes;
};
    
class PreparedCode : public Node
{
	public:
		PreparedCode( const std::string & code, NumberType numType = E_None) : Node(E_PreparedCode), m_code(code), m_numerType(numType)
		{}
		
		std::string Code()
		{
			return m_code;
		}
		
		NumberType NType() const
		{
			return m_numerType;
		}
		
	private:
		std::string m_code;
		NumberType  m_numerType;
	};
    
#endif /* AST_NODE_H_ */
