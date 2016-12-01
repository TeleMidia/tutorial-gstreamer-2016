eval '(exit $?0)' && eval 'exec perl "$0" ${1+"$@"}'
  & eval 'exec perl "$0" $argv:q'
    if 0;
# sc.pl -- Checks grammar and usage of input files.
my $VERSION = '2016-09-18 01:26 UTC';

use strict;
use warnings;
use Getopt::Long;
use Term::ANSIColor;

(my $ME = $0) =~ s|.*/||;

# Options.
my @sc_local = ();
my $sc_latex = 1;
my $sc_misc = 1;
my $sc_english = 1;
my $sc_portuguese = 1;
my $verbose = 0;
my $warnings = 0;

# Regexps.
# my ($re_en_adverb_frequency) =
#     qr(a\s+few\s+times|all\s+day|all\s+the\s+time|almost\s+always
#     |almost\s+never|always|at\s+times|constantly|continuously|daily
#     |every-time|every\s+Monday|every\s+hour|every\s+month|every\s+night
#     |every\s+now\s+and\s+then|every\s+third\s+day|every\s+two\s+months
#     |every\s+week|every\s+year|four\s+times|four\s+times\s+an\s+hour
#     |frequently|generally|hardly\s+ever|hourly|many\s+times|monthly
#     |most\s+times|nearly\s+always|never|normally|not\s+often
#     |now\s+and\s+then|occasionally|off\s+and\s+on|often
#     |on\s+the\s+first\s+of\s+every\s+month|once|once\s+a\s+year
#     |once\s+in\s+a\s+blue\s+moon|once\s+in\s+a\s+while|quite\s+often|rarely
#     |regularly|seldom|several\s+times|sometimes|three\s+times
#     |three\s+times\s+a\s+week|twice|twice\s+a\s+month|usually|very\s+often
#     |weekly|yearly)ix;

# my ($re_en_verb) =
#     qr(accept|act|add|admit|affect|afford|agree|aim|allow|answer|appear
#     |apply|argue|arrange|arrive|ask|attack|avoid|base|be|beat|become|begin
#     |believe|belong|break|build|burn|buy|call|can|care|carry|catch|cause
#     |change|charge|check|choose|claim|clean|clear|climb|close|collect|come
#     |commit|compare|complain|complete|concern|confirm|connect|consider
#     |consist|contact|contain|continue|contribute|control|cook|copy|correct
#     |cost|could|count|count|cover|create|cross|cry|cut|damage|dance|deal
#     |decide|deliver|demand|deny|depend|describe|design|destroy|develop|die
#     |disappear|discover|discuss|divide|do|draw|dress|drink|drive|drop|eat
#     |enable|encourage|enjoy|examine|exist|expect|experience|explain|express
#     |extend|face|fail|fall|fasten|feed|feel|fight|fill|find|finish|fit|fly
#     |fold|follow|force|forget|forgive|form|found|gain|get|give|go|grow
#     |handle|happen|hate|have|head|hear|help|hide|hit|hold|hope|hurt|identify
#     |imagine|improve|include|increase|indicate|influence|inform|intend
#     |introduce|invite|involve|join|jump|keep|kick|kill|knock|know|last|laugh
#     |lay|lead|learn|leave|lend|let|lie|like|limit|link|listen|live|look|lose
#     |love|make|manage|mark|matter|may|mean|measure|meet|mention|might|mind
#     |miss|move|must|need|notice|obtain|occur|offer|open|order|ought|own|pass
#     |pay|perform|pick|place|plan|play|point|prefer|prepare|present|press
#     |prevent|produce|promise|protect|prove|provide|publish|pull|push|put
#     |raise|reach|read|realize|receive|recognize|record|reduce|refer|reflect
#     |refuse|regard|relate|release|remain|remember|remove|repeat|replace
#     |reply|report|represent|require|rest|result|return|reveal|ring|rise|roll
#     |run|save|say|see|seem|sell|send|separate|serve|set|settle|shake|shall
#     |share|shoot|should|shout|show|shut|sing|sit|sleep|smile|sort|sound
#     |speak|stand|start|state|stay|stick|stop|study|succeed|suffer|suggest
#     |suit|supply|support|suppose|survive|take|talk|teach|tell|tend|test
#     |thank|think|throw|touch|train|travel|treat|try|turn|understand|use
#     |used\s+to|visit|vote|wait|walk|want|warn|wash|watch|wear|win|wish
#     |wonder|work|worry|would|write)ix;


sub usage ($) {
    my ($exit_code) = @_;
    my $STREAM = ($exit_code == 0 ? *STDOUT : *STDERR);
    if ($exit_code != 0) {
        print $STREAM "Try '$ME --help' for more information.\n";
    }
    else {
        print $STREAM <<EOF;
Usage: $ME [OPTIONS] [FILE]...

Check files for syntax errors.

OPTIONS:
  --sc-english     enable English checks (default yes)
  --sc-latex       enable LaTeX checks (default yes)
  --sc-local=/RE/MSG supply extra check
  --sc-misc        enable misc checks (default yes)
  --sc-portuguese  enable Portuguese checks (default yes)
  --verbose        explain check failures (default no)
  --warnings       enable warnings (default no)

  --help           display this help and exit
  --version        output version information and exit
EOF
    }
    exit $exit_code;
}


# Parse options.
{
    GetOptions (
        'sc-english!' => \$sc_english,
        'sc-latex!' => \$sc_latex,
        'sc-local=s' => \@sc_local,
        'sc-misc!' => \$sc_misc,
        'sc-portuguese!' => \$sc_portuguese,
        'verbose' => \$verbose,
        'warnings' => \$warnings,
        help => sub { usage 0 },
        version => sub { print "$ME version $VERSION\n"; exit },
        ) or usage 1;
}


# Match paragraphs.
$/ = "";

# Current line number.
my ($lineno) = 1;

# Total number of checks that failed.
my ($num_fail) = 0;

sub ck {
    my ($message) = shift;
  REGEX:
    while (my $regex = shift) {
        /$regex/ and do {
            my ($par) = $_;

            # Narrow match to line(s).
            my ($start) = index ($par, $1);
            my ($end) = $start + length ($1);
            $start = rindex (substr ($par, 0, $start), "\n") + 1;
            $end = index ($par, "\n", $end);

            my ($offset) = substr ($par, 0, $start) =~ tr/\n//;
            my ($line) = substr ($par, $start, $end - $start) . "\n";

            # Comment?
            my ($c) = substr ($line, 0, 1);
            if ($c eq '#' or $c eq '%') {
                next REGEX;
            }

            # Verbose?
            if ($verbose) {
                open my $fp, '<', $0 or die "Could not open $0: $!";
                my $reason = (grep /\Q$message/, <$fp>)[0];
                close $fp;
                foreach my $line (split /\n/, ($reason or '')) {
                    $line =~ s/^\s+|\s+$//g;
                    if (substr ($line, 0, 1) eq '#') {
                        print colored ($line, 'cyan'), "\n";
                    }
                }
            }

            # Log failure.
            my ($s) = quotemeta $1;
            my ($t) = colored ($1, 'bold red');
            $line =~ s/$s/$t/g;
            print "$ARGV:", $lineno + $offset, ": $message\n$line\n";

            $num_fail++;
        }
    }
}

while (<>) {

    ################################ Local #################################
    for my $i (0 .. $#sc_local) {
        if (not $sc_local[$i] =~ m:/(.*)/([^/]*):) {
            warn "$ME: bad syntax for --sc-local=/RE/MSG\n";
            usage 1;
        }
        my $re = $1;
        my $msg = $2;
        my $ith = $i + 1;
        $ith = ($ith == 1) ? "${ith}st"
            : ($ith == 2) ? "${ith}nd"
            : ($ith == 3) ? "${ith}rd"
            : "${ith}th";

        if (not $re =~ /\(.*\)/) {
            warn "$ME: $ith local rule has no capture";
            $re = '(' . $re . ')';
        }

        if ($msg eq '') {
            $msg = "${ith} local rule";
        }

        ck $msg, qr/$re/;
    }



    ################################ LaTeX #################################

    if ($sc_latex) {
        ck q{escape (\) or bind (~) space after non-final dot},
        qr/(\w\w+\. \S)/;

        ck q{add backslash (\) after \dots},
        qr/(\\dots[^a-zA-Z0-9,;\\{\}()\]])/;

        ck q{add nbsp before \cite},
        qr/([^~]\\cite\b)/;

        ck q{add nbsp before \dots},
        qr/(,\s+\\dots\b)/;

        ck q{add nbsp before \ref},
        qr/([^\(~]\\ref\b)/;

        ck q{add nbsp before numeral},
        qr/(\w+\s+\d+)/;

        ck q{add nbsp before math},
        qr/(\w+\s+\$.{0,5}\$)/,
        qr/(\w+\s+\$\\\w+(\{.{0,10}\})?\$)/;

        # -- The TeXbook, p. 74, Exercise 12.6.
        ck q{add \null before period},
        qr/([A-Z][.?!])[\s\)\]]/;

        ck q{add thin space (\,) within strings of initials},
        qr/([A-Z]\.[~ ][A-Z]\.)/;

        ck q{replace: \not= -> \ne},
        qr/(\\not=)/;

        ck q{replace: \ldots -> \dots},
        qr/(\\ldots)\b/;
    }


    ################################# Misc #################################

    if ($sc_misc) {

    ck q{add comma after i.e., e.g., or viz.},
        qr/((i\.e|e\.g|viz)\.[^,\\])/i;

    }


    ############################### English ################################

    if ($sc_english) {

        # Spelling.

        ck q{replace: alright -> all right},
            qr/\b(alright)\b/i;

        ck q{replace: lower-case or lower case -> lowercase},
            qr/\b(lower(-|\s+)case)\b/i;

        ck q{replace: runtime or run time -> run-time},
            qr/\b(run\s*time)\b/i;

        ck q{replace: upper-case or upper case -> uppercase},
            qr/\b(upper(-|\s+)case)\b/i;

        ck q{remove the hyphen},
            qr/\b(non-determinism|sub-module)\b/i;

        # Usage.

        ck q{remove: the},
            qr/\b(by\s+the\s+induction\s+hypothesis)\b/i;

        ck q{replace: an by a},
            qr/\b(an\s+[bcdfgjklmnpqrstvwxyz])/i;

        # Cf. Higham-N-J-1998, page 54:
        ck q{replace: by means of -> by},
            qr/\b(by\s+means\s+of)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: conduct an investigation -> investigate},
            qr/\b(conduct\s+an\s+investigation)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: due to the fact that -> since or because},
            qr/\b(due\s+to\s+the\s+fact\s*(that)?)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: firstly -> first},
            qr/\b((first|second|third)ly)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: in a position to -> can},
            qr/\b(in\s+a\s+position\s+to)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        # ck q{replace: in order to -> to},
        #     qr/\b(in\s+order\s+to)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: in the case or event that -> if},
           qr/\b(in\s+the\s+(case|event)\s*(that)?)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: in the course of -> during},
            qr/\b(in\s+the\s+course\s+of)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: in the first place -> first},
            qr/\b(in\s+the\s+(first|second|third)\s+place)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: it is apparent that -> apparently},
            qr/\b(it\s+is\s+apparent\s+that)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: it may happen that -> there may or might},
            qr/\b(it\s+may\s+happen\s+that)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: most unique -> unique},
            qr/\b(most\s+unique)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: take into consideration -> consider},
            qr/\b(take\s+into\s+consideration)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: that is to say -> in other words or that is},
            qr/\b(that\s+is\s+to\s+say)\b/i;

        # Cf. Higham-N-J-1998, page 54.
        ck q{replace: to begin with -> to begin},
            qr/\b(to\s+begin\s+with)\b/i;

        # Cf. Murphy-R-2012, Unit 129.D.
        ck q{replace: for -> to},
            qr/\b((invitation|solution|answer|reply)\s+for)\b/i;

        # Cf. Murphy-R-2012, Unit 129.A.
        ck q{replace: reason of -> reason for},
            qr/\b(reason\s+of)\b/i;

        # Cf. Murphy-R-2012, Unit 129.D.
        ck q{replace: solution of the -> solution to the},
            qr/\b(solution\s+of\s+the)\b/i;

        ck q{replace: with -> to or towards},
            qr/\b(attitude\s+with)\b/i;

        # -- http://www.ego4u.com/en/cram-up/grammar/word-order/adverb-position
        # ck q{swap the frequency adverb and the verb},
        #     qr/\b($re_en_adverb_frequency\s+(are|is|don\Wt|do\s+not))\b/i,
        #     qr/\b($re_en_verb\s+$re_en_adverb_frequency)\s+[^bB][^eE]/i;

        if ($warnings) {
            ck q{warning: parentheses is the plural of parenthesis},
                qr/\b(parenthesis|parentheses)\b/i;

            ck q{warning: especially and specially are different},
                qr/\b(especially|specially)\b/i;
        }
    }


    ############################## Portuguese ##############################

    if ($sc_portuguese) {

        # Grammar.

        ck q{replace: todos(as) -> todas(os)},
            qr/\b(todos\s+as|todas\s+os)\b/i;

        ck q{replace: alguns(umas) -> algumas(uns)},
            qr/\b(alguns\s+das|algumas\s+dos)\b/i;

        # Usage.

        # Cf. Cegalla-D-P-2009, p. 277.
        ck q{replace: em um(a) -> num(a)},
            qr/\b(em\s+uma?)\b/i;

    }

    ########################################################################

    $lineno += $_ =~ tr/\n// + !/\n\z/;
    eof and do {
        $lineno = 1;
        close ARGV;
    }
}
exit $num_fail;

# Local Variables:
# mode: perl
# eval: (add-hook 'write-file-functions 'time-stamp)
# time-stamp-start: "my $VERSION = '"
# time-stamp-format: "%:y-%02m-%02d %02H:%02M UTC"
# time-stamp-time-zone: "UTC"
# time-stamp-end: "';"
# End:
