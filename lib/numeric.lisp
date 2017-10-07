(begin
	(require lists)
	(require misc)

	((# None (begin
		(define __module__ (macro () numeric))
		(define % (# (A B) (- A (* B (/ A B)))))
		(define abs (# (N) ((if (> N 0) + -) 0 N)))

		(define sum (# (Lst) (foldl Lst 0 (# (N Acc) (+ N Acc)))))
		(define prod (# (Lst) (foldl Lst 1 (# (N Acc) (* Acc N)))))

		;; The factorial function
		(define fac (# (N) (begin
			;; This should tail recurse
			(fac2 N 1)
		)))
		;; This is the tail recursive part
		(define fac2 (# (N A) (begin
			(if (<= N 0) A (fac2 (- N 1) (* N A)))
		)))

		(define alter (macro (Variable Op Count)
			(_eval
				(list set! Variable
					(list Op (list get! Variable) (default Count 1))
				)
			)
		))

		;; increment and return given variable by Count (or 1 if not defined)
		(define incr (macro (Variable Count)
			(_eval (list alter Variable + (default Count 1)))
		))

		;; decrement and return given variable by Count (or 1 if not defined)
		(define decr (macro (Variable Count)
			(_eval (list alter Variable - (default Count 1)))
		))

		;; Export our functions to the global namespace
		(export % abs sum prod fac alter incr decr)
	)))

)
