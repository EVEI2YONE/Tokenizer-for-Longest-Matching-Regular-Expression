/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include "parser.h"
#include <set>
#include <algorithm>

#define PARSE 0
#define DEBUG 0
#define COMPILE 1
using namespace std;
int x = 1;

void my_LexicalAnalyzer::printREG_list(struct REG_list *list) {
    while(list != NULL) {
        cout << list->token_name << endl;
        list = list->next;
    }
    return;
}

std::set<struct REG_node *> my_LexicalAnalyzer::addNodes(struct REG_node *node) {
    std::set<struct REG_node *> set1;
    std::set<struct REG_node *> set2;
    std::set<struct REG_node *> _set;
    _set.insert(node);
    //recursively add '_' nodes
    if(node->first_neighbor != NULL && node->first_label == '_') {
        set1 = addNodes(node->first_neighbor);
    }
    if(node->second_neighbor != NULL && node->second_label == '_') {
        set2 = addNodes(node->second_neighbor);
    }
    std::set<struct REG_node *>::iterator it;
    for(it = set1.begin(); it != set1.end(); it++) {
        struct REG_node *add = *it;
        _set.insert(add);
    }
    for(it = set2.begin(); it != set2.end(); it++) {
        struct REG_node *add = *it;
        _set.insert(add);
    }
    return _set;
}

bool my_LexicalAnalyzer::checkChange(std::set<struct REG_node *> set1, std::set<struct REG_node *> set2) {
    std::set<struct REG_node *>::iterator it1;
    std::set<struct REG_node *>::iterator it2;
    //set2 should have more items than set1 because set2 is S'' and set1 is S'
    for(it2 = set2.begin(); it2 != set2.end(); it2++) {
        struct REG_node *item = *it2;
        //checking if pointer in S'' is also in S'
        it1 = set1.find(item);
        //pointer not found in S'. S'' is different thus changed = true
        if(it1 == set1.end()) {
            return true;
        }
    }
    return false;
}

struct REG_list * my_LexicalAnalyzer::resetLengths(struct REG_list *list) {
    struct REG_list *parser = list;
    while(parser != NULL) {
        parser->longest_length = -1;
        parser = parser->next;
    }
    return list;
}
/*
std::string my_LexicalAnalyzer::updateString(std::string str, int pos) {
    str = str.substr(pos);
    return str;
}
*/
int my_LexicalAnalyzer::findLongest(struct REG_list *list) {
    int pos = 0;
    if(list == NULL) {
        //cout << "found longest length to be 0" << endl;
        return 0;
    }
    while(list != NULL) {
        if(pos < list->longest_length) {
            pos = list->longest_length;
        }
        list = list->next;
    }
    return pos;
}

std::string my_LexicalAnalyzer::cleanString(std::string str) {
    if(str.length() != 0) {
        while(str.at(0) == ' ') {
            if(str.size() != 1) {
                str = str.substr(1);
            }
            else {
                return "";
            }
        }
    }
    return str;
}

std::string my_LexicalAnalyzer::viableString(std::string str) {
    std::string temp = "";
    if(str.length() != 0) {
        int i = 0;
        int length = str.length();
        while(str.at(i) != ' ' && i < length) {
            temp += str.at(i);
            i++;
            if(i == length) {
                break;
            }
        }
    }
    return temp;
}

struct REG_list * my_LexicalAnalyzer::findLexeme(struct REG_list *list, int length) {
      while(list != NULL) {
          if(list->longest_length == length)
              break;
          list = list->next;
      }
      return list;
}

std::set<struct REG_node *> my_LexicalAnalyzer::match_one_char(std::set<struct REG_node *> s, char c) {
  if(PARSE)
  cout << "match_one_char " << c << endl;
    std::set<struct REG_node *> s_prime;
    std::set<struct REG_node *>::iterator it;
    //parse through a set of REG_node pointers
    //with each REG_node pointer, add corresponding reachable nodes after consuming c
    if(DEBUG)
    cout << "s size: " << s.size() << endl;
    for(it = s.begin(); it != s.end(); it++) {
        struct REG_node *path = *it;
        if(path->first_neighbor != NULL && path->first_label == c) {
            s_prime.insert(path->first_neighbor);
        }
        if(path->second_neighbor != NULL && path->second_label == c) {
            s_prime.insert(path->second_neighbor);
        }
    }
    //check for empty empty string - resetLengths needs to be -1 instead of 0
    //no paths reachable at this point - returns an empty set
    if(s_prime.begin() == s_prime.end()) {
      if(DEBUG)
      cout << "s_prime is empty" << endl;
        return s_prime;
    }
    else if(DEBUG){
        cout << "viable node(s):";
        for(it = s_prime.begin(); it != s_prime.end(); it++) {
            struct REG_node *check = *it;
            cout << " " << check->a << endl;
        }
        cout << "s_prime size: " << s_prime.size() << endl;
    }
    //check for '_' after consuming char for update set of reachable nodes
    bool changed = true;
    std::set<struct REG_node *> s_pp;
    //S' slowly inserts all viable '_' nodes
    while(changed) {
        changed = false;
        std::set<struct REG_node *>::iterator p;
        //parse through S' set
        for(p = s_prime.begin(); p != s_prime.end(); p++) {
            std::set<struct REG_node *> set1;
            //iteratively copy S' into S''
            struct REG_node *add = *p;
            //no need to insert(add) because add is already in S' which is being updated
//            s_pp.insert(add);
            //recursively add '_' nodes after consuming char c
            set1 = addNodes(add);
            //there is a copy of S'' and two sets from branching '_' nodes
            std::set<struct REG_node *>::iterator n;
            //copy the two sets (obtained from branching) into S''
            for(n = set1.begin(); n != set1.end(); n++) {
                struct REG_node *temp1 = *n;
                s_pp.insert(temp1);
            }
        }
        if(checkChange(s_prime, s_pp) == true) {
            changed = true;
            //due to S' != S'' -> S' must copy S'' (update S')
            for(p = s_pp.begin(); p != s_pp.end(); p++) {
                struct REG_node *add = *p;
                s_prime.insert(add);
            }
            //empty S''
            while(!s_pp.empty()) {
                s_pp.erase(s_pp.begin());
            }
        }
    }
     //S' is updated at this point to included all viable nodes
     //viable incldues nodes reached when consuming char c, and
     //nodes reached after iteratively adding '_' stemming from nodes reached by consuming c
    return s_prime;
}

int my_LexicalAnalyzer::match(REG *r, std::string s, int p) {
    if(PARSE)
    cout << "match" << endl;
    std::set<struct REG_node *> _set;
    //initial parse of REG expression for '_' nodes
    _set = addNodes(r->start);
    //cout << "_set size: " << _set.size() << endl;
    //while set is not empty and p can parse string length
    if(DEBUG)
    cout << "initial epsilon nodes: " << _set.size() << endl;
    int length = -1;
    //check if initial set of nodes has an accept node
    std::set<struct REG_node *>::iterator it;
    for(it = _set.begin(); it != _set.end(); it++) {
        struct REG_node *node = *it;
        if(node == r->accept) {
            if(DEBUG)
            cout << "epsilon found" << endl;
            length = 0;
        }
    }
    while(p < s.length()) {
        //gets next set of nodes
        _set = match_one_char(_set, s.at(p));
        //_set may return empty - or it may have accept_node
        if(_set.size() == 0) {
            //empty because token string is max
            if(DEBUG)
            cout << "string parsed" << endl;
            break;
        }
        //check if resulting set of nodes has an accept node
        std::set<struct REG_node *>::iterator _accept;
        for(_accept = _set.begin(); _accept != _set.end(); _accept++) {
            struct REG_node *node = *_accept;
            if(node == r->accept) {
                length = p + 1;
            }
        }
        if(DEBUG)
        cout << "pos: " << p << endl;
        p++;
    }
    if(DEBUG)
    cout << s << endl;
    if(length == -1) {
      if(DEBUG)
      cout << "no matches" << endl;
//if(COMPILE)
//      cout << "ERROR";
    }
    else if(_set.begin() == _set.end() && length < s.length()) {
      if(DEBUG)
      cout << "string partially tokenized" << endl;
    }
    else if(_set.begin() != _set.end() && length == s.length()) {
      if(DEBUG)
      cout << "viable string tokenized" << endl;
    }
    if(DEBUG)
    cout << "length returned: " << length << endl;
    return length;
}

struct REG_list * my_LexicalAnalyzer::my_getToken(struct REG_list *list, std::string str) {
  if(PARSE)
  cout << "my_getToken" << endl;
    struct REG_list *parser = list;
    while(parser != NULL) {
        //lexeme
        if(DEBUG)
        cout << "lexeme: " << parser->token_name << endl;
        parser->longest_length = match(parser->reg_pointer, str, 0);
        if(DEBUG)
        cout << "longest length: " << parser->longest_length << endl;
        parser = parser->next;
    }
    return list;
}

my_LexicalAnalyzer::my_LexicalAnalyzer(struct REG_list *list, std::string s) {
    //printREG_list(list);
    list = resetLengths(list);
    int length = s.length();
    struct REG_list *parser = list;
    while(length != 0) {
        if(DEBUG)
        cout << "original string: |" << s << "|" << endl;
        if(DEBUG)
        cout << "string length: " << length << endl;
        s = cleanString(s);
        if(s.size() == 0) {
          if(DEBUG)
            cout << "no more string" << endl;
            break;
        }
        std::string temp = viableString(s);
        list = my_getToken(list, temp);
        int pos = findLongest(list);
        if(pos == -1) {
            if(DEBUG || COMPILE)
            cout << "ERROR";
            break;
        }
        else if(pos == 0) {
            if(DEBUG || COMPILE)
            cout << "ERROR";
            break;
        }
        else {
            REG_list *lex = findLexeme(list, pos);
            if(COMPILE)
            cout << lex->token_name << " , ";
            if(DEBUG)
            cout << "longest find: " << pos << endl;
            //tokenized string
            if(DEBUG || COMPILE)
            cout << "\"" << temp.substr(0, pos) << "\"";
            if(DEBUG)
            cout << "before tokination: |" << s << "|"<< endl;

            s = s.substr(pos, s.length()-pos);
            if(DEBUG)
            cout << "after tokenization: |" << s << "|" << endl;
    //        s = updateString(s, pos);
            length = s.length();
            if(DEBUG)
            cout << "post length: " << length << endl;
            list = resetLengths(list);
        }
    }
/*    set_of_nodes.insert(list->reg_pointer->start);
    std::set<struct REG_node *>::iterator it;
    for(it = set_of_nodes.begin(); it != set_of_nodes.end(); it++) {
        struct REG_node *temp = *it;
        //cout << temp->first_label << endl;
    }
*/
    return;
}

void Parser::printREG(struct REG_node *node) {
    while(node->first_neighbor != NULL) {
        cout << node->first_label;
        node = node->first_neighbor;
    }
    cout << endl;
    return;
}

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

// this function simply checks the next token without
// consuming the input
// Written by Mohsen Zohrevandi
Token Parser::peek()
{
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}

// Parsing

void Parser::parse_input()
{
    //input -> tokens_section INPUT_TEXT
    struct REG_list *list = parse_tokens_section();
    Token input = expect(INPUT_TEXT);
    //get rid of quotes and initial spaces
    //std::string temp = (input.lexeme).substr(2, (input.lexeme).length()-4);
    std::string temp = (input.lexeme).substr(1, (input.lexeme).length()-2);
    my_LexicalAnalyzer analyze = my_LexicalAnalyzer(list, temp);
}


struct REG_list * Parser::parse_tokens_section()
{
    // tokens_section -> token_list HASH
    struct REG_list *list = parse_token_list();
    expect(HASH);
    return list;
}

struct REG_list * Parser::parse_token_list()
{
    // token_list -> token
    // token_list -> token COMMA token_list
    struct REG_list *expression = parse_token();
    Token t = peek();
    if (t.token_type == COMMA)
    {
        // token_list -> token COMMA token_list
        expect(COMMA);
        expression->next = parse_token_list();
    }
    else if (t.token_type == HASH)
    {
        // token_list -> token
    }
    else
    {
        syntax_error();
    }
    return expression;
}

struct REG_list * Parser::parse_token()
{
    // token -> ID expr
    x = 1;
    Token t = expect(ID);
    struct REG *reg_expr = parse_expr();
    //printREG(reg_expr->start);
    struct REG_list *list = new REG_list;
    list->token_name = t.lexeme;
    list->reg_pointer = reg_expr;
    list->next = NULL;
    return list;
}

struct REG * Parser::parse_expr()
{
    // expr -> CHAR
    // expr -> LPAREN expr RPAREN DOT LPAREN expr RPAREN
    // expr -> LPAREN expr RPAREN OR LPAREN expr RPAREN
    // expr -> LPAREN expr RPAREN STAR
    // expr -> UNDERSCORE
    struct REG *reg_expr = (struct REG *)malloc(sizeof(struct REG));
    Token t = lexer.GetToken();
    if(t.token_type == CHAR){
        // expr -> CHAR
        struct REG_node *_char = (struct REG_node *)malloc(sizeof(struct REG_node));
        struct REG_node *temp_accept = (struct REG_node *)malloc(sizeof(struct REG_node));
        _char->first_neighbor = temp_accept;
        std::string convert = t.lexeme;
        char ch = convert.at(0);
        _char->first_label = ch;
        _char->second_neighbor = NULL;
        temp_accept->first_neighbor = NULL;
        temp_accept->second_neighbor = NULL;
        reg_expr->start = _char;
        reg_expr->accept = temp_accept;
        _char->a = x++;
        temp_accept->a = x++;
    }
    else if (t.token_type == UNDERSCORE){
        // expr -> UNDERSCORE
        struct REG_node *underscore = (struct REG_node *)malloc(sizeof(struct REG_node));
        struct REG_node *temp_accept = (struct REG_node *)malloc(sizeof(struct REG_node));
        underscore->first_neighbor = temp_accept;
        underscore->first_label = '_';
        underscore->second_neighbor = NULL;
        temp_accept->first_neighbor = NULL;
        temp_accept->second_neighbor = NULL;
        reg_expr->start = underscore;
        reg_expr->accept = temp_accept;
        underscore->a = x++;
        temp_accept->a = x++;
    }
    else if(t.token_type == LPAREN){
        // expr -> LPAREN expr RPAREN DOT LPAREN expr RPAREN
        // expr -> LPAREN expr RPAREN OR LPAREN expr RPAREN
        // expr -> LPAREN expr RPAREN STAR
        struct REG *first_term = parse_expr();
        expect(RPAREN);
        Token t2 = lexer.GetToken();
        if(t2.token_type == DOT || t2.token_type == OR){
            expect(LPAREN);
            struct REG *second_term = parse_expr();
            expect(RPAREN);
            if(t2.token_type == DOT) {

                first_term->accept->first_neighbor = second_term->start;
                first_term->accept->first_label = '_';
                reg_expr->start = first_term->start;
                reg_expr->accept = second_term->accept;

            }
            else {
                struct REG_node *fork = (struct REG_node *)malloc(sizeof(struct REG_node));
                fork->first_neighbor = first_term->start;
                fork->first_label = '_';
                fork->second_neighbor = second_term->start;
                fork->second_label = '_';
                struct REG_node *fork_accept = (struct REG_node *)malloc(sizeof(struct REG_node));
                first_term->accept->first_neighbor = fork_accept;
                first_term->accept->first_label = '_';
                second_term->accept->first_neighbor = fork_accept;
                second_term->accept->first_label = '_';

                fork_accept->first_neighbor = NULL;
                fork_accept->second_neighbor = NULL;

                reg_expr->start = fork;
                reg_expr->accept = fork_accept;
                fork->a = x++;
                fork_accept->a = x++;
            }
            free(first_term);   //first REG object
            free(second_term);  //second REG object
        }
        else if(t2.token_type == STAR)
        {
            struct REG_node *initial_node = (struct REG_node *)malloc(sizeof(struct REG_node));
            struct REG_node *accept_node = (struct REG_node *)malloc(sizeof(struct REG_node));
            initial_node->first_neighbor = first_term->start;
            initial_node->first_label = '_';
            initial_node->second_neighbor = accept_node;
            initial_node->second_label = '_';
            first_term->accept->first_neighbor = accept_node;
            first_term->accept->first_label = '_';
            first_term->accept->second_neighbor = first_term->start;
            first_term->accept->second_label = '_';
            accept_node->first_neighbor = NULL;
            accept_node->second_neighbor = NULL;
            reg_expr->start = initial_node;
            reg_expr->accept = accept_node;
            initial_node->a = x++;
            accept_node->a = x++;
        }
    }
    else
    {
        syntax_error();
    }
    return reg_expr;
}


void Parser::ParseProgram()
{
    parse_input();
    expect(END_OF_FILE);
}

int main()
{
    Parser parser;
    parser.ParseProgram();
}
