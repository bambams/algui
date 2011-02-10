#ifndef ALGUI_LOG_H
#define ALGUI_LOG_H


#ifdef _DEBUG


/** printf-style logging.
    In debug mode, it writes the output in a file algui.log.
    In release mode, it does nothing.
 */
#define ALGUI_LOG            algui_log


#else


/** printf-style logging.
    In debug mode, it writes the output in a file algui.log.
    In release mode, it does nothing.
 */
#define ALGUI_LOG            1 ? 0 : algui_log


#endif //_DEBUG


/** logging function.
    It writes the output in a file algui.log.
    @param format output format string.
    @param ... parameters.
    @return non-zero if the operation succeeded, zero otherwise.
 */
int algui_log(const char *format, ...); 


#endif //ALGUI_LOG_H
