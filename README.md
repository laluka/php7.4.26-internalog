# Concept

- POST-client added as a macro in main/internalog.h
- POST-server added as python script in log-ingester/
- Functions instrumented to call this macro and log stuff
- Tests to be added in sample.php


# HowTo - Internalog

```bash
# Get deps
sudo apt install -y pkg-config build-essential autoconf bison re2c libxml2-dev libsqlite3-dev libreadline-dev libzip-dev libssl-dev libcurl4-openssl-dev libonig-dev libpq-dev libreadline-dev libpng-dev libjpeg-dev libfreetype-dev

# Prepare build
./buildconf --force

# For development with extensions
./configure --enable-debug --enable-mysqlnd --with-pdo-mysql --with-pdo-mysql=mysqlnd --with-pdo-pgsql=/usr/bin/pg_config --enable-bcmath --enable-fpm --with-fpm-user=www-data --with-fpm-group=www-data --enable-mbstring --enable-phpdbg --enable-shmop --enable-sockets --enable-sysvmsg --enable-sysvsem --enable-sysvshm --with-zlib --with-curl --with-pear --with-openssl --enable-pcntl --with-readline --enable-gd --with-freetype --with-jpeg --enable-intl --with-zip

# Build
make -j$(nproc)
make install

# Configure the config sample to be used as config file
sudo ln -sf "$(pwd)/internalog/internalog.conf" /etc/internalog.conf

# Run
./sapi/cli/php samples/index.php
```


# HowTo - Log Ingester

```bash
# Setup & run
poetry install

# Test & Usage
poetry run python udp-ingester.py &
./sapi/cli/php samples/index.php

# Clean before push
poetry run black .
```


# Expected result

```
$ poetry run uvicorn main:app --reload --port 5555 
INFO:     Will watch for changes in these directories: ['/opt/php7.4.26-internalog/log-ingester']
INFO:     Uvicorn running on http://127.0.0.1:5555 (Press CTRL+C to quit)
INFO:     Started reloader process [112159] using watchgod
2022-01-24 10:31:58,657.657 internalog INFO     Log ingester is UP
2022-01-24 10:31:59,310.310 internalog INFO     phpinfo()
2022-01-24 10:31:59,316.316 internalog INFO     popen("/bin/ls", "r")
2022-01-24 10:31:59,321.321 internalog INFO     shell_exec("ls")
2022-01-24 10:31:59,325.325 internalog INFO     exec("whoami")
2022-01-24 10:31:59,328.328 internalog INFO     system("whoami")
2022-01-24 10:31:59,331.331 internalog INFO     passthru("whoami")
2022-01-24 10:31:59,335.335 internalog INFO     assert("$test == 1")
2022-01-24 10:31:59,337.337 internalog INFO     preg_replace("/quick/", "/brown/", "/fox/"], ["bear", "black", "slow"], "The quick brown fox jumps over the lazy dog.")
2022-01-24 10:31:59,339.339 internalog INFO     copy("not_a_file", "not_a_file_either")
2022-01-24 10:31:59,341.341 internalog INFO     proc_open("\"whoami\"", ["zend_resource : not yet supported", "zend_resource : not yet supported", "zend_resource : not yet supported"], "unknown type")
2022-01-24 10:31:59,348.348 internalog INFO     eval("echo \"test \\n\";")
2022-01-24 10:31:59,351.351 internalog INFO     include("include.php")
2022-01-24 10:31:59,353.353 internalog INFO     require("include.php")
2022-01-24 10:31:59,354.354 internalog INFO     include_once("include.php")
2022-01-24 10:31:59,356.356 internalog INFO     require_once("include.php")
2022-01-24 10:31:59,357.357 internalog INFO     require("include.php")
2022-01-24 10:31:59,359.359 internalog INFO     require_once("include.php")
```

# Configure the logs
The cli tools will expect to find a JSON config file named `/etc/internalog.conf`

Possibles values :  
 - `ihost` : `<IP>` (a string) The IP to send UDP packages to
 - `iport` : `<PORT>` (a number) The port to send UDP packages to
 - (Optional) : `true|false` Enable/disable ILOG

**NOTE :** Since PHP is called during its own compilation, we try to read the
`MAKEFLAGS` env variable, if it exists we disable ILOG (overrides `enable` setting)

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

- [x] lalu: Faster server, less logs
- [] lalu: More samples
- [x] max: Speed boost, curl to null
- [] max: Ignore function
- [x] max: conf file
- [x] max: thread the logging process


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
- [x] preg_replace
- [x] copy
- [x] eval
- [x] include
- [x] include_once
- [x] require
- [x] require_once
- [x] proc_open // done, env not added, too painful
- [x] pcntl_exec // done, not tested, requires build flags..
- 


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

