;; Entry point for standard REPL environment
(begin
	;; (timing true)
	;; (debug true)
	(define __module__ (macro () init))
	(define __init_debug true)
	(define __require_debug (# (Message) (begin
		(if (= true (get! __init_debug)) (print Message))
	)))
	(define import (# (Path)
		;; Run in topmost context
		(_eval_ctx (_tokenize (readfile (+ "lib/" (str Path) ".lisp"))))
	))
	(import "require")

	;; Import default libraries
	(require numeric)
	(require string)
	(require misc)

	(print (banner))
	(repl)
)
