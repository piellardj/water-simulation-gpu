#ifndef GLCHECK_HPP_INCLUDED
#define GLCHECK_HPP_INCLUDED

void gl_CheckError(const char* file, unsigned int line, const char* expression);


#ifdef DEBUG

    #define GLCHECK(expr) do { expr; gl_CheckError(__FILE__, __LINE__, #expr); } while (false)

#else


    #define GLCHECK(expr) (expr)

#endif // DEBUG

#endif // GLCHECK_HPP_INCLUDED
