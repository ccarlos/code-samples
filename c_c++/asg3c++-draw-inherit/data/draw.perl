#!/usr/bin/perl
# $Id: draw.perl,v 1.2 2010-01-29 18:15:03-08 - - $

use strict;
use warnings;
use POSIX qw (strftime);
use Data::Dumper;

my $script = $0;
my $date = strftime "%a %b %e %H:%M:%S %Z %Y", localtime;
my $debug = 0;

$0 =~ s|^(.*/)?([^/]+)/*$|$2|;
my $exit_status = 0;
END{ exit $exit_status; }
sub note(@){ print STDERR "$0: @_"; };
$SIG{'__WARN__'} = sub{ note @_;; $exit_status = 1; };
$SIG{'__DIE__'} = sub{ warn @_; exit; };

sub startpage ($) {
   my ($state) = @_;
   my $outfile = $state->{OUTFILE};
   print $outfile
      "%%Page: $state->{PAGENR} $state->{PAGENR}\n",
      "18 18 translate\n",
      "/Courier findfont 10 scalefont setfont\n",
      "0 0 moveto ($state->{INFILENAME}:$state->{PAGENR}) show\n";
}

sub endpage ($) {
   my ($state) = @_;
   my $outfile = $state->{OUTFILE};
   print $outfile
      "showpage\n",
      "grestoreall\n";
}

sub prolog ($) {
   my ($state) = @_;
   my $outfile = $state->{OUTFILE};
   print $outfile
      "%!PS-Adobe-3.0\n",
      "%%Creator: $script\n",
      "%%CreationDate: $date\n",
      "%%PageOrder: Ascend\n",
      "%%Orientation: Portrait\n",
      "%%SourceFile: $state->{INFILENAME}\n",
      "%%EndComments\n";
   $state->{PAGENR} = 1;
   startpage $state;
}

sub epilog ($) {
   my ($state) = @_;
   my $outfile = $state->{OUTFILE};
   endpage $state;
   print $outfile
      "%%Trailer\n",
      "%%Pages: $state->{PAGENR}\n",
      "%%EOF\n",
}

sub error ($;$) {
   my ($state, $message) = @_;
   $message = "syntax error" unless $message;
   warn "$state->{INFILENAME}: $.: $message\n";
}

sub numeric ($) {
   my ($number) = @_;
   return $number =~ m/^[+-]?(\d+\.?\d*|\.\d+)([Ee][+-]?\d+)?$/;
}

sub numbers ($$) {
   my ($count, $numbers) = @_;
   return 0 if $count != @$numbers and $count > 0;
   return ! grep {! numeric $_} @$numbers;
}

sub make_text ($$) {
   my ($state, $words) = @_;
   my $size = (numeric $words->[0]) ? (shift @$words) : 12;
   my $font = shift @$words;
   do {error $state; return} unless @$words >= 1;
   my $text = join ' ', @$words;
   $text =~ s/[(\\)]/\\$&/g;
   my $outfile = $state->{OUTFILE};
   return sub {
      my ($place) = @_;
      my ($x0, $y0, $angle) = @$place;
      map {$_ *= 72} $x0, $y0;
      $angle = 0 unless $angle;
      print $outfile
            "gsave\n",
            "   /$font findfont\n",
            "   $size scalefont setfont\n",
            "   $x0 $y0 translate\n",
            "   $angle rotate\n",
            "   0 0 moveto\n",
            "   ($text)\n",
            "   show\n",
            "grestore\n";
   }
}

sub make_ellipse ($$) {
   my ($state, $words) = @_;
   push @$words, 2 unless @$words == 3;
   do {error $state; return} unless numbers 3, $words;
   my ($height, $width, $thick) = @$words;
   map {$_ *= 72} $height, $width;
   do {error $state, "syntax error height"; return} if $height == 0;
   my ($xscale, $yscale, $radius);
   if ($height < $width) {
      $xscale = 1;
      $yscale = $height / $width;
      $radius = $width / 2;
   }else {
      $xscale = $width / $height;
      $yscale = 1;
      $radius = $height / 2;
   }
   my $outfile = $state->{OUTFILE};
   return sub {
      my ($place) = @_;
      my ($x0, $y0, $angle) = @$place;
      map {$_ *= 72} $x0, $y0, $width;
      $angle = 0 unless $angle;
      print $outfile
            "gsave\n",
            "   newpath\n",
            "   /save matrix currentmatrix def\n",
            "   $x0 $y0 translate\n",
            "   $angle rotate\n",
            "   $xscale $yscale scale\n",
            "   0 0 $radius 0 360 arc\n",
            "   save setmatrix\n",
            "   $thick setlinewidth\n",
            "   stroke\n",
            "grestore\n";
   }
}

sub make_circle ($$) {
   my ($state, $words) = @_;
   unshift @$words, $words->[0];
   return make_ellipse $state, $words;
}

sub make_polygon ($$) {
   my ($state, $words) = @_;
   do {error $state; return} unless @$words >= 2 and numbers 0, $words;
   my $thick = (@$words % 2 == 0) ? 2 : pop @$words;
   my $outfile = $state->{OUTFILE};
   return sub {
      my ($place) = @_;
      my ($x0, $y0, $angle) = @$place;
      map {$_ *= 72} $x0, $y0;
      $angle = 0 unless $angle;
      print $outfile
            "gsave\n",
            "   newpath\n",
            "   $x0 $y0 translate\n",
            "   $angle rotate\n",
            "   0 0 moveto\n";
      for (my $gon = 0; $gon < @$words; $gon += 2) {
         my ($xrel, $yrel) = @{$words}[$gon, $gon + 1];
         map {$_ *= 72} $xrel, $yrel;
         print $outfile
            "   $xrel $yrel rlineto\n";
      }
      print $outfile
            "   closepath\n",
            "   $thick setlinewidth\n",
            "   stroke\n",
            "grestore\n";
   }
}

sub make_rectangle ($$) {
   my ($state, $words) = @_;
   push @$words, 2 unless @$words == 3;
   do {error $state; return} unless numbers 3, $words;
   my ($hght, $wid, $thick) = @$words;
   return make_polygon $state, [0, $hght, $wid, 0, 0, - $hght, $thick];
}

sub make_square ($$) {
   my ($state, $words) = @_;
   unshift @$words, $words->[0];
   return make_rectangle $state, $words;
}

sub make_line ($$) {
   my ($state, $words) = @_;
   push @$words, 2 unless @$words == 2;
   do {error $state; return} unless numbers 2, $words;
   my ($length, $thick) = @$words;
   $thick = 2 unless $thick;
   return make_polygon $state, [$words->[0], 0, $thick];
}

my %objects = (
   "text" => \&make_text,
   "ellipse" => \&make_ellipse,
   "circle" => \&make_circle,
   "polygon" => \&make_polygon,
   "rectangle" => \&make_rectangle,
   "square" => \&make_square,
   "line" => \&make_line,
);

sub do_define ($$) {
   my ($state, $words) = @_;
   do {error $state; return} unless @$words > 2;
   my ($name) = shift @$words;
   my ($class) = shift @$words;
   my ($maker) = $objects{$class};
   do {error $state, "no such shape"; return} unless $maker;
   my $object = $maker->($state, $words);
   $state->{SYMTAB}->{$name} = $object if $object;
}

sub do_draw ($$) {
   my ($state, $words) = @_;
   my $object = $state->{SYMTAB}->{shift @$words};
   do {error $state, "no such object"; return} unless $object;
   push @$words, 0 if @$words == 2;
   do {error $state; return} unless numbers 3, $words;
   $object->($words);
}

sub do_newpage ($$) {
   my ($state, $words) = @_;
   do {error $state; return} unless @$words == 0;
   endpage $state;
   $state->{PAGENR}++;
   startpage $state;
}

my %commands = (
   "define" => \&do_define,
   "draw" => \&do_draw,
   "newpage" => \&do_newpage,
);

sub parsefile ($) {
   my ($state) = @_;
   prolog $state;
   my $infile = $state->{INFILE};
   my $outfile = $state->{OUTFILE};
   while (defined (my $line = <$infile>)) {
      for (;;) {
         chomp $line;
         last unless $line =~ s/\\$//;
         my $contin = <$infile>;
         $line .= $contin;
      }
      chomp $line;
      print $outfile "\n",
            "%%Command[$.]: $line\n";
      next if $line =~ m/^\s*(#|$)/;
      my @words = split " ", $line;
      my $command = $commands{shift @words};
      do {error $state; next} unless defined $command;
      $command->($state, \@words);
   }
   epilog $state;
}

sub main () {
   @ARGV and $ARGV[0] =~ m/^-D/ and $debug = shift;
   unless (@ARGV) {
      parsefile {INFILENAME => "-",
                 INFILE => *STDIN,
                 OUTFILE => *STDOUT};
   }else {
      for my $infilename (@ARGV) {
         my $outfilename = $infilename;
         $outfilename =~ s|^.*/([^/]+)/*$|$1|;
         $outfilename =~ s/(\.dr)?$/.ps/;
         open my $infile, "<$infilename"
              or warn "$infilename: $!\n" and next;
         open my $outfile, ">$outfilename"
              or warn "$outfilename: $!\n" and next;
         print "$0: $infilename => $outfilename\n";
         parsefile {INFILENAME => $infilename,
                    INFILE => $infile,
                    OUTFILE => $outfile};
         close $infile;
         close $outfile;
      }
   }
}

main;

