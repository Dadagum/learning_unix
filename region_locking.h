#infdef REGION_LOCKING_H
#define REGION_LOCKING_H

/* Lock a file region using nonblocking F_SETLK */
int lockRegion(int fd, int type, int whence, int start, int len);
/* Lock a file region using blocking F_SETLKW */
int lockRegionWait(int fd, int type, int whence, int start, int len);
pid_t regionIsLocked(int fd, int type, int whence, int start, int len);

#endif