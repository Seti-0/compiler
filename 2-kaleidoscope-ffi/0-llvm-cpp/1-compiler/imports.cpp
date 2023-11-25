#pragma once

#include <memory>
#include <string>
#include <map>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT double printc(double X) {
  fputc((char)X, stdout);
  return 0;
}

extern "C" DLLEXPORT double printd(double X) {
  printf("%d", (int)X);
  return 0;
}

namespace builtins {
    std::map<std::string, std::shared_ptr<std::string>> map;

    void init() {
        map["test"] = std::make_shared<std::string>("42");

        // A prelude with simple utilities.
        map["pre"] = std::make_shared<std::string>(R"(
            extern sin(x)
            extern sin(y)
            extern printc(x)
            extern printd(x)

            def unary!(x) 
                if x then 0 
                else 1
            
            def unary-(x) 0-x

            def binary|5(a b) 
                if a then 1 
                else if b then 1 
                else 0
            
            def binary&6(a b) 
            if a then if b then 1;

            def binary>8(a b) b < a
            def binary~7(a b) !(a < b | b > a)
            def binary:1(a b) 0
        )");

        // Printing a snapshot of the mandlebrot set.
        map["mandel"] = std::make_shared<std::string>(R"(
            def printdensity(d)
                if d > 8 then printc(32)
                else if d > 4 then printc(46)
                else if d > 2 then printc(43)
                else printc(42)
            
            def mandelconverger(real imag iters creal cimag)
                if iters > 255 | (real*real + imag*imag > 4) then
                    iters
                else
                    mandelconverger(
                        real*real - imag*imag + creal, 
                        2*real*imag + cimag, 
                        iters+1, creal, cimag
                    )

            def mandelconverge(real imag)
                mandelconverger(real, imag, 0, real, imag)
            
            def mandelhelp(xmin xmax xstep ymin ymax ystep)
                for y=ymin, y<ymax, ystep in (
                    (for x=xmin, x<xmax, xstep in
                        printdensity(mandelconverge(x,y)) 
                    ): printc(10)
                )

            def mandel(realstart imagstart realmag imagmag)
                mandelhelp(
                    realstart, realstart+realmag*78, realmag,
                    imagstart, imagstart + imagmag*40, imagmag
                )

            def demo()
                mandel(-2.3, -1.3, 0.05, 0.07)
        )");
    }
}