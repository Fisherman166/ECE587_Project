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

sub extract_cache_name() {
    my $logfile_name = shift;
    if( $logfile_name =~ /^\w+?_\w+?_(\w+?)_/ ) {
        return $1;
    }
    return undef;
}

sub gen_missrate_grep_string() {
    my $cache_name = shift;
    my $missrate_string = $cache_name . ".*miss_rate";
    return $missrate_string;
}

sub find_logfiles() {
    my $logfiles_path = shift;
    my $pwd = `pwd`;
    chdir $logfiles_path;
    my @logfiles = glob("*.out");
    chdir $pwd;
    return @logfiles;
}

sub extract_info_from_filename() {
    my $logfile_name = shift;
    if( $logfile_name =~ /^(\w+?)_(\w+?)_(\w+)\./ ) {
        return ($1, $2, $3);
    }
    return undef;
}

sub extract_stat_value() {
    my $stat_grep = shift;
    if( $stat_grep =~ /\S+\s+(\S+)/ ) {
        return $1;
    }
    return undef;
}

sub get_missrate_value() {
    my ($logfile_fullpath, $logfile) = @_;
    my $cache_name = &extract_cache_name($logfile);
    my $missrate_grep_string = &gen_missrate_grep_string($cache_name);
    my $missrate_grep = &my_grep($missrate_grep_string, $logfile_fullpath);
    my $missrate_value = &extract_stat_value($missrate_grep);
    return $missrate_value;
}

sub get_IPC_value() {
    my $logfile_fullpath = shift;
    my $IPC_grep = &my_grep("IPC", $logfile_fullpath);
    my $IPC_value = &extract_stat_value($IPC_grep);
    return $IPC_value;
}

sub generate_csv_lines() {
    my ($logfiles_path, @logfiles) = @_;
    my @csv_lines;

    foreach my $logfile (@logfiles) {
        my $logfile_fullpath = $logfiles_path . '/' . $logfile;
        my @cache_info = &extract_info_from_filename($logfile);

        my $benchmark = $cache_info[0];
        my $replacement_type = $cache_info[1];
        my $cache_config = $cache_info[2];
        my $missrate_value = &get_missrate_value($logfile_fullpath, $logfile);
        my $IPC_value = &get_IPC_value($logfile_fullpath);

        my $csv_line = "$benchmark,$replacement_type,$cache_config,$IPC_value,$missrate_value";
        push(@csv_lines, $csv_line);
    }
    return @csv_lines;
}

sub run_unit_tests() {
    use Test::More;
    my $test_file = "go_PLRU_dl2_1024_64_16_p_9_12.out";
    $logfiles_path = ".";
   
    my $result = &my_grep("IPC", $test_file);
    &is($result, "sim_IPC  1.1643  #  instructions  per  cycle", "Grepping for IPC");

    my $cache_name = &extract_cache_name($test_file);
    &is($cache_name, "dl2", "Extracting the cachename of interest from filename");
    my $cache_name = &extract_cache_name("bad_format.log");
    &is($cache_name, undef, "Fails to extract cachename due to bad logfile format");

    my $missrate_grep_string = &gen_missrate_grep_string( &extract_cache_name($test_file) );
    &is($missrate_grep_string, "dl2.*miss_rate", "Generate the cache miss_rate grep string");
    my $grep_result = &my_grep($missrate_grep_string, $test_file);
    &is($grep_result, "dl2.miss_rate  0.0062  #  miss  rate  (i.e.,  misses/ref)", "Grepping for missrate");

    my @logfiles = &find_logfiles($logfiles_path);
    my @expected_logfiles = ("$test_file");
    &is_deeply(\@logfiles, \@expected_logfiles, "Finding all .out logfiles to convert to csv in directory");

    my @filename_info = &extract_info_from_filename($test_file);
    my @expected_filename_info = ("go", "PLRU", "dl2_1024_64_16_p_9_12");
    &is_deeply(\@filename_info, \@expected_filename_info, "Extracting all test information from logfile name");

    my $missrate = &extract_stat_value("dl2.miss_rate  0.0062  #  miss  rate  (i.e.,  misses/ref)");
    &is($missrate, "0.0062", "Extracting the missrate value from the missrate grep");
    my $IPC_value = &extract_stat_value("sim_IPC  1.1643  #  instructions  per  cycle");
    &is($IPC_value, "1.1643", "Extracting the IPC value from the IPC grep");

    my $missrate = &get_missrate_value($logfiles_path . '/' . $test_file, $test_file);
    &is($missrate, "0.0062", "get the missrate value");

    my $IPC = &get_IPC_value($logfiles_path . '/' . $test_file, $test_file);
    &is($IPC, "1.1643", "get the IPC value");

    my @csv_lines = &generate_csv_lines($logfiles_path, @logfiles);
    my @expected_csv_lines = ("go,PLRU,dl2_1024_64_16_p_9_12,1.1643,0.0062");
    &is_deeply(\@csv_lines, \@expected_csv_lines, "Creating the actual csv lines");

    &done_testing();
}

