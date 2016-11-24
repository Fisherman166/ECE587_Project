#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;

my $logfiles_path = undef;
my $run_unittests = undef;
my $help = undef;

&parse_cmdline();

if(defined $run_unittests) {
    &run_unit_tests();
}
else {
    &main();
}
exit(0);

sub main() {
    return 0;
}

###############################################################################
### Functions
###############################################################################
sub parse_cmdline() {
    GetOptions(
        "logs|l=s"      => \$logfiles_path,
        "unittest|u"    => \$run_unittests,
        "help|h"        => \$help,
    );

    if( (not defined $run_unittests) and (not defined $logfiles_path) ) {
        die "ERROR: No logfile path provided with the -logs or -l command\n";
    }
    if(defined $help) {
        print "-log or -l = Path to directiories containing the logfiles to convert into csv - REQUIRED\n";
        print "-unittest or -u = Run the unittests\n";
        print "-help or -h = Print the help information\n";
        exit(0);
    }
}

sub my_grep() {
    my ($grep_string, $file_to_grep) = @_;
    my $grep_result = `grep "$grep_string" $file_to_grep | column -t`;
    chomp($grep_result);
    return $grep_result;
}

sub gen_missrate_grep_string() {
    return 0;
}

sub run_unit_tests() {
    use Test::More;
    my $test_file = "go_PLRU_dl2_1024_64_16_p_9_12.out";
   
    my $result = &my_grep("IPC", $test_file);
    &is($result, "sim_IPC  1.1643  #  instructions  per  cycle", "Grepping for IPC");

    my $missrate_grep_string = &gen_missrate_grep_string();
    &is($missrate_grep_string, "dl2.*miss_rate", "Generate the cache miss_rate grep string");
    my $result = &my_grep("dl2.*miss_rate", $test_file);
    &is($result, "sim_IPC  1.1643  #  instructions  per  cycle", "Grepping for IPC");

    &done_testing();
}
