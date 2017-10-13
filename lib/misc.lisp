(begin
	(define __module__ (macro () misc))
	(define compose (# (F G) (# (X) (F (G X)))))
	(define repeat (# (F) (compose F F)))
	;; reverse head
	(define rhead (# (List) (head (reverse List))))
	;; reverse tail
	(define rtail (# (List) (tail (reverse List))))
	;; Call Callback with given Arguments
	(define apply (# (Callback Arguments) ((+ (list Callback) Arguments))))
	;; Passed in: Value Value .. Callback
	(define with (macro Values (begin
		;; (Callback Val1 Val2 Val3 ..)
		(apply (rhead Values) (reverse (rtail Values))))))
	;; (define with (macro (Value Callback) (_eval (list Callback Value))))
	(define dbg (macro (Exp)
		;; TODO: (with (debug true) (_eval Exp) (# (DbgPrev Result) ...))
		(with (debug true) (_eval Exp) (# (DbgPrev Result) (begin
				(debug DbgPrev)
				(get! Result)
			))
		)
	))
	;; Lookup a function or return a stub
	(define lookup (# (Symbol)
		;; Use eval to get Symbol's value at runtime
		(if (_eval (list defined Symbol))  ;; If symbol present
			(_eval (list get! Symbol))     ;; get symbol
		    (# () stub))                   ;; else return a stub
	))

	;; A macro that checks if Name is defined, and returns its value
	;; or the atom none. Can be used to supply default values for parameters.
	(define default (macro (Name Default) (begin
		;; Construct a manual AST of: (if (defined Name) (get! Name) none)
		(_eval (list if (list defined Name) (list get! Name)
			;; and also check if a default was provided to us
			(if (defined Default) Default none)))
	)))
	;; A tuple
	(define tuple (# (A B)
		(list (ifDefined A) (ifDefined B))
	))

	;; Export a function from the current namespace to the global namespace.
	;; Can also be used to alias functions.
	(define alias (macro (Function Name)
		(_eval_ctx (list define Name (_eval (list get! Function))))))
	(define export (macro Symbols (begin
		(each Symbols (# (Symbol)
			(_eval_ctx (list define Symbol (_eval (list get! Symbol))))))
	)))
	(alias default ifDefined)

	;; A comment macro. This is not evaluated at runtime, thus doing nothing.
	;; However, its arguments are converted to a string, effectively storing a comment
	;; in code.
	(define %% (macro Args (str Args)))

	(define while (# (Condition Body) (begin
		(if (Condition) (begin
			Body
			(while Condition Body)
		))
	)))

	;; Print out all environment items using str
	(define envstr (# (Mode Env) (begin
		(if (defined Env) (begin
			(if (= list (tag Env))
				(begin
					(each (flatten Env) (# (Ele) (envstr Mode Ele)))
				) ;; else
				(begin
					(each (keys Env) (# (Func) (begin
						(print Func "=>" (_eval (list str (list get! Func) Mode)))
					)))
				)
			)
		) ;; else
		(begin
			(envstr (default Mode false) (env))
		))
	)))

)
