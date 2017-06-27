#ifndef __COMMANDS_H
#define __COMMANDS_H

#include <functional>
#include <unordered_map>
#include <vector>

#include "forten.h"

using namespace std;


typedef function<string(vec_iterator *, vec_iterator)> operator_t;
extern unordered_map<string, operator_t> OPERATORS;

typedef function<string(vector<string>)> command_t;
extern unordered_map<string, command_t> COMMANDS;

extern unordered_map<string, vector<string>> VARIABLES;
extern unordered_map<string, int> COUNTERS;

inline void push_var(string name, string val) {
  VARIABLES[name].push_back(val);
}

inline void pop_var(string name) {
  VARIABLES[name].pop_back();
}
inline string get_var(string name) {
  return VARIABLES[name][VARIABLES[name].size()-1];
}

string next_token(vec_iterator *i, vec_iterator end);
string peek_token(vec_iterator *i, vec_iterator end);
string next_word(vec_iterator *i, vec_iterator end);

bool check_args(string name, vector<string> args, int n);

void init_operators();
void init_commands();

#endif
