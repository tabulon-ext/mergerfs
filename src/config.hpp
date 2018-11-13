/*
  Copyright (c) 2016, Antonio SJ Musumeci <trapexit@spawn.link>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#pragma once

#include "branch.hpp"
#include "fusefunc.hpp"
#include "policy.hpp"

#include <fuse.h>

#include <stdint.h>
#include <sys/stat.h>

#include <string>
#include <vector>

class Config
{
public:
  struct StatFS
  {
    enum Enum
      {
        BASE,
        FULL
      };
  };

  struct StatFSIgnore
  {
    enum Enum
      {
        NONE,
        RO,
        NC
      };
  };

public:
  Config();

public:
  int set_func_policy(const std::string &fusefunc_,
                      const std::string &policy_);
  int set_category_policy(const std::string &category_,
                          const std::string &policy_);

public:
  std::string              destmount;
  Branches                 branches;
  mutable pthread_rwlock_t branches_lock;
  uint64_t                 minfreespace;
  bool                     moveonenospc;
  bool                     direct_io;
  bool                     dropcacheonclose;
  bool                     symlinkify;
  time_t                   symlinkify_timeout;
  bool                     nullrw;
  bool                     ignorepponrename;
  bool                     security_capability;
  bool                     link_cow;
  int                      xattr;
  StatFS::Enum             statfs;
  StatFSIgnore::Enum       statfs_ignore;

public:
  const Policy  *policies[FuseFunc::Enum::END];
  const Policy *&access;
  const Policy *&chmod;
  const Policy *&chown;
  const Policy *&create;
  const Policy *&getattr;
  const Policy *&getxattr;
  const Policy *&link;
  const Policy *&listxattr;
  const Policy *&mkdir;
  const Policy *&mknod;
  const Policy *&open;
  const Policy *&readlink;
  const Policy *&removexattr;
  const Policy *&rename;
  const Policy *&rmdir;
  const Policy *&setxattr;
  const Policy *&symlink;
  const Policy *&truncate;
  const Policy *&unlink;
  const Policy *&utimens;

public:
  const std::string controlfile;

public:
  static
  const
  Config &
  get(void)
  {
    const fuse_context *fc = fuse_get_context();

    return get(fc);
  }

  static
  const Config &
  get(const fuse_context *fc_)
  {
    return *((Config*)fc_->private_data);
  }

  static
  Config &
  get_writable(void)
  {
    return (*((Config*)fuse_get_context()->private_data));
  }
};
