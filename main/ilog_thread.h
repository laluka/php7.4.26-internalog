#ifndef __ILOG_THREAD_H__
#define __ILOG_THREAD_H__

/**
 * @brief Log the given message, lock-free and thread safe
 * 
 * @param msg The message to log, will be sent asynchronously by the ILOG thread
 */
void log_msg(char* msg);


/**
 * @brief Start the ILOG thread then return
 */
void init_ilog_thread();

/**
 * @brief Signal the ILOG thread it should terminate, then wait for it
 * 
 * @note The ILOG thread will wait for the msg queue to be empty before
 * terminating, this way we make sure we don't miss some function call
 */
void join_ilog_thread();


#endif //__ILOG_THREAD_H__