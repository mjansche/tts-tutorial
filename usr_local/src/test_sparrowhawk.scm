;;;
;;; Function for testing Sparrowhawk
;;;

(define (synth_and_return_words text)
  (let ((utt (utt.synth (eval (list 'Utterance 'Text text))))
	(output ""))
    (mapcar
     (lambda (word)
       (if (not (string-equal output ""))
	   (set! output (string-append output " ")))
       (set! output (string-append output
		    (format nil "%s" (item.name word)))))
     (utt.relation.items utt 'Word))
 output))

(voice_sparrowhawk_test_null)
