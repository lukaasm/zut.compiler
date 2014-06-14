#if !defined LOOKUPTABLE_H_
#define LOOKUPTABLE_H_

#include <map>
#include <vector>
#include <sstream>

#include "Defines.h"
#include "Nodes.h"

class Node;
class CodeBlock;

class Symbol
{
    public:
        Symbol( NumberType type, int offset ) : Type( type ), Offset( offset ) { }
        Symbol( NumberType type, int offset, const std::vector<int> & size ) : Type( type ), Offset( offset ), Size( size ) { }
        
        int                 Offset;
        NumberType          Type;
        std::vector<int>    Size;
};

class LookupTable
{
    public:
        LookupTable(){}
        
        void AddSymbol( NumberType type, const std::string & name, int address )
        {
            std::pair< std::map< std::string, Symbol >::iterator, bool > result = m_symbols.insert( std::make_pair( name, Symbol( type, address ) ) );
            
            if ( !result.second )
            {
                FAILURE( "Symbol redefinition: " << name );
            }
        }
        
        void AddSymbol( NumberType type, const std::string & name, int address, const std::vector<int> & size )
        {
            std::pair< std::map< std::string, Symbol >::iterator, bool > result = m_symbols.insert( std::make_pair( name, Symbol( type, address, size ) ) );
            
            if ( !result.second )
            {
                FAILURE( "Symbol redefinition: " << name );
            }
        }
        
        Symbol GetSymbol( const std::string & name )
        {
            std::map< std::string, Symbol >::const_iterator it = m_symbols.find( name );
            if( it == m_symbols.end())
            {
                FAILURE( "Undefined symbol: " << name );
            }
            
            return it->second;
        }
        
        bool HasSymbol( const std::string & name ) const
        {
            return m_symbols.find( name ) != m_symbols.end();
        }
        
        int GetCurrentAddress() { return m_currentAddress; }
        int GetNextAddress( int size = 1 )
        {
            static int temp = -1;

            ++temp;
            m_currentAddress = temp;
            temp += size - 1;
            return m_currentAddress;
        }
        
        int GetCurrentLabel() { return m_currentLabel; }
        int GetNextLabel( int size = 1 )
        {
            static int temp = -1;

            ++temp;
            m_currentLabel = temp;
            temp += size - 1;
            return m_currentLabel;
        }
        
        void PushLabel( int idx )
        {
            m_labels.push( idx );
        }
        
        int PeekLabel()
        {
            return m_labels.top();
        }
        
        void PopLabel()
        {
            m_labels.pop();
        }
        
        std::string GetCurrentTemp()
        {
            std::stringstream stream;
            stream << "____TEMP____" << m_currentTemp;
            return stream.str();
        }
        
        std::string GetNextTemp()
        {
            std::stringstream stream;
            stream << "____TEMP____" << ++m_currentTemp;
            return stream.str();
        }
        
        Node* TopElement()
        {
            Node* node = m_elements.top();
            m_elements.pop();
            return node;
        }
        
        void PushElement( Node* node )
        {
            m_elements.push( node );
        }
        
        std::stack< Node* > & GetElements() { return m_elements; }
        
        void PushBlock( CodeBlock* block ) { m_blocks.push( block ); }
        
        CodeBlock* PeekBlock() { return m_blocks.top(); }
        CodeBlock* TopBlock() 
        {
            CodeBlock* block = m_blocks.top();
            m_blocks.pop();
            return block;
        }
        
        std::stack< CodeBlock* > & GetBlocks() { return m_blocks; }
    
    private:
        int m_currentAddress;
        int m_currentLabel;
        int m_currentTemp;
        
        std::map< std::string, Symbol > m_symbols;
        
        std::stack< Node* >         m_elements;
        std::stack< CodeBlock* >    m_blocks;
        std::stack<int>             m_labels;
};

extern LookupTable table;

#endif
