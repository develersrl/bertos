#!/usr/bin/perl -w
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#  Based on checkpatch.pl from Linux
#
#  (c) 2010, Andrea Righi <arighi@develer.com>

use strict;

my $count = 0;
my $error = 0;
my $warn = 0;

while (my $line = <STDIN>) {
	chomp($line);
	$count++;

#trailing whitespace or DOS ^M
	if ($line =~ /^.*\015/) {
		ERROR("DOS line endings\n" . $line, $count);
	} elsif ($line =~ /^.*\S\s+$/ || $line =~ /^\s+$/) {
		WARN("trailing whitespace\n" . $line, $count);
	}
# at the beginning of a line any tabs must come first and anything
# more than 8 must use tabs.
	if ($line =~ /^\+\s* \t\s*\S/ ||
			$line =~ /^\+\s*        \s*/) {
		ERROR("code indent should use tabs where possible\n"
			. $line, $count);
	}
# check for RCS/CVS revision markers
	if ($line =~ /^\+.*\$(Revision|Log|Id)(?:\$|)/) {
		WARN("CVS style keyword markers should be removed\n"
			. $line, $count);
	}
# check for braces in loops, conditions, etc.
	if ($line =~ /.*(for|while|if|else|switch|struct|enum|union).*{/ &&
			$line !~ /#define/) {
		ERROR("if/while/etc brace must go on next line\n".
			$line, $count);
	}
# check for malformed paths in #include statements
	if ($line =~ m{^.\s*\#\s*include\s+[<"](.*)[">]}) {
		my $path = $1;
		if ($path =~ m{//}) {
			ERROR("malformed #include filename\n" .
				$line, $count);
		}
	}
# check for static initialisers.
	if ($line =~ /\bstatic\s.*=\s*(0|NULL|false)\s*;/) {
		WARN("do not initialise statics to 0 or NULL\n" .
			$line, $count);
	}
# closing brace should have a space following it when it has anything
# on the line
	if ($line =~ /}(?!(?:,|;|\)))\S/) {
		ERROR("space required after close brace '}'\n" .
			$line, $count);
	}
# check spacing on square brackets
	if ($line =~ /\[\s/ && $line !~ /\[\s*$/) {
		ERROR("space prohibited after open square bracket '['\n" .
			$line, $count);
	}
	if ($line =~ /\s\]/) {
		ERROR("space prohibited before close square bracket ']'\n"
			. $line, $count);
	}
# check spacing on parentheses
	if ($line =~ /\(\s/ && $line !~ /\(\s*(?:\\)?$/ &&
			$line !~ /for\s*\(\s+;/) {
		ERROR("space prohibited after open parenthesis '('\n" .
			$line, $count);
	}
	if ($line =~ /(\s+)\)/ && $line !~ /^.\s*\)/ &&
			$line !~ /for\s*\(.*;\s+\)/ &&
			$line !~ /:\s+\)/) {
		ERROR("space prohibited before close parenthesis ')'\n" .
			$line, $count);
	}
# Need a space before open parenthesis after if, while etc
	if ($line =~ /\b(if|while|for|switch)\(/) {
		ERROR("space required before the open parenthesis '('\n" .
			$line, $count);
	}
# warn about #if 0
	if ($line =~ /^.\s*\#\s*if\s+0\b/) {
		WARN("if this code is redundant consider removing it\n" .
			$line, $count);
	}
# warn about spacing in #ifdefs
	if ($line =~ /^.\s*\#\s*(ifdef|ifndef|elif)\s\s+/) {
		ERROR("exactly one space required after that #$1\n" .
			$line, $count);
	}
# check for gcc specific __FUNCTION__
	if ($line =~ /__FUNCTION__/) {
		ERROR("use __func__ instead of gcc specific __FUNCTION__\n" .
			$line, $count);
	}
}

print "\ntotal: $error errors, $warn warnings, $count lines checked\n";

sub ERROR {
	print STDERR "ERROR at line $_[1]: $_[0]\n";
	$error++;
}

sub WARN {
	print STDERR "WARNING at line $_[1]: $_[0]\n";
	$warn++;
}
