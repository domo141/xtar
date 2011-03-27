#!/usr/bin/perl
# -*- cperl -*-

# This script reads 'codepicker.data', and picks the functions
# defined there from source files and write corresponding .c
# files with just those functions in there.
# The header files (and the skipped .c files) needs to be
# picked separately.

use strict;
use warnings;

open I, '<', 'codepicker.data' or die;

my (@sources, %funcs, %files);

while (<I>) {
    next if /^\s*$/;
    s/^\s*(.*?)\s*$/$1/;
    push(@sources, $_), next if index($_, '/') >= 0;
    $funcs{$_} = 1, next if (/^\w+$/);
    $files{$_} = 1, next; #if -f $_;
    #warn "'$_': ignored line\n";
}
close I;

sub xfork()
{
    my $pid = fork();
    die "fork() failed: $!\n" unless defined $pid;
    return $pid;
}

sub xpIopen(@)
{
    pipe I, WRITE or die "pipe() failed: $!\n";
    if (xfork) {
	# parent;
	close WRITE;
	return;
    }
    # child
    close I;
    open STDOUT, ">&WRITE" or die "dup2() failed: $!\n";
    exec @_;
    die "execve() failed: $!\n";
}
sub xpIclose()
{
    close I;
    return wait;
}


#print "@sources\n";
my @srcfiles;
foreach (@sources)
{
    xpIopen 'find', $_, '-name', '*.c';
    while (<I>) {
	chomp;
	push @srcfiles, $_;
    }
    xpIclose;
}

foreach (@srcfiles)
{
    my $filename = $_;
    s/.*\///;
    my $basename = $_;
    warn("Skipping $filename\n"), next if defined $files{$basename};
    xpIopen qw(indent -st -npro -orig -i8 -l100000), $filename;
    my @code = "// $_ //\n";
    my @flines;
    my $matchcount = 0;
    my $funcname = '';
    $. = 0;
    while (<I>) {
	if (/^{/) {
	    my $match;
	    if (defined $funcs{$funcname}) {
		$match = 1;
		$matchcount++;
		push @code, @flines, $_;
	    }
	    else {
		$match = 0;
	    }
	    #print @flines[0] if $match;
	    @flines = (); $funcname = '';
	    while (<I>) {
		push @code, $_ if $match;
		last if /^}/;
	    }
	    next;
	}
	unless (/^\w/) {
	    push @code, @flines, $_;
	    @flines = (), $funcname = '';
	    next;
	}
	push @flines, "// $filename:$.: //\n" unless @flines;
	push @flines, $_;
	$funcname = $1 if /(\w+)\(/;
    }
    xpIclose;
    next unless $matchcount;
    open O, "| indent -linux -o $basename -";
    #open O, '>', $filename;
    print O @code;
    close O;
}

__END__

open O, '| indent -linux -o lxzma.c -';

#open O, '>', 'lxzma.c' or die;
select O;
print '#include "lxzma.h"', "\n\n";
print((join ";\n", @staticprototypes), ";\n\n");
print @code;
close O;

#exec 'indent', '-linux', 'lxzma.c';
