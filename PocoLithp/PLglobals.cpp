#include "stdafx.h"

PocoLithp::AtomMapById_t atomMapById;
PocoLithp::AtomMapByName_t atomMapByName;

typedef std::list<PocoLithp::add_environment_proc> environment_procs_t;
environment_procs_t environment_procs;

namespace PocoLithp {
	const std::string lithp_banner =
		"Welcome to PocoLithp " PLITHP_VERSION " " + PLITHP_ARCH + " " + STATS_DESC + "\n"
		"Type (q) to quit, (debug) to get / set state, (timing) to get / set state" + "\n"
		"  Additional useful functions: (tests) (str expr) (str expr true) (env) (_depth) (_max_depth)\n"
		"                             : (repl [prompt]) (_eval expr) (expl \"string\") \n";

	const LithpCell sym_false(Atom, "false");
	const LithpCell sym_true(Atom, "true");
	const LithpCell sym_nil(Atom, "nil");
	const LithpCell sym_quote(Atom, "quote");
	const LithpCell sym_if(Atom, "if");
	const LithpCell sym_get(Atom, "get!");
	const LithpCell sym_set(Atom, "set!");
	const LithpCell sym_define(Atom, "define");
	const LithpCell sym_defined(Atom, "defined");
	const LithpCell sym_lambda(Atom, "lambda");
	const LithpCell sym_lambda2(Atom, "#");   // Alternative to lambda
	const LithpCell sym_macro(Atom, "macro");
	const LithpCell sym_begin(Atom, "begin");

	const LithpCell booleanCell(const bool val) {
		return val ? sym_true : sym_false;
	}
	bool booleanVal(const LithpCell &val) {
		return val == sym_true;
	}

	std::string getAtomById(atomId id) {
		return atomMapById[id];
	}

	atomId getAtomId(const std::string &name) {
		AtomMapByName_t::const_iterator it = atomMapByName.find(name);
		if (it != atomMapByName.end())
			return it->second;
		atomId id = (atomId)atomMapByName.size();
		atomMapByName.emplace(name, id);
		atomMapById.emplace(id, name);
		return id;
	}
	LithpCell getAtom(const std::string &name) {
		return LithpCell(Atom, getAtomId(name));
	}

	LithpCell proc_add(const LithpCells &c)
	{
		if(c.empty()) return LithpCell(Var, 0);
		LithpCell n(c[0]);
		auto i = c.cbegin(); ++i;
		for (; i != c.cend(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
				n.value = n.value.convert<SignedInteger>();
			}
			n += *i;
		}
		return n;
	}

	LithpCell proc_sub(const LithpCells &c)
	{
		if(c.empty()) return LithpCell(Var, 0);
		LithpCell n(c[0]);
		auto i = c.cbegin(); ++i;
		for (; i != c.cend(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
				n.value = n.value.convert<SignedInteger>();
			}
			n -= *i;
		}
		return n;
	}

	LithpCell proc_mul(const LithpCells &c)
	{
		if(c.empty()) return LithpCell(Var, 0);
		LithpCell n(c[0]);
		auto i = c.cbegin(); ++i;
		for (; i != c.cend(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
				n.value = n.value.convert<SignedInteger>();
			}
			n *= *i;
		}
		return n;
	}

	LithpCell proc_div(const LithpCells &c)
	{
		if(c.empty()) return LithpCell(Var, 0);
		LithpCell n(c[0]);
		auto i = c.cbegin(); ++i;
		for (; i != c.cend(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
				n.value = n.value.convert<SignedInteger>();
			}
			n /= *i;
		}
		return n;
	}

	LithpCell proc_greater(const LithpCells &c)
	{
		if(!c.size_atleast(2)) return sym_false;
		LithpCell n(c[0]);
		auto i = c.cbegin(); ++i;
		for (; i != c.cend(); ++i)
			if (n <= *i)
				return sym_false;
		return sym_true;
	}

	LithpCell proc_greater_equal(const LithpCells &c)
	{
		if(!c.size_atleast(2)) return sym_false;
		LithpCell n(c[0]);
		auto i = c.cbegin(); ++i;
		for (; i != c.cend(); ++i)
			if (n < *i)
				return sym_false;
		return sym_true;
	}

	LithpCell proc_less(const LithpCells &c)
	{
		if(!c.size_atleast(2)) return sym_false;
		LithpCell n(c[0]);
		auto i = c.cbegin(); ++i;
		for (; i != c.cend(); ++i) {
			if (n >= *i)
				return sym_false;
		}
		return sym_true;
	}

	LithpCell proc_less_equal(const LithpCells &c)
	{
		if(!c.size_atleast(2)) return sym_false;
		LithpCell n(c[0]);
		auto i = c.cbegin(); ++i;
		for (; i != c.cend(); ++i) {
			if (n > *i)
				return sym_false;
		}
		return sym_true;
	}

	LithpCell proc_equal(const LithpCells &c)
	{
		if(!c.size_atleast(2)) return sym_false;
		return booleanCell(c[0] == c[1]);
	}

	LithpCell proc_not_equal(const LithpCells &c)
	{
		if(!c.size_atleast(2)) return sym_false;
		return booleanCell(c[0] != c[1]);
	}

	LithpCell proc_not(const LithpCells &c)
	{
		if(c.empty()) return sym_true;
		return booleanCell(!(c[0] == sym_true));
	}

	LithpCell proc_length(const LithpCells &c) {
		if(c.empty()) return LithpCell(Var, 0);
		return LithpCell(Var, c[0].size());
	}
	LithpCell proc_nullp(const LithpCells &c) {
		if(c.empty()) return sym_true;
		return c[0].is_nullp() ? sym_true : sym_false;
	}
	LithpCell proc_empty(const LithpCells &c) {
		if (c.hasHead())
			return booleanCell(c[0].list().hasHead() == false);
		return sym_false;
	}
	LithpCell proc_head(const LithpCells &c) {
		if (c.hasHead())
			return c[0].list().front();
		else
			return sym_nil;
	}

	LithpCell proc_tail(const LithpCells &c)
	{
		if (c.empty())
			return LithpCell(List, LithpCells());
		const LithpCells &list = c[0].list();
		return LithpCell(List, list.tail_clist());
	}

	LithpCell proc_append(const LithpCells &c)
	{
		if (c.empty())
			return LithpCell(List, LithpCells());
		else if(!c.hasTail())
			return LithpCell(List, c[0].list());
		LithpCells result = c[0].list();
		const LithpCells &c1l = c[1].list();
		for (auto i = c1l.cbegin(); i != c1l.cend(); ++i)
			result.push_back(*i);
		return LithpCell(List, result);
	}

	LithpCell proc_cons(const LithpCells &c)
	{
		if (c.empty())
			return LithpCell(List, LithpCells());
		else if(!c.size_atleast(1))
			return LithpCell(List, c[0].list());
		LithpCells result;
		const LithpCells &c1l = c[1].list();
		result.push_back(c[0]);
		for (auto i = c1l.cbegin(); i != c1l.cend(); ++i)
			result.push_back(*i);
		return LithpCell(List, result);
	}

	LithpCell proc_list(const LithpCells &c)
	{
		return LithpCell(List, c);
	}

	// Explode a string into a list
	LithpCell proc_expl(const LithpCells &c) {
		if(c.empty()) return LithpCell(List, LithpCells());
		std::string str = c[0].str();
		LithpCells list;
		for (auto it = str.begin(); it != str.end(); ++it)
			list.push_back(LithpCell(Var, std::string(1, *it)));
		return LithpCell(List, list);
	}

	// Get or set debug state
	LithpCell proc_debug(const LithpCells &c)
	{
		bool was = DEBUG;
		if (!c.empty())
			DEBUG = booleanVal(c[0]);
		return booleanCell(was);
	}

	// Get or set timing state
	LithpCell proc_timing(const LithpCells &c)
	{
		bool was = TIMING;
		if (!c.empty())
			TIMING = booleanVal(c[0]);
		return booleanCell(was);
	}

	// Quit interpreter
	LithpCell proc_quit(const LithpCells &c)
	{
		QUIT = true;
		return sym_true;
	}

	// Get number of reductions
	LithpCell proc_reds(const LithpCells &c) {
		return LithpCell(Var, reductions);
	}

	// Run tests
	LithpCell proc_tests(const LithpCells &c) {
		return LithpCell(Var, Test::RunTests());
	}

	// Get environment values
	LithpCell proc_env(const LithpCells &c, Env_p env) {
		return LithpCell(List, env->getCompleteEnv());
	}

	// Get keys from dictionary
	LithpCell proc_keys(const LithpCells &c) {
		if(c.empty()) throw InvalidArgumentException("Not enough parameters for keys(::dict)");
		const LithpDict &dict = c[0].dict();
		LithpCells keys;
		for(auto it = dict.cbegin(); it != dict.cend(); ++it)
			keys.push_back(LithpCell(Atom, it->first));
		return LithpCell(List, keys);
	}

	// Get values from dictionary
	LithpCell proc_values(const LithpCells &c) {
		if(c.empty()) throw InvalidArgumentException("Not enough parameters for values(::dict)");
		const LithpDict &dict = c[0].dict();
		LithpCells values;
		for(auto it = dict.cbegin(); it != dict.cend(); ++it)
			values.push_back(LithpCell(Var, it->second));
		return LithpCell(List, values);
	}

	// Get current eval depth
	LithpCell proc__depth(const LithpCells &c) {
		TRACK_STATS(return LithpCell(Var, depth));
		return LithpCell(Atom, "#stats_not_tracked");
	}

	// Get max eval depth
	LithpCell proc__depth_max(const LithpCells &c) {
		TRACK_STATS(return LithpCell(Var, depth_max));
		return LithpCell(Atom, "#stats_not_tracked");
	}

	/** IO Procedures */

	// Print the given arguments
	LithpCell proc_print(const LithpCells &c) {
		std::stringstream ss;
		bool first = true;
		for (auto it = c.cbegin(); it != c.cend(); ++it) {
			if (first) first = false;
			else ss << " ";
			ss << to_string(*it);
		}
		std::cout << ss.str() << "\n";
		return sym_nil;
	}

	// Read a line
	LithpCell proc_getline(const LithpCells &c) {
		std::string prompt = "> ";
		if(!c.empty())
			prompt = c[0].str();
		std::string line = GETLINE(prompt);
		return LithpCell(Var, line);
	}

	// Invoke REPL
	LithpCell proc_repl(const LithpCells &c, Env_p env) {
		if (c.size_atleast(1))
			return repl(c[0].str(), env);
		else
			return repl(env);
	}

	// Call the eval function in the current environment with the tokenized
	// arguments.
	// It is named _eval since implementing a native  eval function is
	// a goal of this interpreter. It would allow extending the
	// interpreter in classic Lisp ways.
	LithpCell proc__eval(const LithpCells &c, Env_p env) {
		if(c.empty()) return sym_nil;
		return eval(c[0], env);
	}

	// Call the eval function in the topmost environment with the tokenized
	// arguments.
	// This means any definitions will be defined in the topmost environment.
	// TODO: Should a define-all or export be implemented instead?
	LithpCell proc__eval_ctx(const LithpCells &c, Env_p env) {
		if(c.empty()) return sym_nil;
		return eval(c[0], env->getTopmost(env));
	}

	// Tokenize the given string
	LithpCell proc__tokenize(const LithpCells &c) {
		if(c.empty()) return sym_nil;
		std::list<std::string> tokens = tokenize(c[0].str());
		return read_from(tokens);
	}

	/** String procedures */

	// Convert argument to string
	LithpCell proc_tostring(const LithpCells &c) {
		bool repre = false;
		if (c.empty())
			return LithpCell(Var, std::string(""));
		if (c.size_atleast(2))
			repre = c[1] == sym_true ? true : false;
		return LithpCell(Var, to_string(c[0], true, repre));
	}

	/** Variable procedures */

	const LithpCell tag_atom(Atom, "atom");
	const LithpCell tag_number(Atom, "number");
	const LithpCell tag_string(Atom, "string");
	const LithpCell tag_other(Atom, "other");
	const LithpCell tag_lambda(Atom, "lambda");
	const LithpCell tag_proc(Atom, "proc");
	const LithpCell tag_list(Atom, "list");
	const LithpCell tag_dict(Atom, "dict");

	// Get the tag of the given variable
	LithpCell proc_tag(const LithpCells &c) {
		if (c.empty()) throw InvalidArgumentException("Not enough arguments to tag(N)");
		const LithpCell &c0 = c[0];
		switch (c0.tag) {
		case Atom:
			return tag_atom;
		case Var:
			if (c0.value.isString())
				return tag_string;
			if (c0.value.isNumeric())
				return tag_number;
			return tag_other;
		case Lambda:
			return tag_lambda;
		case Proc:
			return tag_proc;
		case List:
			return tag_list;
		case Dict:
			return tag_dict;
		default:
			return getAtom("unknown");
		}
	}

	void add_environment_runtime(add_environment_proc p) {
		environment_procs.push_back(p);
	}

	// define the bare minimum set of primitives necessary to pass the unit tests
	void add_globals(LithpEnvironment &env)
	{
		env["append"] = LithpCell(&proc_append);        env["head"] = LithpCell(&proc_head);
		env["tail"] = LithpCell(&proc_tail);            env["cons"] = LithpCell(&proc_cons);
		env["length"] = LithpCell(&proc_length);        env["list"] = LithpCell(&proc_list);
		env["empty"] = LithpCell(&proc_empty);          env["null?"] = LithpCell(&proc_nullp);
		env["+"] = LithpCell(&proc_add);                env["-"] = LithpCell(&proc_sub);
		env["*"] = LithpCell(&proc_mul);                env["/"] = LithpCell(&proc_div);
		env[">"] = LithpCell(&proc_greater);            env["<"] = LithpCell(&proc_less);
		env[">="] = LithpCell(&proc_greater_equal);     env["<="] = LithpCell(&proc_less_equal);
		env["="] = env["=="] = LithpCell(&proc_equal);  env["!="] = LithpCell(&proc_not_equal);
		env["!"] = LithpCell(&proc_not);
		env["debug"] = LithpCell(&proc_debug);          env["timing"] = LithpCell(&proc_timing);
		env["q"] = env["quit"] = LithpCell(&proc_quit);
		env["reds"] = LithpCell(&proc_reds);            env["tests"] = LithpCell(&proc_tests);
		env["env"] = LithpCell(&proc_env);
		env["_depth"] = LithpCell(&proc__depth);        env["_depth_max"] = LithpCell(&proc__depth_max);

		// IO
		env["print"] = LithpCell(&proc_print);
		env["getline"] = LithpCell(&proc_getline);
		env["repl"] = LithpCell(&proc_repl);
		env["_eval"] = LithpCell(&proc__eval);
		env["_eval_ctx"] = LithpCell(&proc__eval_ctx);
		env["_tokenize"] = LithpCell(&proc__tokenize);

		// File IO
		
		// String
		env["str"] = env["tostring"] = LithpCell(&proc_tostring);

		// Variable information
		env["tag"] = LithpCell(&proc_tag);
		
		// String functions
		env["expl"] = LithpCell(&proc_expl);

		// Dict functions
		env["keys"] = LithpCell(&proc_keys);
		env["values"] = LithpCell(&proc_values);

		// Add any other procs
		for (auto it = environment_procs.begin(); it != environment_procs.end(); ++it)
			(*it)(env);
	}
}
