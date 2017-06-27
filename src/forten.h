#ifndef __FORTEN_H
#define __FORTEN_H

#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef vector<string>::iterator vec_iterator;

vector<string> tokenize(istream &input, string filename);

string eval_string(string code);
string eval_file(string filename);
string eval_tokens(vec_iterator start, vec_iterator end);

const string FORTEN_VERSION = "Forten 0.1.0";

#endif
