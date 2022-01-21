# Concept

- POST-client added as a macro in main/internalog.h
- POST-server added as python script in log-ingester/
- Functions instrumented to call this macro and log stuff
- Tests to be added in php.php


# HowTo

```bash
# Get deps
sudo apt install -y pkg-config build-essential autoconf bison re2c libxml2-dev libsqlite3-dev libreadline-dev libzip-dev libssl-dev libcurl4-openssl-dev libonig-dev libpq-dev libreadline-dev libpng-dev libjpeg-dev libfreetype-dev

# Prepare build
./buildconf --force

# For development
./configure --enable-debug

# For development with extensions
./configure --enable-debug --enable-mysqlnd --with-pdo-mysql --with-pdo-mysql=mysqlnd --with-pdo-pgsql=/usr/bin/pg_config --enable-bcmath --enable-fpm --with-fpm-user=www-data --with-fpm-group=www-data --enable-mbstring --enable-phpdbg --enable-shmop --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --enable-zip --with-zlib --with-curl --with-pear --with-openssl --enable-pcntl --with-readline --enable-gd --with-freetype --with-jpeg
# php-7 : --enable-zip instead of --with-zip

# Build
make -j$(nproc)

# Start the log server - once
nc -lnvkup 8888
# Start the log server - loop
while true; do clear; timeout 3 nc -lnvkup 8888; done

# Run
./sapi/cli/php php.php
```


# Add functions logs

1. Look for `PHP_FUNCTION(<FCT_NAME>)`
2. Just at the beginning of the function add `ILOG_FUNCTION("<FCT_NAME>")`
3. Build and run, add your test to php.php

Example : `ext/standard.c`
```c
PHP_FUNCTION(assert)
{
  ILOG_FUNCTION("assert") // Log macro

  [...] // Original code
}
```


# TODO

## Make it threadsafe

Add a lock in the udp client


## Functions to instrument

https://gist.github.com/mccabe615/b0907514d34b2de088c4996933ea1720


## Needs more research

Aka can't find with `PHP_FUNCTION(...`:

- [ ] call_user_func
- [ ] call_user_func_array
- [ ] create_function
- [ ] PHPExecute


## Batch 1

- [x] popen
- [x] shell_exec
- [x] exec
- [x] system
- [x] passthru
- [x] phpinfo
- [x] assert
- [x] copy
- [x] eval
- [x] include
- [x] include_once
- [x] require
- [x] require_once
- [x] proc_open // done, env not added, too painful
- [x] pcntl_exec // done, not tested, requires build flags..


## Batch 2

- [ ] array_diff_uassoc
- [ ] array_diff_ukey
- [ ] array_filter
- [ ] array_intersect_uassoc
- [ ] array_intersect_ukey
- [ ] array_map
- [ ] array_reduce
- [ ] array_udiff
- [ ] array_udiff_assoc
- [ ] array_udiff_uassoc
- [ ] array_uintersect
- [ ] array_uintersect_assoc
- [ ] array_uintersect_uassoc
- [ ] array_walk
- [ ] array_walk_recursive
- [ ] assert_options
- [ ] backtickoperator
- [ ] bzopen
- [ ] chgrp
- [ ] chmod
- [ ] chown
- [ ] disk_free_space
- [ ] disk_total_space
- [ ] diskfreespace
- [ ] exif_imagetype
- [ ] exif_read_data
- [ ] exif_thumbnail
- [ ] extract
- [ ] file
- [ ] file_exists
- [ ] file_get_contents
- [ ] file_put_contents
- [ ] fileatime
- [ ] filectime
- [ ] filegroup
- [ ] fileinode
- [ ] filemtime
- [ ] fileowner
- [ ] fileperms
- [ ] filesize
- [ ] filetype
- [ ] fopen
- [ ] fsockopen
- [ ] ftp_get
- [ ] ftp_nb_get
- [ ] ftp_nb_put
- [ ] ftp_put
- [ ] get_cfg_var
- [ ] get_current_user
- [ ] get_meta_tags
- [ ] getcwd
- [ ] getenv
- [ ] getimagesize
- [ ] getlastmo
- [ ] getmygid
- [ ] getmyinode
- [ ] getmypid
- [ ] getmyuid
- [ ] glob
- [ ] gzfile
- [ ] gzopen
- [ ] hash_file
- [ ] hash_hmac_file
- [ ] hash_update_file
- [ ] header
- [ ] highlight_file
- [ ] image2wbmp
- [ ] imagecreatefromgif
- [ ] imagecreatefromjpeg
- [ ] imagecreatefrompng
- [ ] imagecreatefromwbmp
- [ ] imagecreatefromxbm
- [ ] imagecreatefromxpm
- [ ] imagegd
- [ ] imagegd2
- [ ] imagegif
- [ ] imagejpeg
- [ ] imagepng
- [ ] imagewbmp
- [ ] imagexbm
- [ ] ini_set
- [ ] invoke
- [ ] iptcembed
- [ ] is_dir
- [ ] is_executable
- [ ] is_file
- [ ] is_link
- [ ] is_readable
- [ ] is_uploaded_file
- [ ] is_writable
- [ ] is_writeable
- [ ] iterator_apply
- [ ] lchgrp
- [ ] lchown
- [ ] link
- [ ] linkinfo
- [ ] lstat
- [ ] mail
- [ ] md5_file
- [ ] mkdir
- [ ] move_uploaded_file
- [ ] ob_start
- [ ] Other
- [ ] parse_ini_file
- [ ] parse_str
- [ ] pathinfo
- [ ] pfsockopen
- [ ] php_strip_whitespace
- [ ] posix_getlogin
- [ ] posix_kill
- [ ] posix_mkfifo
- [ ] posix_setpgid
- [ ] posix_setsid
- [ ] posix_setuid
- [ ] posix_ttyname
- [ ] preg_replace
- [ ] preg_replace_callback
- [ ] proc_close
- [ ] proc_get_status
- [ ] proc_nice
- [ ] proc_terminate
- [ ] putenv
- [ ] read_exif_data
- [ ] readfile
- [ ] readgzfile
- [ ] readlink
- [ ] realpath
- [ ] register_shutdown_function
- [ ] register_tick_function
- [ ] rename
- [ ] rmdir
- [ ] session_set_save_handler
- [ ] set_error_handler
- [ ] set_exception_handler
- [ ] sha1_file
- [ ] show_source
- [ ] spl_autoload_register
- [ ] sqlite_create_aggregate
- [ ] sqlite_create_function
- [ ] stat
- [ ] symlink
- [ ] tempnam
- [ ] tmpfile
- [ ] touch
- [ ] uasort
- [ ] uksort
- [ ] unlink
- [ ] usort

