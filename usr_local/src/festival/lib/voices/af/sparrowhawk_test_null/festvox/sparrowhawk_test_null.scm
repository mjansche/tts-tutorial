;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;-*-mode:scheme-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  A frontend configuration to test Sparrowhawk
;;;
;;;  Rob Clark
;;;

(defvar sparrowhawk_prefix "/usr/local/share/sparrowhawk/af/")

(defvar sparrowhawk_config_file "sparrowhawk_configuration.ascii_proto")

(require 'radio_phones)
(require_module 'UniSyn)

; a null utterance function
(define (pass_utt utt)
utt)

(define (voice_sparrowhawk_test_null:reset)
 (sparrowhawk.close 'voice_sparrowhawk_test_null))

(define (voice_sparrowhawk_test_null)
"(voice_sparrowhawk_test_null)
 Set up the current voice to be Afrikaans, but just a front end"
  ;; Phone set
  (voice_reset)
  (Parameter.set 'Language 'afrikaans)
  (require 'radio_phones)
  (Parameter.set 'PhoneSet 'radio)
  (PhoneSet.select 'radio)
  (set! token_pos_cart_trees nil)
  (Parameter.set 'Token_Method Sparrowhawk_normalize)
  (sparrowhawk.init 'sparrowhawk_test_null 
		     sparrowhawk_config_file 
		     sparrowhawk_prefix)
  (Parameter.set 'POS_Method pass_utt)
  (Parameter.set 'Phrase_Method pass_utt)  
  (Parameter.set 'Phrasify_Method pass_utt)
  (Parameter.set 'Word_Method pass_utt)
  (Parameter.set 'Pause_Method pass_utt)
  (Parameter.set 'Intonation_Method pass_utt)
  (Parameter.set 'PostLex_Method pass_utt)
  (Parameter.set 'Int_Method pass_utt)
  (Parameter.set 'Duration_Method pass_utt)
  (Parameter.set 'Synth_Method 'None)
  (set! current-voice 'sparrowhawk_test_null)
  (set! current_voice_reset voice_sparrowhawk_test_null:reset)
t
)

(proclaim_voice
 'sparrowhawk_test_null
 '((language afrikaans)
   (gender female)
   (dialect afrikaans)
   (description
    "This is a Sparrowhawk test frontend")))



(provide 'sparrowhawk_test_null)

