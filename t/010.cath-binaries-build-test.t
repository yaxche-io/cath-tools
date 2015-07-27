use Test::More;

use strict;
use warnings;

use Data::Dumper;
use FindBin;
use IPC::Cmd qw/ run can_run /;

use lib $FindBin::Bin.'/lib';

use Ssap::TestEnv;

my $build_test_exe = 'build-test';
ok( can_run( $build_test_exe ), 'Can run the build-test executable' );

my( $success, $error_message, $full_buf, $stdout_buf, $stderr_buf ) = run( command => $cath_tools_build_test_exe );
ok( $success, 'Test that the build-tests all pass' );
if (!$success) {
	warn join('', @$full_buf);
}
else {
	warn "Success";
}

done_testing();
