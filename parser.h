/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include <set>

class Parser {
  private:
    LexicalAnalyzer lexer;

    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();

    void printREG(struct REG_node *n);
    void parse_input();
    void parse_char_list();
    struct REG_list * parse_tokens_section();
    struct REG_list * parse_token_list();
    struct REG_list * parse_token();
    struct REG * parse_expr();


  public:
    void ParseProgram();
};

class my_LexicalAnalyzer
{
private:
    std::set<struct REG_node *> addNodes(struct REG_node *_set);
    bool checkChange(std::set<struct REG_node *> prime, std::set<struct REG_node *> prime_prime);
    struct REG_list * resetLengths(struct REG_list *list);
  //  std::string updateString(std::string str, int pos);
    int findLongest(struct REG_list *list);
    std::string cleanString(std::string str);
    std::string viableString(std::string str);
    struct REG_list * findLexeme(struct REG_list *list, int length);

  //stores the list of structures and keeps track of the part
  //of the input string that has been processed
public:
    void printREG_list(struct REG_list *list);
    std::set<struct REG_node *> match_one_char(std::set<struct REG_node *> s, char c);
    int match(REG *r, std::string s, int p);
    struct REG_list * my_getToken(struct REG_list *list, std::string s);
    my_LexicalAnalyzer(struct REG_list *list, std::string s);

};

struct REG_node
{
    int a;
    REG_node(int x) {
        a = x;
    }
    struct REG_node *first_neighbor;
    char first_label;
    struct REG_node *second_neighbor;
    char second_label;
};

struct REG
{
  struct REG_node *start;
  struct REG_node *accept;
};

struct REG_list
{
    std::string token_name;
    struct REG *reg_pointer;
    struct REG_list *next;
    int longest_length;
};
#endif
