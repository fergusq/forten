#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "forten.h"
#include "commands.h"

using namespace std;

vector<string> tokenize(istream &input, string filename) {
  //cerr << "Tokenizing..." << endl;
  vector<string> output;
  int linenum = 1;
  char chr;
  while (input.get(chr)) {
    if (chr == '\n') linenum++;
    if (chr == '`') {
      if (!input.get(chr)) {
	cerr << "forten: at " << filename << ":" << linenum << ": syntax error: unexpected eof, unfinished escape sequence" << endl;
	exit(1);
      }
      //cerr << "Pushing escape seq: `" << chr << endl;
      output.push_back("`" + string(1, chr));
    } else if (chr == '{') {
      string token;
      int depth = 1;
      while (depth > 0) {
	if (!input.get(chr)) {
	  cerr << "forten: at " << filename << ":" << linenum << ": syntax error: unexpected eof, unbalanced braces" << endl;
	  exit(1);
	}
	if (chr == '\n') linenum++;
	if (chr == '{') depth++;
	if (chr == '}') depth--;
	if (depth > 0) token += chr;
      }
      //cerr << "Pushing block: {" << token << "}" << endl;
      output.push_back("{");
      output.push_back(token);
      output.push_back("}");
    } else {
      //cerr << "Pushing char: `" << chr << endl;
      output.push_back(string(1, chr));
    }
  }
  return output;
}

string eval_string(string code) {
  istringstream input(code);
  vector<string> tokens = tokenize(input, "<block>");
  return eval_tokens(tokens.begin(), tokens.end());
}

string eval_file(string filename) {
  ifstream input(filename);
  vector<string> tokens = tokenize(input, filename);
  input.close();
  return eval_tokens(tokens.begin(), tokens.end());
}

string eval_tokens(vec_iterator start, vec_iterator end) {
  string output;
  vec_iterator i;
  for (i = start; i < end;) {
    string token = *i;
    i++;
    if (OPERATORS.find(token) != OPERATORS.end()) {
      output += OPERATORS[token](&i, end);
    } else if (token[0] == '`') {
      output += token.substr(1, token.length()-1);
    } else {
      output += token;
    }
  }
  return output;
}

vector<string> after_blocks;
bool eval_again = false;

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Usage: forten <file>" << endl;
    return 1;
  }

  init_operators();
  init_commands();

  char *filename = argv[1];
  string output;
  bool first = true;
  do {
    eval_again = false;
    output = first ? eval_file(filename) : eval_string(output);
    for (vec_iterator i = after_blocks.begin(); i < after_blocks.end(); i++) {
      eval_string(*i);
    }
    after_blocks.erase(after_blocks.begin(), after_blocks.end());
    first = false;
  } while (eval_again);
  cout << output << endl;
  
  return 0;
}
