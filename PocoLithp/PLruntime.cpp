#include "stdafx.h"

using namespace PocoLithp;

// Runtime information
LithpCell proc_banner(const LithpCells &x) {
	return LithpCell(Var, lithp_banner);
}

// File IO
LithpCell proc_readfile(const LithpCells &x) {
	if (x.size() < 1)
		throw InvalidArgumentException("No filename provided");
	std::ifstream f(x[0].str());
	std::stringstream ss;
	ss << f.rdbuf();
	f.close();
	return LithpCell(Var, ss.str());
}

// Convert a string to an atom
LithpCell proc_atom(const LithpCells &x) {
	if(x.size() == 0)
		return LithpCell(Atom, "atom");
	return LithpCell(Atom, x[0].str());
}

// Throw an InvalidArgument exception
LithpCell proc_invalid_arg(const LithpCells &c) {
	std::string message = "";
	if(c.size() > 0)
		message = c[0].str();
	throw InvalidArgumentException(message);
}

void PocoLithp::init_runtime() {
	add_environment_runtime([](LithpEnvironment &env) {
		env["banner"] = LithpCell(&proc_banner);
		env["readfile"] = LithpCell(&proc_readfile);
		env["atom"] = LithpCell(&proc_atom);
		env["invalid_arg"] = LithpCell(&proc_invalid_arg);
	});
}
