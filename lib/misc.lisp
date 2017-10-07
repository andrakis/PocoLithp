(begin
	(define __module__ (macro () misc))
	(define compose (# (F G) (# (X) (F (G X)))))
	(define repeat (# (F) (compose F F)))
	;; Lookup a function or return a stub
	(define lookup (# (Symbol)
		;; Use eval to get Symbol's value at runtime
		(if (_eval (list defined Symbol))  ;; If symbol present
			(_eval (list get! Symbol))     ;; get symbol
		    (# () stub))                   ;; else return a stub
	))

	;; Alias a function to another function, by defining the new name as the value
	;; of the old name. This copies the pointing function, allowing you to redefine
	;; the old name to something else, and calling the new name will use the old behaviour.
	(define alias (macro (OldName NewName) (_eval_ctx (list define NewName (list get! OldName)))))

	;; A macro that checks if Name is defined, and returns its value
	;; or the atom none. Can be used to supply default values for parameters.
	(define default (macro (Name Default) (begin
		;; Construct a manual AST of: (if (defined Name) (get! Name) none)
		(_eval (list if (list defined Name) (list get! Name)
			;; and also check if a default was provided to us
			(if (defined Default) Default none)))
	)))
	(alias default ifDefined)
	;; A tuple
	(define tuple (# (A B)
		(list (ifDefined A) (ifDefined B))
	))

	;; A comment macro. This is not evaluated at runtime, thus doing nothing.
	;; However, its arguments are converted to a string, effectively storing a comment
	;; in code.
	(define %% (macro Args (str Args)))

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
