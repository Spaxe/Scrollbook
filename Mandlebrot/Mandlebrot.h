/**
  Xavier Ho, s2674674
  contact@xavierho.com

  For a point C(r, i) on the complex plane where
     -2.5 < r < 1
     -1   < i < 1
  the Mandlebrot fractal is defined by the following algorithm:
     count = 0
     Z = 0     (Z is a complex number)
     while |Z|<2 and count < some limit:
       Z = Z^2 + C
       count++
     if |Z| < 2:
       return 0
     else:
       return count      (intensity of the pixel)
 */