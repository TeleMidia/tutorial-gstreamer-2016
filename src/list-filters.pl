eval '(exit $?0)' && eval 'exec perl "$0" ${1+"$@"}'
  & eval 'exec perl "$0" $argv:q'
    if 0;

open(L,"gst-inspect-1.0 |") or die "Failed: $!\n";
 LINE: while (<L>)
{
    /^\S+\s+([^\s:]+)/ or next LINE;
    $elt=$1;
    `gst-inspect-1.0 "$elt"` =~ m:klass\s+(filter/effect.*):i or next LINE;
    print "$1\t$elt\n";
}
