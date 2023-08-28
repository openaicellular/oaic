#ifndef RIC_TIMER_QUEUE_H
#define RIC_TIMER_QUEUE_H

#include <queue>
#include <map>
#include <ctime>

#include <sys/time.h>
#include "pthread.h"

namespace ric {

class timer_queue
{
public:

  typedef void *(*timer_callback_t)(int timer_id,void *arg);

  typedef struct timer {
    int id;
    bool repeats;
    bool canceled;
    struct timeval next;
    struct timeval interval;
    timer_callback_t callback;
    void *arg;
  } timer_t;

  class timer_comparator
  {
  public:
    bool operator() (const timer_t *l,const timer_t *r) const
    {
      if (l->next.tv_sec > r->next.tv_sec
	  || (l->next.tv_sec == r->next.tv_sec
	      && l->next.tv_usec > r->next.tv_usec))
	return true;
      return false;
    }
  };

  timer_queue()
    : thread(0), running(false), next_id(0), cond(PTHREAD_COND_INITIALIZER),
      lock(PTHREAD_MUTEX_INITIALIZER) {};
  virtual ~timer_queue() { stop(); };
  bool start();
  void stop();
  static void *run(void *arg);
  int insert_periodic(const struct timeval &interval,
		      timer_callback_t callback,void *arg);
  void cancel(int id);

private:
  std::priority_queue<timer_t *,std::vector<timer_t *>,timer_comparator> queue;
  std::map<int,timer_t *> timer_map;
  pthread_t thread;
  pthread_cond_t cond;
  pthread_mutex_t lock;
  bool running;
  int next_id;
};

}

#endif /* RIC_TIMER_QUEUE_H */
