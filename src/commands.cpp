#include <functional>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <ctype.h>

#include "forten.h"
#include "commands.h"

using namespace std;

unordered_map<string, operator_t> OPERATORS;
unordered_map<string, command_t> COMMANDS;
unordered_map<string, vector<string>> VARIABLES;
unordered_map<string, int> COUNTERS;

string next_token(vec_iterator *i, vec_iterator end)  {
  if (*i < end) {
    string output = **i;
    (*i)++;
    return output;
  } else {
    cerr << "forten: unexpected eof" << endl;
    exit(1);
  }
}

string peek_token(vec_iterator *i, vec_iterator end) {
  if (*i < end) {
    return **i;
  } else {
    cerr << "forten: unexpected eof" << endl;
    exit(1);
  }
}

string next_word(vec_iterator *i, vec_iterator end) {
  string word;
  if (peek_token(i, end) == "{") {
    (*i)++;
    word = next_token(i, end);
    (*i)++;
  }
  else while (*i < end && (isalnum((**i)[0]) || (**i)[0] == '@')) {
    word += next_token(i, end);
  }
  return word;
}

void init_operators() {
  OPERATORS["\\"] = [] (vec_iterator *i, vec_iterator end) {
    string command = next_word(i, end);
    //cerr << command << "(" << endl;
    vector<string> args;
    while (*i < end) {
      string s = peek_token(i, end);
      if (s == ":") {
	(*i)++;
	string arg = next_word(i, end);
	args.push_back(eval_string(arg));
      }
      else if (s == "{") {
	(*i)++;
	args.push_back(next_token(i, end));
	(*i)++;
      }
      else break;
    }
    /*for (string arg : args) {
      cerr << "\t" << arg << endl;
    }
    cerr << ")" << endl;*/
    if (COMMANDS.find(command) != COMMANDS.end()) {
      return COMMANDS[command](args);
    }
    else if (VARIABLES.find(command) != VARIABLES.end()) {
      check_args(command, args, 0);
      return get_var(command);
    }
    else if (COUNTERS.find(command) != COUNTERS.end()) {
      check_args(command, args, 0);
      return to_string(COUNTERS[command]);
    }
    else {
      cerr << "forten: warning: unknown command `" << command << "'" << endl;
      return string("");
    }
  };
}

bool check_args(string cmdname, vector<string> args, int n) {
  if (args.size() != n) {
    cerr << "forten: warning: " << cmdname <<
      ": wrong number of arguments (got " << args.size() << ", expected " << n << ")" << endl;
  }
  return args.size() >= n;
}

vector<vec_iterator*> i_stack;
vector<vec_iterator> end_stack;
int stack_len = 0;
bool go_else = false;

void init_commands() {
  /* Forten */
  COMMANDS["FortenVersion"] = [] (vector<string> args) {
    check_args("FortenVersion", args, 0);
    return FORTEN_VERSION;
  };

  /* System */
  COMMANDS["System"] = [] (vector<string> args) {
    if (check_args("System", args, 1)) {
      system(args[0].c_str());
    }
    return "";
  };

  /* Eval, After */
  COMMANDS["Eval"] = [] (vector<string> args) {
    if (check_args("Eval", args, 1)) {
      return eval_string(args[0]);
    }
    return string("");
  };
  COMMANDS["After"] = [] (vector<string> args) {
    if (check_args("After", args, 1)) {
      after_blocks.push_back(args[0]);
    }
    return "";
  };
  COMMANDS["EvalAgain"] = [] (vector<string> args) {
    check_args("EvalAgain", args, 0);
    eval_again = true;
    return "";
  };
  COMMANDS["Include"] = [] (vector<string> args) {
    if (check_args("Include", args, 1)) {
      return eval_file(args[0]);
    }
    return string("");
  };

  /* Ignore */
  COMMANDS["Ignore"] = [] (vector<string> args) {
    return "";
  };

  /* If */
  COMMANDS["IfEq"] = [] (vector<string> args) {
    string output("");
    if (check_args("IfEq", args, 3)) {
      if (args[0] == args[1]) {
	output = eval_string(args[2]);
	go_else = false;
      }
      else {
	go_else = true;
      }
    }
    return output;
  };
  COMMANDS["Else"] = [] (vector<string> args) {
    if (check_args("Else", args, 1)) {
      if (go_else)
	return eval_string(args[0]);
    }
    return string("");
  };

  /* Operators */
  COMMANDS["DefOp"] = [] (vector<string> args) {
    if (check_args("DefOp", args, 2)) {
      string token = args[0];
      string body = args[1];
      OPERATORS[token] = [token, body] (vec_iterator *i, vec_iterator end) {
	i_stack.push_back(i);
	end_stack.push_back(end);
	stack_len++;
	string output = eval_string(body);
	i_stack.pop_back();
	end_stack.pop_back();
	stack_len--;
	return output;
      };
    }
    return "";
  };
  COMMANDS["NextToken"] = [] (vector<string> args) {
    check_args("NextToken", args, 0);
    if (stack_len != 0)
      return next_token(i_stack.back(), end_stack.back());
    cerr << "forten: \\NextToken used outside \\DefOp body" << endl;
    return string("");
  };
  COMMANDS["PeekToken"] = [] (vector<string> args) {
    check_args("PeekToken", args, 0);
    if (stack_len != 0)
      return peek_token(i_stack.back(), end_stack.back());
    cerr << "forten: \\PeekToken used outside \\DefOp body" << endl;
    return string("");
  };
  COMMANDS["Eof"] = [] (vector<string> args) {
    check_args("Eof", args, 0);
    if (stack_len != 0)
      return *i_stack.back() < end_stack.back() ? "0" : "1";
    cerr << "forten: \\Eof used outside \\DefOp body" << endl;
    return "";
  };

  /* Commands */
  COMMANDS["DefCmd"] = [] (vector<string> args) {
    if (check_args("DefCmd", args, 3)) {
      string name = args[0];
      int n = stoi(args[1]);
      string body = args[2];
      COMMANDS[name] = [name, n, body] (vector<string> args2) {
	if (check_args(name, args2, n)) {
	  push_var("0", name);
	  for (int i = 0; i < args2.size(); i++) {
	    push_var(to_string(i+1), args2[i]);
	  }
	  string output = eval_string(body);
	  pop_var("0");
	  for (int i = 0; i < args2.size(); i++) {
	    pop_var(to_string(i+1));
	  }
	  return output;
	}
	return string("");
      };
    }
    return "";
  };
  COMMANDS["Alias"] = [] (vector<string> args) {
    if (check_args("Alias", args, 2)) {
      COMMANDS[args[0]] = COMMANDS[args[1]];
    }
    return "";
  };

  /* Variables */
  COMMANDS["PushVar"] = [] (vector<string> args) {
    if (check_args("PushVar", args, 2)) {
      push_var(args[0], args[1]);
    }
    return "";
  };
  COMMANDS["PopVar"] = [] (vector<string> args) {
    if (check_args("PopVar", args, 1)) {
      pop_var(args[0]);
    }
    return "";
  };
  COMMANDS["SetVar"] = [] (vector<string> args) {
    if (check_args("SetVar", args, 2)) {
      if (!VARIABLES[args[0]].empty()) {
	pop_var(args[0]);
	push_var(args[0], args[1]);
      }
    }
    return "";
  };

  /* Counters */
  COMMANDS["SetCounter"] = [] (vector<string> args) {
    if (check_args("SetCounter", args, 2)) {
      COUNTERS[args[0]] = stoi(args[1]);
    }
    return "";
  };
  COMMANDS["Increase"] = [] (vector<string> args) {
    if (check_args("Increase", args, 1)) {
      if (COUNTERS.find(args[0]) != COUNTERS.end()) {
	COUNTERS[args[0]]++;
      }
      else {
	cerr << "forten: counter not found: `" << args[0] << "'" << endl;
      }
    }
    return "";
  };
  COMMANDS["Decrease"] = [] (vector<string> args) {
    if (check_args("Decrease", args, 1)) {
      if (COUNTERS.find(args[0]) != COUNTERS.end()) {
	COUNTERS[args[0]]--;
      }
      else {
	cerr << "forten: counter not found: `" << args[0] << "'" << endl;
      }
    }
    return "";
  };
}
