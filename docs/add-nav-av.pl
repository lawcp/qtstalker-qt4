#!/usr/bin/perl 
#
#	12/2006 by loh.tar
#	This is an advanced version of add-nav.pl
#	regarding to the original:
#	- add-nav-av.list can now contain remarks
#		each line contain a # will ignored
#		leading blanks in a line are now possible
#		but NO empty line, this will stop the process
#		
#   Add navigation buttons to a list of files.
#      By Kern Sibbald 2003
#    This program is in the public domain.
#
#   This is a very simplistic program. You call it
#      ./add-nav.pl 
#   It assumes:
#      - perl is in /usr/bin/perl (if not, set a link)
#      - there is a file named add-nav.list in the current directory
#      - This list supplies the names of all the html files to which
#        you wish to add buttons.  
#      - The buttons are named back.png, next.png and home.png
#      - The buttons are added two lines before the end of the
#        html file (that means that the last two lines should be 
#           </body>
#           </html>
#        You probably will want the previous line to be:  <hr>
#      - The first file is ASSUMED to be index.html and is not included
#        in the add-nav.list file
#
#   Whew!  There are a lot of assumptions.  In addition, the output
#          when running ./add-nav.pl is not very pretty, but so what,
#          it gets the job done.
#
#   This program needs to be run only once. There after, you run it 
#     only if you add a new file (be sure to add it to add-nav.list),
#     or if you want to re-order the files. You can, of course,
#     run it as many times as you want.
#

#
# check that the file exists and is a normal file
sub chkfile {
   if ( ! -e @_[0] ) {
      die ("File ".@_[0]." does not exist.\n");
   }
   if ( ! -f @_[0] ) {
      die ( "File ".@_[0]." must be a normal file.\n");
   }
}


#
# get rid of any line containing <img src="back.png"
#                                <img src="next.png"
#                                <img src="home.png"
sub clean_file {
   @outf = ();
   chkfile(@_[0]);
   open(HTML, @_[0]);
   while (<HTML>) {
      # strip any old navigation buttons.
      if (! /\<img\ src\=\"back\.png\"/ &&  
          ! /\<img\ src\=\"next\.png\"/ &&  
          ! /\<img\ src\=\"home\.png\"/ ) {
         push @outf, $_;
      }
   }
   close(HTML);
#   printf("Cleaned @_[0]\n");
   
}

sub get_filename {
	my $n;
	$n = @navfiles[$i];
	chomp($n);
#	printf("extract filename...");
#	printf("%3d ",$i);
# == ignore lines with # 
	while ($n =~ /#/) {
	printf("ignore line: $n\n");
		$i++;
		$n = @navfiles[$i];
		chomp($n);
#		printf("%3d ",$i);
	}
# === extract filename
       if ($n =~ /([A-Za-z\.0-9]+)/){
	$n = $1;
#	printf("$n\n");
	}
	else {
# set stop token, no more in list or empty line
	$n = "#";
	}
	$i++;

	return($n);
}

#
#
# ======= begin main program ========

    @navfiles = ();
    open(NAV, "add-nav-av.list") or die("add-nav-av.list could not be opened");
    @navfiles = <NAV>;
    close(NAV);


my $lines = $#navfiles;
printf("%3d lines in add-nav-av.list\n",$lines);
    
  
my $firstfile = 1;
my $file = get_filename();
my $next;
my $prev = $file;
    
    do {
	$next = get_filename();
#	printf("Next file is $next\n");
	printf("Processing file: $file\n");
 
       clean_file($file);
       $l = $#outf;
       if ($firstfile) {
# === the first file becomes no prev button
          splice(@outf, $l-1, 0, 
"<p><a href=\"$next\" target=\"_self\"><img src=\"next.png\" border=\"0\" width=\"32\" height=\"32\" alt=\"Next\"></a>\n" .
"<a href=\"toc.html\"><img src=\"home.png\" border=\"0\" width=\"32\" height=\"32\" alt=\"Home\"></a></p>\n");
#          printf("this was first file\n");
          $firstfile--;

       } else {
          if ($next eq "#") {
# === last file to proceed, no next button
          splice(@outf, $l-1, 0, 
"<p><a href=\"$prev\" target=\"_self\"><img src=\"back.png\" border=\"0\" width=\"32\" height=\"32\" alt=\"Back\"></a>\n" .
"<a href=\"toc.html\"><img src=\"home.png\" border=\"0\" width=\"32\" height=\"32\" alt=\"Home\"></a></p>\n");
#          printf("this was the last file\n");
          } else {
          splice(@outf, $l-1, 0, 
"<p><a href=\"$prev\" target=\"_self\"><img src=\"back.png\" border=\"0\" width=\"32\" height=\"32\" alt=\"Back\"></a>\n" .
"<a href=\"$next\" target=\"_self\"><img src=\"next.png\" border=\"0\" width=\"32\" height=\"32\" alt=\"Next\"></a>\n" .
"<a href=\"toc.html\"><img src=\"home.png\" border=\"0\" width=\"32\" height=\"32\" alt=\"Home\"></a></p>\n");
		
          }
       }
#      printf("print in file: $file\n");
       open OUTF, ">$file";
       foreach $file (@outf) {
           print OUTF $file;
       }
       close OUTF;
          $prev = $file;
          $file = $next;
    } until ($next eq "#");
    if (++$lines == --$i){
    printf("--- finished normal ---\n");
	}
	else { 
	$lines -=  $i;
	printf("\n*** WARNING $lines lines left in add-nav-av.list ***\n\n");
	}
	
exit 0;   
