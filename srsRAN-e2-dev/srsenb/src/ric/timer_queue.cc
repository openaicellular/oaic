
#include "srsenb/hdr/ric/timer_queue.h"

namespace ric {

void timer_queue::stop()
{
  pthread_mutex_lock(&lock);
  if (!running) {
    pthread_mutex_unlock(&lock);
    return;
  }
  running = false;
  while (queue.size() > 0)
    queue.pop();
  pthread_mutex_unlock(&lock);
  pthread_cond_signal(&cond);
  pthread_join(thread,NULL);
}

bool timer_queue::start()
{
  pthread_mutex_lock(&lock);
  if (running) {
    pthread_mutex_unlock(&lock);
    return false;
  }
  running = true;
  if (pthread_create(&thread,NULL,timer_queue::run,this) != 0) {
    running = false;
    pthread_mutex_unlock(&lock);
    return false;
  }
  pthread_mutex_unlock(&lock);

  return true;
}

void *timer_queue::run(void *arg)
{
  timer_queue *tq = (timer_queue *)arg;

  pthread_setname_np(pthread_self(),"KPM_TIMER_QUEUE");

  pthread_mutex_lock(&tq->lock);
  while (tq->running) {
    if (tq->queue.size() == 0) {
      pthread_cond_wait(&tq->cond,&tq->lock);
      continue;
    }
    struct timeval now;
    gettimeofday(&now,NULL);
    timer_t *t = tq->queue.top();
    while (t && timercmp(&t->next,&now,<=)) {
      if (t->canceled) {
	delete t;
	tq->queue.pop();
	t = tq->queue.top();
	continue;
      }
      if (t->repeats) {
	struct timeval res;
	timeradd(&t->next,&t->interval,&res);
	t->next = res;
	tq->queue.push(t);
      }
      t->callback(t->id,t->arg);
      if (!t->repeats)
	delete t;
      tq->queue.pop();
      t = tq->queue.top();
    }
    if (t) {
	struct timeval wtv;
	timersub(&t->next,&now,&wtv);
	struct timespec ts = { wtv.tv_sec, wtv.tv_usec * 1000 };
	pthread_cond_timedwait(&tq->cond,&tq->lock,&ts);
    }
  }
  pthread_mutex_unlock(&tq->lock);
  return NULL;
}

int timer_queue::insert_periodic(const struct timeval &interval,
				 timer_callback_t callback,void *arg)
{
  timer_t *t;
  struct timeval now;
  int timer_id;

  gettimeofday(&now,NULL);
  t = new timer_t{};  
  t->repeats = true;
  t->canceled = false;
  t->interval = interval;
  t->callback = callback;
  t->arg = arg;
  timeradd(&now,&interval,&t->next);
  pthread_mutex_lock(&lock);
  timer_id = t->id = ++next_id;
  queue.push(t);
  pthread_mutex_unlock(&lock);
  pthread_cond_signal(&cond);

  return timer_id;
}

void timer_queue::cancel(int id)
{
  timer_t *t;

  pthread_mutex_lock(&lock);
  t = timer_map[id];
  if (!t)
    return;
  t->canceled = true;
  t->callback = NULL;
  t->arg = NULL;
  pthread_mutex_unlock(&lock);
}

}
