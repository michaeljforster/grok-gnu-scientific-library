#lang racket

(provide success-result)

(define (success-result result)
  "The functions will always return a list. First element is status code. Success when code = 0, otherwise error."
  (match result
    ((list code val estimated-error)
     #:when (= 0 code) val)
    ((list codenr gsl-symbol message)
     #:when (not (= 0 codenr))
     (raise-result-error 'quad1d "0" codenr))))
