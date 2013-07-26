#!/usr/bin/perl

#-----------------------------------------------------------
# This is a simple demonstration of the Qtstalker "ExScript" indicator.
# See the indicator misc/CUS_examples/Contributions/xley/ExScript
#
# That example indicator outputs just the "date,volume" for each bar, as well
# as the commandline argument scaleFactor, and calls this Perl script.
# The script calculates the average of all bars, gets the difference
# from average and scales it. The script then passes the data back to the
# indicator which plots it as a "Histogram Bar". It is a silly example which
# could be done directly as a CUS script, but it gives an idea of the power.
#
# For example, it could also be enhanced to access the xml summary which
# is exported using the "Tools => IndicatorSummary".
#
#-----------------------------------------------------------

$debugEnabled = 1;

if ($debugEnabled) {
  $outFn="$ENV{'HOME'}/.qtstalker/ExScript-out.txt"; # needs absolute pathname
  # Note: That output file is not used. It is just for debugging purposes.
  open (OUT, ">$outFn");
  print OUT "Echo input ...\n";
}
$arg1 = shift;
$scaleFactor = $arg1;
$arg2 = shift;
if ($debugEnabled) {
  print OUT "  arg1=$arg1\n";
  print OUT "  arg2=$arg2\n";
}
if ($debugEnabled) { print OUT "stdin:\n"; }
while (<STDIN>) {
  chomp;
  if ($debugEnabled) {
    $lineCount++;
    print OUT "$lineCount: $_\n";
  }
  ($date, $volume) = split(",");
  $volumeSum += $volume;
  # Store the data so that we can use it again
  push(@bars, $_);
}

$volumeAverage = $volumeSum / $lineCount;

# Get % difference and prepare the output.
$output = "";
foreach $bar (@bars) {
  ($date, $volume) = split(",", $bar);
  $volumeDiff = ($volume - $volumeAverage) / $volumeAverage * 100 * $scaleFactor;
  if ($debugEnabled) { print OUT "$date,$volume,$volumeDiff\n"; }
  $output .= sprintf("%.1f", $volumeDiff) . ",";
}

# Send the output
chop($output); # Strip the trailing comma.
if ($debugEnabled) { print OUT "\noutput:\n$output\n"; }
print $output;

if ($debugEnabled) {
  print OUT qq!
lineCount=$lineCount
volumeAverage=$volumeAverage
scaleFactor=$scaleFactor
arg2=$arg2
!;
  close OUT;
}
