typedef struct _rwlock_t rwlock_t;

void rwlock_init(rwlock_t* l);
void rwlock_rdlock(rwlock_t* l);
void rwlock_rdunlock(rwlock_t* l);
void rwlock_unlock(rwlock_t* l);
void rwlock_destroy(rwlock_t* l);
