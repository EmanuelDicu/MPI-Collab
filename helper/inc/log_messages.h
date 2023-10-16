#ifndef LOG_MESSAGES_H
#define LOG_MESSAGES_H

void set_rank(int rank);
void log_message(char message[]);
void log_send_message(int source, int destination);
void log_recv_message(int source, int destination);

#endif // LOG_MESSAGES_H
