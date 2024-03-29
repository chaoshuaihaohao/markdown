  #include <fcntl.h>	//open
  #include <string.h>	//memset\strncpy
#include <unistd.h>	//close

  #include <sys/ioctl.h>	//ioctl
  #include <linux/if.h>
  #include <linux/if_tun.h>

  #include <linux/if.h>
  #include <linux/if_tun.h>

  int tun_alloc(char *dev)
  {
      struct ifreq ifr;
      int fd, err;

      if( (fd = open("/dev/net/tun", O_RDWR)) < 0 )
//         return tun_alloc_old(dev);
	return -1;

      memset(&ifr, 0, sizeof(ifr));

      /* Flags: IFF_TUN   - TUN device (no Ethernet headers) 
       *        IFF_TAP   - TAP device  
       *
       *        IFF_NO_PI - Do not provide packet information  
       */ 
      //ifr.ifr_flags = IFF_TUN; //根据flags，表示使用tun驱动。
      ifr.ifr_flags = IFF_TAP; //根据flags，表示使用tun驱动。
      if( *dev )
         strncpy(ifr.ifr_name, dev, IFNAMSIZ);

      if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
         close(fd);
         return err;
      }
//      strcpy(dev, ifr.ifr_name);
      return fd;
  }    


  int tun_alloc_mq(char *dev, int queues, int *fds)
  {
      struct ifreq ifr;
      int fd, err, i;

      if (!dev)
          return -1;

      memset(&ifr, 0, sizeof(ifr));
      /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
       *        IFF_TAP   - TAP device
       *
       *        IFF_NO_PI - Do not provide packet information
       *        IFF_MULTI_QUEUE - Create a queue of multiqueue device
       */
      ifr.ifr_flags = IFF_TAP | IFF_NO_PI | IFF_MULTI_QUEUE;//使用tap驱动，多队列
      strcpy(ifr.ifr_name, dev);

      for (i = 0; i < queues; i++) {
          if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
             goto err;
          err = ioctl(fd, TUNSETIFF, (void *)&ifr);
          if (err) {
             close(fd);
             goto err;
          }
          fds[i] = fd;
      }

      return 0;
  err:
      for (--i; i >= 0; i--)
          close(fds[i]);
      return err;
  }


  int tun_set_queue(int fd, int enable)
  {
      struct ifreq ifr;

      memset(&ifr, 0, sizeof(ifr));

      if (enable)
         ifr.ifr_flags = IFF_ATTACH_QUEUE;
      else
         ifr.ifr_flags = IFF_DETACH_QUEUE;

      return ioctl(fd, TUNSETQUEUE, (void *)&ifr);
  }


  int main()
  {
	tun_alloc("test-hao");
	pause();
}
