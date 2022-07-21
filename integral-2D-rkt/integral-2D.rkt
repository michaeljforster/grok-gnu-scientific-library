#lang racket

(require gsl-integration)
(require "gsl-util.rkt")

(define key 3)
(define limit 1000)
(define epsabs 5e-3)
(define epsrel 5e-3)
(define xmin 1)
(define cpp-result 3.002117e-01)

;;; The 2-ary function to be integraged, f(x, y).
(define (f x y)
  (exp (- (+ x y))))

(define (ymin x)
  0)

(define (ymax x)
  x)

;;; Integrate fy(y) from y = ymin(x) to ymax(x) where fy(y) = f(x, y).
(define (quad1d x)
  (let ((fy (lambda (y)
              (f x y))))
    (success-result (qag fy
                         (ymin x)
                         (ymax x)
                         #:epsabs epsabs
                         #:epsrel epsrel
                         #:limit limit
                         #:key key))))

;;; Integrate f(x, y) from x = xmin to x = inifinity and y = ymin(x) to y = ymax(x).
(define (quad2d xmin)
  (success-result (qagiu quad1d
                         xmin
                         #:epsabs epsabs
                         #:epsrel epsrel
                         #:limit limit)))

(let ((result (quad2d xmin)))
  (displayln (string-append "RESULT:             " (~r result #:notation 'exponential)))
  (displayln (string-append "COMPARE CPP RESULT: " (~r cpp-result #:notation 'exponential))))
