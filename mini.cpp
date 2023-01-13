#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#define OPERATOR "OPERATOR"
#define NEW_LINE "NEW_LINE"
#define INDENT "INDENT"
#define EQUALS "EQUAL"
#define EQUAL_EQUAL "EQUAL_EQUAL"
#define COLON "COLON"
#define COMMA "COMMA"
#define SPACE "SPACE"
#define ASSIGMENT "ASSIGMENT"
#define OPERATION "OPERATION"
#define INT "INT"
#define STRING "STRING"
#define BOOLEAN "BOOLEAN"
#define NULL_ID "NULL"
#define LIST "LIST"
#define LIST_INDEX "LIST_INDEX"
#define IDENTIFIER "IDENTIFIER"
#define FUNCTION_CALL "FUNCTION_CALL"
#define EXPRESSION "EXPRESSION"
#define BODY "BODY"
#define FUNCTION "FUNCTION"
#define PARAMETER "PARAMETER"
#define RETURN "RETURN"
#define ERROR "ERROR"
#define ADD "ADD"
#define LEFT_PAREN "LEFT_PAREN"
#define RIGHT_PAREN "RIGHT_PAREN"
#define LEFT_BRACK "LEFT_BRACK"
#define RIGHT_BRACK "RIGHT_BRACK"
#define TRUE "TRUE"
#define FALSE "FALSE"
#define IDENTIFIER "IDENTIFIER"
#define STRING "STRING"
#define PRINT "PRINT"
#define IF "IF"
#define ELSE "ELSE"
#define FUNCTION_DEFINITION "FUNCTION_DEFINITION"
#define RETURN "RETURN"
#define NEW_CHUNK "NEW_CHUNK"
#define COMMENT "COMMENT"

class token 
{
public:
    std::string type;
    std::string value;
};

int fun_if_else = 0; 

class Node
{
public:
    int line;
    std::string type;
    std::string value;
    std::string symbol;
    std::vector<Node> children;
    std::vector<Node> parameters;
    std::vector<Node> expression;
};

class Symbol
{
public:
    std::string type;
    int integer;
    std::string str;
    bool boolean;
    std::vector<int> list;
    Node function;
    int line;
};

std::unordered_map<std::string, Symbol> symTable;
std::vector<std::vector<token>> lexer(std::string);
Node runParser(std::vector<std::vector<token>> tokens);
Symbol evaulate(Node node);
Node evaluate(std::vector<std::vector<token>> tokens, int line, int t_idx, token currToken) ;


int main(int argc, char* argv[]) 
{
    //load contents of python file into std::string contents
    std::string input_file_name = argv[1];
    std::ifstream input(input_file_name);
    std::string contents;
    contents.assign( (std::istreambuf_iterator<char>(input) ),(std::istreambuf_iterator<char>()    ) );
    
    std::vector<std::vector<token>> tokens = lexer(contents);

    Node* root = new Node;
    Node* ast = new Node;
    Symbol* sym = new Symbol;

    *root = runParser(tokens);

    for (int i=0; i < root->children.size(); i++) 
    {
        evaulate(root->children[i]);
    }
    return 0;
}


Node runParser(std::vector<std::vector<token>> tokens) 
{
    Node* root = new Node;
    int line = 0; 
    int t_idx = 0; 
    token currToken; 

    while (line < tokens.size()) 
    {
        currToken = tokens[line][t_idx]; 
        Node* node = new Node;
        *node = evaluate(tokens, line, t_idx, currToken); 
        root->children.push_back(*node); 
        if(fun_if_else == 0)
        {
            line++;
        }
        else
        {
            line = fun_if_else + line;
        }

        fun_if_else = 0; 
    }

    return *root; 
}

Node evaluate(std::vector<std::vector<token>> tokens, int line, int t_idx, token currToken) 
{
    Node* node = new Node;
    if (currToken.type == IDENTIFIER) 
    {

        node->type = IDENTIFIER; 
        node->symbol = currToken.value;

        if (tokens[line].size() == 1 || t_idx == tokens[line].size() - 1 || tokens[line][t_idx + 1].type == NEW_CHUNK)
        {
            return *node;
        }
        
        currToken = tokens[line][t_idx+1];

        if (currToken.type == EQUALS) 
        {

            node->type = ASSIGMENT; 
            Node expression = evaluate(tokens, line, t_idx+2, tokens[line][t_idx+2]); 

            node->expression.push_back(expression); 
            return *node; 
        }


        if (currToken.type == ADD) 
        {

            Node* operation = new Node;
            operation->type = OPERATION; 
            operation->value = "+"; 

            operation->parameters.push_back(*node); 
            operation->parameters.push_back(evaluate(tokens, line, t_idx+2, tokens[line][t_idx+2])); 

            return *operation;
        }

        if (currToken.type == LEFT_PAREN) 
        {

            Node* func_call = new Node;
            func_call->type = FUNCTION_CALL; 
            func_call->symbol = node->symbol; 
            int tokenIncrement = 2;
            currToken = tokens[line][t_idx + 2]; 
            while (currToken.type != RIGHT_PAREN) 
            {
                //Ignore commas
                if (currToken.type == COMMA) 
                {
                    tokenIncrement++;
                    currToken = tokens[line][t_idx+tokenIncrement];
                    continue;
                }
                Node expression = evaluate(tokens, line, t_idx+tokenIncrement, currToken); //parse parameter
                func_call->parameters.push_back(expression); 
                tokenIncrement ++; 
                currToken = tokens[line][t_idx+tokenIncrement]; 
            }
            return *func_call;
        }

        
        if (currToken.type == RIGHT_PAREN) 
        {
            return *node;
        }

        if (currToken.type == LEFT_BRACK) 
        {
            Node* list_idx = new Node;
            list_idx->type = LIST_INDEX; 
            list_idx->symbol = node->symbol; 
            list_idx->parameters.push_back(evaluate(tokens, line, t_idx+2, tokens[line][t_idx+2])); 
            int tokenIncrement = 2; 
            while (currToken.type != RIGHT_BRACK) 
            { 
                if (tokens[line].size() > t_idx + tokenIncrement) 
                {
                    currToken = tokens[line][t_idx+tokenIncrement];
                    tokenIncrement++;
                } 
                else 
                {
                    std::cout << "no closing bracket\n";
                }
            }

            //Check if there are more tokens after the close bracket
            if (tokens[line].size() > t_idx+tokenIncrement) 
            {
                currToken = tokens[line][t_idx+tokenIncrement];
            } else 
            {
                return *list_idx;
            }
            
            if (currToken.type == EQUALS) 
            {
                node->type = ASSIGMENT;
                currToken = tokens[line][t_idx+tokenIncrement+1];
                list_idx->expression.push_back(evaluate(tokens, line, t_idx+tokenIncrement+1, currToken));
                node->expression.push_back(*list_idx);
                return *node;
            }

            if (currToken.type == ADD) 
            { 
                Node* operation = new Node;
                operation->type = OPERATION;
                operation->value = currToken.value;

                operation->parameters.push_back(*list_idx);
                operation->parameters.push_back(evaluate(tokens, line, t_idx+tokenIncrement+1, tokens[line][t_idx+tokenIncrement+1]));

                return *operation;
            }

            return *list_idx;
        }

        if (currToken.type == OPERATOR) 
        {

            Node* operation = new Node;
            operation->type = OPERATION;
            operation->value = currToken.value;

            operation->parameters.push_back(*node);
            operation->parameters.push_back(evaluate(tokens, line, t_idx+2, tokens[line][t_idx+2]));

            return *operation;
        }

        if (currToken.type == LEFT_PAREN) 
        {

            Node* func_call = new Node;
            func_call->type = FUNCTION_CALL;
            func_call->symbol = node->symbol;

            int tokenIncrement = 1;
            while (t_idx+2+tokenIncrement < tokens[line].size()) 
            {
                currToken = tokens[line][t_idx+2+tokenIncrement];
                if (currToken.type == RIGHT_PAREN) 
                {
                    break;
                }
                Node push_b = evaluate(tokens, line, t_idx+tokenIncrement+2, currToken);
                func_call->parameters.push_back(push_b);
            }

            return *func_call;
        }

        return *node;
    }

    if (currToken.type == LEFT_PAREN) 
    {

        Node* expression = new Node;
        expression->type = EXPRESSION;

        int tokenIncrement = 1;
        while (currToken.type != RIGHT_PAREN) 
        {
            currToken = tokens[line][t_idx+tokenIncrement];

            if (currToken.type == COMMA) 
            {
                tokenIncrement++;
                currToken = tokens[line][t_idx+tokenIncrement];
            }

            if (currToken.type == RIGHT_PAREN) 
            {
                break;
            }

            Node node = evaluate(tokens, line, t_idx+tokenIncrement, currToken);
            expression->expression.push_back(node);
            tokenIncrement++;

            for (int i = t_idx+tokenIncrement; i < tokens[line].size(); i++) 
            {
                if (tokens[line][i].type == COMMA || tokens[line][i].type == RIGHT_PAREN) {
                    break;
                }
                tokenIncrement ++;
            }
            
        }

        return *expression;
    }

    if (currToken.type == LEFT_BRACK) 
    {
        Node* list = new Node;
        list->type = LIST;
        int tokenIncrement = 1;

        while (currToken.type != RIGHT_BRACK) 
        {
            currToken = tokens[line][t_idx+tokenIncrement];

            if (currToken.type == COMMA) 
            {
                tokenIncrement++;
                currToken = tokens[line][t_idx+tokenIncrement];
            }

            if (currToken.type == RIGHT_BRACK)
            {
                break;
            } 

            Node push_b = evaluate(tokens, line, t_idx+tokenIncrement, currToken);
            list->expression.push_back(push_b);
            tokenIncrement++;
        }


        tokenIncrement ++;
        if (t_idx + tokenIncrement < tokens[line].size()) 
        {
            currToken = tokens[line][t_idx+tokenIncrement];
        } 
        else 
        {
            return *list;
        }

        if (currToken.type == ADD && tokens[line][t_idx+tokenIncrement+1].type != NEW_CHUNK) 
        {
            Node* op = new Node;
            op->type = OPERATION;
            op->value = currToken.value;

            op->parameters.push_back(*list);
            op->parameters.push_back(evaluate(tokens, line, t_idx+tokenIncrement+1, tokens[line][t_idx+tokenIncrement+1]));

            return *op;
        }
    }

    if (currToken.type == INT) 
    {

        node->type = INT;
        node->value = currToken.value;

        if (tokens[line].size() == 1 || t_idx == tokens[line].size() - 1)
        {
            return *node;
        }

        currToken = tokens[line][t_idx+1];

        if (currToken.type == ADD) 
        {

            Node* op = new Node;
            op->type = OPERATION;
            op->value = "+";

            op->parameters.push_back(*node);
            Node push_b = evaluate(tokens, line, t_idx+2, tokens[line][t_idx+2]);
            op->parameters.push_back(push_b);

            return *op;
        }

        return *node;
    }

    if (currToken.type == STRING) 
    {
        node->type = STRING;
        node->value = currToken.value;
        return *node;
    }

    if (currToken.type == TRUE || currToken.type == FALSE) 
    {
        node->type = BOOLEAN;
        node->value = currToken.value;
        return *node;
    }

    if (currToken.type == PRINT) 
    {

        Node* print = new Node;
        print->type = FUNCTION_CALL;
        print->symbol = currToken.value;

        Node push_b = evaluate(tokens, line, t_idx+1, tokens[line][t_idx+1]);
        print->expression.push_back(push_b);
        return *print;
    }

    
    if (currToken.type == IF) 
    {

        Node* if_block = new Node;
        if_block->type = FUNCTION_CALL;
        if_block->symbol = currToken.value;

        if_block->parameters.push_back(evaluate(tokens, line, t_idx+1, tokens[line][t_idx+1]));

        int tokenIncrement = 2;
        for (int i = t_idx+tokenIncrement; i<tokens[line].size(); i++) 
        {
            currToken = tokens[line][i];
            if (currToken.type == COLON) 
            {
                break;
            } 
        }
        
        line ++;
        currToken = tokens[line][t_idx];
        while (currToken.type != NEW_CHUNK) 
        {
            fun_if_else ++;
            Node push_b = evaluate(tokens, line, t_idx, currToken);
            if_block->children.push_back(push_b);
            token lastElement = tokens[line][tokens[line].size()-1];
            line += 1 + node->children.size();
            if (lastElement.type == NEW_CHUNK) 
            {
                break;
            }
            currToken = tokens[line][t_idx];
        }
        
        currToken = tokens[line][t_idx];
        if (currToken.type == ELSE) 
        {
            Node* else_block = new Node;
            else_block->type = FUNCTION_CALL;
            else_block->symbol = currToken.value;

            line ++;
            fun_if_else ++;
            currToken = tokens[line][t_idx];
            while (currToken.type != NEW_CHUNK) 
            {
                fun_if_else ++;
                if(line > tokens.size()) break;
                Node node = evaluate(tokens, line, t_idx, currToken);
                else_block->children.push_back(node);
                token lastElement = tokens[line][tokens[line].size()-1];
                line += 1 + node.children.size();
                if (lastElement.type == NEW_CHUNK) break;
                if (tokens.size() > line) 
                {
                    currToken = tokens[line][t_idx];
                }
            }

            if_block->children.push_back(*else_block);
        }
        
        return *if_block;
    }

    if (currToken.type == FUNCTION_DEFINITION) 
    {

        Node* function = new Node;
        function->type = FUNCTION;
        int tokenIncrement = 1;
        currToken = tokens[line][t_idx+tokenIncrement];
        function->symbol = currToken.value;
        tokenIncrement ++;
        currToken = tokens[line][t_idx+tokenIncrement];
        while (currToken.type != COLON) 
        {
            if (currToken.type == IDENTIFIER) 
            {
                Node* param = new Node;
                param->type = PARAMETER;
                param->symbol = currToken.value;
                function->parameters.push_back(*param);
            } 

            tokenIncrement ++;
            currToken = tokens[line][t_idx+tokenIncrement];
        }
        
        line ++;
        fun_if_else ++;
        currToken = tokens[line][t_idx];
        while (currToken.type != NEW_CHUNK) 
        {
            fun_if_else ++;
            Node funcStatement = evaluate(tokens, line, t_idx, currToken);
            function->children.push_back(funcStatement);
            token lastElement = tokens[line][tokens[line].size()-1];
            line += 1 + funcStatement.children.size();
            if (lastElement.type == NEW_CHUNK) break;
            if (tokens.size() > line) 
            {
                currToken = tokens[line][t_idx];
            }
        }
        return *function;
    }

    if (currToken.type == NEW_CHUNK) 
    {
        if (tokens[line].size() > 1) 
        {
            return evaluate(tokens, line, t_idx+1, tokens[line][t_idx+1]);
        } 
        else 
        {
            return Node();
        }
    }

    if (currToken.type == RETURN) 
    {

        Node* returnNode = new Node;
        returnNode->type = RETURN;

        currToken = tokens[line][t_idx+1];
        Node push_b = evaluate(tokens, line, t_idx+1, currToken);
        returnNode->expression.push_back(push_b);
        return *returnNode;
    }
    
    Node* null_node = new Node;
    node->type = NULL_ID;
    node->value = "null";
    return *null_node;
}



Symbol evaulate(Node node) 
{
    if (node.type == ASSIGMENT) 
    {

        Symbol symbol = Symbol();
        std::string symbolName = node.symbol;
        symbol.line = node.line;


        symbol = evaulate(node.expression[0]);
        symTable[symbolName] = symbol;

        return symbol;
    }

    if (node.type == FUNCTION) 
    {
        Symbol function = Symbol();
        std::string functionName = node.symbol;
        function.function = node;
        function.line = node.line;
        function.type = "function";
        symTable[functionName] = function;
    }

    if (node.type == FUNCTION_CALL) 
    {
        if (node.symbol == "print") 
        {
            for (int i=0; i<node.expression[0].expression.size(); i++) 
            {
                Symbol toPrint = evaulate(node.expression[0].expression[i]);
                if (toPrint.type == INT)
                {
                    int intPrint = toPrint.integer;
                    std::cout << intPrint << " ";
                }
                if (toPrint.type == STRING)
                { 
                    std::string strPrint = toPrint.str;
                    std::cout << strPrint << " ";
                }
                if (toPrint.type == LIST) 
                {
                    std::cout << "[";
                    for (int i = 0; i < toPrint.list.size() -1 ; i++) 
                    {
                        std::cout << toPrint.list[i] << ", ";
                    }
                    int last = toPrint.list.size()-1;
                    std::cout << toPrint.list[last];
                    std::cout << "] ";
                }
            }

            std::cout << std::endl;
        }

        if (node.symbol == "if") 
        {
            Symbol condition = evaulate(node.parameters[0]);
            
            if (condition.boolean) 
            {
                for (int i = 0; i < node.children.size(); i++) 
                {
                    evaulate(node.children[i]);
                }
            } 
            else if (node.expression.size() > 0) 
            {
                evaulate(node.expression[0]);
            }
        }

        if (node.symbol == "else") 
        {
            for (int i = 0; i < node.children.size(); i++)
            {
                evaulate(node.children[i]);
            }
        }

        if (symTable.find(node.symbol) != symTable.end()) 
        {
            Node function = symTable[node.symbol].function;

            for (int i = 0; i < function.parameters.size(); i++) 
            {
                std::string parameterName = function.parameters[i].symbol;
                Symbol parameterValue = evaulate(node.parameters[i]);
                symTable[parameterName] = parameterValue;
            }

            for (int i = 0; i < function.children.size(); i++) 
            {
                if (function.children[i].type == RETURN) 
                {
                    return evaulate(function.children[i]);
                }
                evaulate(function.children[i]);
            }
            return Symbol();
        }
    }

    if (node.type == OPERATION) 
    {
        Symbol term1 = evaulate(node.parameters[0]);
        Symbol term2 = evaulate(node.parameters[1]);
        Symbol result = Symbol();
        result.line = node.line;

        if (node.value == "+") 
        {
            if (term1.type == LIST && term2.type == LIST) 
            {
                Symbol res = Symbol();
                res.type = LIST;
                res.line = node.line;
                std::vector<int> list;
                for (int i = 0; i < term1.list.size(); i++) 
                {
                    list.push_back(term1.list[i]);
                }
                for (int i = 0; i < term2.list.size(); i++) 
                {
                    list.push_back(term2.list[i]);
                }
                res.list = list;
                return res;
            }

            if (term1.type == STRING && term2.type == STRING) 
            {
                Symbol res = Symbol();
                res.type = STRING;
                res.line = node.line;
                std::string str = "";
                for (int i = 0; i < term1.str.size(); i++) 
                {
                    str += term1.str[i];
                }
                for (int i = 0; i < term2.str.size(); i++) 
                {
                    str += term2.str[i];
                }
                res.str = str;
                return res;
            }

            if(term1.type == INT && term2.type == INT)
            {
                result.type = INT;
                result.integer = term1.integer + term2.integer;
                return result;
            }

            if((term1.type == INT && term2.type != INT) || (term1.type != INT && term2.type == INT))
            {
                std::cout << "error, cannot add number to non-number\n";
            }
            if((term1.type == LIST && term2.type != LIST) || (term1.type != LIST && term2.type == LIST))
            {
                std::cout << "error, cannot add list to non-list\n";
            }
            
        }

        if (node.value == "==") 
        {
            result.type = BOOLEAN;
            if (term1.type == NULL_ID) 
            {
                result.boolean = false;
            } 
            if (term2.type == NULL_ID)
            {
                result.boolean = false;
            } 
            else 
            {
                result.boolean = term1.integer == term2.integer;
            }
            return result;
        }

        if (node.value == ">") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer > term2.integer;
            return result;
        }

        if (node.value == "<") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer < term2.integer;
            return result;
        }

        if (node.value == ">=") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer >= term2.integer;
            return result;
        }

        if (node.value == "<=") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer <= term2.integer;
            return result;
        }

        if (node.value == "!=") 
        {
            result.type = BOOLEAN;
            result.boolean = term1.integer != term2.integer;
            return result;
        }

    }

    if (node.type == EXPRESSION) 
    {
        return evaulate(node.expression[0]);
    }

    if (node.type == INT) 
    {
        Symbol symbol = Symbol();
        symbol.type = "INT";
        symbol.integer = stoi(node.value);
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == STRING) 
    {
        Symbol symbol = Symbol();
        symbol.type = "STRING";
        symbol.str = node.value;
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == BOOLEAN) 
    {
        Symbol symbol = Symbol();
        symbol.type = "BOOLEAN";
        symbol.boolean = node.value == "true";
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == LIST) 
    {
        Symbol symbol = Symbol();
        symbol.type = "LIST";
        symbol.line = node.line;
        for (int i=0; i<node.expression.size(); i++) 
        {
            Symbol listElement = evaulate(node.expression[i]);
            symbol.list.push_back(listElement.integer);
        }
        return symbol;
    }

    if (node.type == NULL_ID) 
    {
        Symbol symbol = Symbol();
        symbol.type = "NULL";
        symbol.line = node.line;
        return symbol;
    }

    if (node.type == LIST_INDEX) 
    {
        if (symTable.find(node.symbol) != symTable.end()) 
        {
            Symbol symbol = symTable[node.symbol];
            if (node.expression.size() > 0) 
            {
                Symbol parameter = evaulate(node.parameters[0]);
                Symbol expression = evaulate(node.expression[0]);
                symbol.list[parameter.integer] = expression.integer;
                return symbol;
            } 
            else 
            {
                Symbol result = Symbol();
                result.type = INT;
                result.line = node.line;
                result.integer = symbol.list[evaulate(node.parameters[0]).integer];
                return result;
            }
        } 
    }

    if (node.type == RETURN) 
    {
        return evaulate(node.expression[0]);
    }

    if (node.type == IDENTIFIER) 
    {
        if(symTable.find(node.symbol) != symTable.end()) 
        {
            return symTable[node.symbol];
        } 
    }
    return Symbol();
}

std::vector<std::vector<token>> lexer(std::string input) 
{
    std::vector<std::vector<token>> tokens; 
    std::vector<token> lineTokens; 
    int curr_idx = 0; 
    int indentationLevel = 0; 
    int indentationLength = 0; 
    bool isIndent = false; 
    int lastIndentationLength = 0; 
    while (curr_idx < input.length()) 
    {
        std::string subStr = ""; 
        subStr += input[curr_idx]; 

        
        if (input[curr_idx] == '\n') 
        {
            if(isIndent) 
            {
                std::string indentation = "";

                if (indentationLevel == 1 && indentationLength == 0)
                {
                    int index = 1;
                    while (input[curr_idx+index] == ' ') 
                    {
                        index ++;
                        indentationLength ++;
                    }
                }

                for (int i = 1; i <= indentationLength * indentationLevel; i++) 
                {
                    if (input[curr_idx+i] == ' ') 
                    {
                        indentation += input[curr_idx+i];
                    } 
                    else 
                    {
                        break;
                    }
                }
                if (indentation.length() != 0 && indentation.length() % (indentationLength*indentationLevel) == 0) 
                {
                    curr_idx += 1 + indentation.length();
                    if (lineTokens.size() > 0) tokens.push_back(lineTokens);
                    lineTokens.clear();
                    continue;
                }
                
                if (input[curr_idx+1] == '\n') 
                {
                    curr_idx ++;
                    continue;
                }
                if (indentation.length() == 0 && indentationLevel > 1) 
                {
                    for (int i = 1; i < indentationLevel; i++) 
                    {
                        std::vector<token> breakLine;
                        token t;
                        t.type = NEW_CHUNK;
                        t.value = NEW_CHUNK;
                        breakLine.push_back(t);
                        tokens.push_back(breakLine);
                        indentationLevel --;
                    }
                }
                
                indentationLevel --;
                if (indentationLevel == 0) isIndent = false; 
                token t;
                t.type = NEW_CHUNK;
                t.value = "NEW_CHUNK";
                lineTokens.push_back(t);
                indentationLength = indentationLength * indentationLevel; 
                curr_idx += 1 + indentation.length(); 
                tokens.push_back(lineTokens); 
                lineTokens.clear(); 
                continue;
            }

            curr_idx ++; 
            if (lineTokens.size()>0) 
            {
                tokens.push_back(lineTokens);
                lineTokens.clear();
            }
            continue;
        }

        //check for comment or whitespace
        if(input[curr_idx] == '#' || input[curr_idx] == ' ') 
        {
            curr_idx ++;
            continue;
        }

        //check for comma
        if(input[curr_idx] == ',') {
            token t;
            t.type = COMMA;
            t.value = ",";
            lineTokens.push_back(t);
            curr_idx ++;
            continue;
        }

        //check for colon
        if(input[curr_idx] == ':') {
            token t;
            t.type = COLON;
            t.value = ":";
            lineTokens.push_back(t);
            curr_idx ++;
            continue;
        }

        //equal and equal_equal
        if (input[curr_idx] == '=') 
        {
            curr_idx ++;
            if (input[curr_idx] == '=') 
            {
                token t;
                t.type = OPERATOR;
                t.value = "==";
                lineTokens.push_back(t);
                curr_idx ++;
                continue;
            } 
            else 
            {
                token t;
                t.type = EQUALS;
                t.value = "=";
                lineTokens.push_back(t);
                continue;
            }
        }

        //greater, greater than
        if (input[curr_idx] == '>') 
        {
            curr_idx ++;
            if (input[curr_idx] == '=') 
            {
                token t;
                t.type = OPERATOR;
                t.value = ">=";
                lineTokens.push_back(t);
                curr_idx ++;
                continue;
            } 
            else 
            {
                token t;
                t.type = OPERATOR;
                t.value = ">";
                continue;
            }
        }

        //less, less than
        if (input[curr_idx] == '<') 
        {
            curr_idx ++;
            if (input[curr_idx] == '=') 
            {
                token t;
                t.type = OPERATOR;
                t.value = "<=";
                lineTokens.push_back(t);
                curr_idx ++;
                continue;
            } 
            else 
            {
                token t;
                t.type = OPERATOR;
                t.value = "<";
                lineTokens.push_back(t);
                continue;
            }
        }

        //check not equal
        if (input[curr_idx] == '!') 
        {
            curr_idx ++;
            if (input[curr_idx] == '=') 
            {
                token t;
                t.type = OPERATOR;
                t.value = "!=";
                lineTokens.push_back(t);
                curr_idx ++;
                continue;
            } 
        }

        //check add
        if (input[curr_idx] == '+') 
        {
            token t;
            t.type = ADD;
            t.value = "+";
            lineTokens.push_back(t);
            curr_idx ++;
            continue;
        }

        //check for parentheses
        if (input[curr_idx] == '(') 
        {
            token t;
            t.type = LEFT_PAREN;
            t.value = "(";
            lineTokens.push_back(t);
            curr_idx ++;
            continue;
        }

        if (input[curr_idx] == ')') 
        {
            token t;
            t.type = RIGHT_PAREN;
            t.value = ")";
            lineTokens.push_back(t);
            curr_idx ++;
            continue;
        }

        //checking for bracket
        if (input[curr_idx] == '[') 
        {
            token t;
            t.type = LEFT_BRACK;
            t.value = "[";
            lineTokens.push_back(t);
            curr_idx ++;
            continue;
        }

        if (input[curr_idx] == ']') 
        {
            token t;
            t.type = RIGHT_BRACK;
            t.value = "]";
            lineTokens.push_back(t);
            curr_idx ++;
            continue;
        }

        //checking for string
        if (input[curr_idx] == '\"') 
        {
            std::string str = "";
            curr_idx ++;
            while (input[curr_idx] != '\"') 
            {
                str += input[curr_idx];
                curr_idx ++;
            }
            token t;
            t.type = STRING;
            t.value = str;
            lineTokens.push_back(t);
            curr_idx ++;
            continue;
        }

        //INTEGER
        if (isdigit(subStr[0])) 
        {
            std::string number = "";
            while (isdigit(input[curr_idx]) && curr_idx < input.length()) 
            {
                number += input[curr_idx];
                curr_idx ++;
            }

            token t;
            t.type = INT;
            t.value = number;
            lineTokens.push_back(t);
            continue;
        }

        //checking for identifier
        if (isalpha(subStr[0])) 
        {
            std::string identifier = "";
            while ((isalpha(input[curr_idx]) || isdigit(input[curr_idx])) && curr_idx < input.length()) 
            {
                identifier += input[curr_idx];
                curr_idx ++;
            }

            if (identifier == "print") 
            {
                token t;
                t.type = PRINT;
                t.value = "print";
                lineTokens.push_back(t);
            
            } 
            else if (identifier == "if")
            {
                isIndent = true;
                indentationLevel ++;
                token t;
                t.type = IF;
                t.value = "if";
                lineTokens.push_back(t);
            } 
            else if (identifier == "else") 
            {
                isIndent = true;
                indentationLevel ++;
                token t;
                t.type = ELSE;
                t.value = "else";
                lineTokens.push_back(t);
            } 
            else if (identifier == "def") 
            {
                indentationLevel ++;
                isIndent = true;
                token t;
                t.type = FUNCTION_DEFINITION;
                t.value = "def";
                lineTokens.push_back(t);
            } 
            else if (identifier == "return") 
            {
                token t;
                t.type = RETURN;
                t.value = "return";
                lineTokens.push_back(t);
            } 
            else if (identifier == "True") 
            {
                token t;
                t.type = TRUE;
                t.value = "true";
                lineTokens.push_back(t);
            } 
            else if (identifier == "False") 
            {
                token t;
                t.type = FALSE;
                t.value = "false";
                lineTokens.push_back(t);
            }
            
            else 
            {
                token t;
                t.type = IDENTIFIER;
                t.value = identifier;
                lineTokens.push_back(t);
            }
            continue;
        }        
    }

    //add line tokens to tokens
    if (lineTokens.size() > 0) {
        tokens.push_back(lineTokens);
    }
    return tokens;
}
