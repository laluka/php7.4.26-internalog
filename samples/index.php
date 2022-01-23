<?php
assert_options(ASSERT_ACTIVE, 1); // On active les assertions (au cas où).

// phpinfo
phpinfo();

// popen
$handle = popen("/bin/ls", "r");
pclose($handle);

// shell_exec
shell_exec('ls');

// exec
exec("whoami");

// system
system("whoami");

// passthru
passthru("whoami");

// assert
$test = 1;
assert('$test == 1');

// preg_replace
$string = 'The quick brown fox jumps over the lazy dog.';
$patterns = array();
$patterns[0] = '/quick/';
$patterns[1] = '/brown/';
$patterns[2] = '/fox/';
$replacements = array();
$replacements[2] = 'bear';
$replacements[1] = 'black';
$replacements[0] = 'slow';
echo preg_replace($patterns, $replacements, $string);

// copy
copy("not_a_file", "not_a_file_either");

// proc_open
$descriptorspec = [STDIN, STDOUT, STDOUT];
$cmd = '"whoami"';
$proc = proc_open($cmd, $descriptorspec, $pipes);
proc_close($proc);

// pctnl_exec
// pcntl_exec("/usr/bin/whoami");

// Eval / include / require
eval ("echo \"test \\n\";");
include 'include.php';
include_once 'include.php';
require 'include.php';
require_once 'include.php';

