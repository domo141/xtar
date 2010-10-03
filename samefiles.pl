#!/usr/bin/perl

# A tool to find out which files from 'dir2' were copied verbatim to 
# 'dir1'. With this tool one doesn't need to bookkeep original files...
# First used in this project to see what 'libarchive-2.7.0' files
# were used (in order to replace with 'libarchive-2.8.4' files).
# If 'dir3' defined, try to find same file and (xx)diff it.

use strict;
use warnings;

die "Usage: $0 dir1 dir2 [dir3]\n" unless defined $ARGV[1];

die "$ARGV[0]: not a directory\n" unless -d $ARGV[0];
die "$ARGV[1]: not a directory\n" unless -d $ARGV[1];

my %hash;

print "\n";
open P, "find $ARGV[0] -type f | xargs md5sum |";
while (<P>) {
    /(\S+)\s+(\S+)/; # XXX no whitespaces in paths !!!

    $hash{$1} = $2;
}
close P;

my %hash2 = %hash;
my $sl = length $ARGV[1];

open P, "find $ARGV[1] -type f | xargs md5sum |";
while (<P>) {
    /(\S+)\s+(\S+)/; # XXX no whitespaces in paths !!!

    my $m = $hash{$1};
    if (defined $m) {
	print "$m\n$2\n\n";
	delete $hash2{$1};
	if (defined $ARGV[2]) {
	    my $fd = substr $2, $sl;
	    system 'xxdiff', $2, "$ARGV[2]/$fd";
	}
    }
}
close P;

if (keys %hash2 > 0) {
    print "No matches:\n";
    print " $_\n" foreach (sort values %hash2);
}



