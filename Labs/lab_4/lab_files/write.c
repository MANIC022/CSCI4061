#include <errno.h>
#include <unistd.h>


// how it works?
ssize_t r_write(int fd, void *buf, size_t size) {
  char *bufp;
  size_t to_write, total_written;
  ssize_t written;

  for (bufp = buf, to_write = size, total_written = 0;
       to_write > 0; 
	    bufp += written, to_write -= written) {
    written = write(fd, bufp, to_write);
    if (written == -1) {
      if (errno == EINTR) { written = 0; }
      else { return -1; }
    }
    total_written += written;
  }
  return total_written;
}