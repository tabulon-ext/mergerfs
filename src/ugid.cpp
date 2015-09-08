/*
   The MIT License (MIT)

   Copyright (c) 2014 Antonio SJ Musumeci <trapexit@spawn.link>

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

#include <vector>
#include <map>

typedef std::vector<gid_t> gid_t_vector;
typedef std::map<uid_t,gid_t_vector> gid_t_cache;

#if defined __linux__ and UGID_USE_RWLOCK == 0
#include "ugid_linux.ipp"
#else
#include "ugid_rwlock.ipp"
#endif

namespace mergerfs
{
  namespace ugid
  {
    static
    inline
    void
    prime_cache(const uid_t   uid,
                const gid_t   gid,
                gid_t_vector &gidlist)
    {
      int rv;
      char buf[4096];
      struct passwd pwd;
      struct passwd *pwdrv;

      rv = getpwuid_r(uid,&pwd,buf,sizeof(buf),&pwdrv);
      if(pwdrv != NULL && rv == 0)
        {
          int count;

          count = 0;
          rv = ::getgrouplist(pwd.pw_name,gid,NULL,&count);
          gidlist.resize(count);
          rv = ::getgrouplist(pwd.pw_name,gid,&gidlist[0],&count);
          if(rv == -1)
            gidlist.resize(1,gid);
        }
    }

    void
    initgroups(const uid_t uid,
               const gid_t gid)
    {
      static __thread gid_t_cache cache;

      gid_t_vector &gidlist = cache[uid];
      if(gidlist.empty())
        prime_cache(uid,gid,gidlist);

      setgroups(gidlist);
    }
  }
}
