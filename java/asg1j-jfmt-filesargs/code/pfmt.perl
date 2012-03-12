#!/usr/bin/perl
# $Id: pfmt.perl,v 1.1 2010-12-13 17:26:24-08 - - $
use strict;
use warnings;

$0 =~ s|^(.*/)?([^/]+)/*$|$2|;
$/ = "";
my $exit_status = 0;
END {exit $exit_status}
sub note(@) {print STDERR "@_"};
$SIG{'__WARN__'} = sub {note @_; $exit_status = 1};
$SIG{'__DIE__'} = sub {warn @_; exit};

my $linelen = 65;
if (@ARGV and $ARGV[0] =~ m/^-(.+)/) {
   $linelen = $1;
   die "Usage: $0 [-width] [filename...]\n" if $linelen =~ m/\D/;
   shift @ARGV
}

push @ARGV, "-" unless @ARGV;
for my $filename (@ARGV) {
   open my $file, "<$filename" or warn "$0: $filename: $!\n" and next;
   while (my @words = split " ", <$file>) {
      for (;;) {
         last unless my $line = shift @words;
         while (@words and my $newline = "$line $words[0]") {
            last if (length $newline) > $linelen;
            $line = $newline;
            shift @words;
         }
         print $line, "\n";
      }
      print "\n";
   }
   close $file;
}

